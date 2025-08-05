#include "FCFSScheduler.h"
#include "Process.h"
#include "FlatMemoryAllocator.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <string> 
#include <mutex>

FCFSScheduler::FCFSScheduler(int cores, std::shared_ptr<DemandPagingAllocator> memoryManager, std::shared_ptr<ProcessManager> processManager)
    : stop(false), numCores(cores), memoryManager(memoryManager), processManager(processManager)
{
    // Ensure valid core count
    if (cores < 1 || cores > 128) cores = 4;
    numCores = cores;
}

FCFSScheduler::~FCFSScheduler()
{
    stop = true;
    cv.notify_all();

    for (auto& t : coreThreads) {
        if (t.joinable()) {
            t.join();
        }
    }
}
void FCFSScheduler::stopScheduler() {
    stop = true;
    cv.notify_all();

    for (auto& t : coreThreads) {
        if (t.joinable()) {
            t.join();
        }
    }
}
void FCFSScheduler::runScheduler()
{
    for (int i = 0; i < numCores; ++i) {
        coreThreads.emplace_back(&FCFSScheduler::coreWorker, this, i);
    }
}

void FCFSScheduler::addProcess(std::shared_ptr<Process> process)
{
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        readyQueue.push(process);
    }
    cv.notify_one();
}

void FCFSScheduler::coreWorker(int coreId)
{
    while (!stop) {
        std::shared_ptr<Process> process;

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            cv.wait(lock, [this]() { return stop || !readyQueue.empty(); });

            if (stop) break;
            if (!readyQueue.empty()) {
                process = readyQueue.front();
                readyQueue.pop();
            }
        }

        process->setCoreId(coreId);

        while (!process->hasFinished()) {
            process->executeInstruction();
        }

        processManager->cleanupProcess(process->getName());
    }
}