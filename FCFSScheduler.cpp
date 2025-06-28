#include "FCFSScheduler.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <string> 

FCFSScheduler::FCFSScheduler(int cores)
    : stop(false)
{
    // Ensure valid core count
    if (cores < 1) cores = 1;
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

        if (process) {
            process->setCoreId(coreId);
            int pid = process->getPID();

            for (int i = 0; i < process->getTotalLines(); ++i) {
                process->executeInstruction();
            }
        }
    }
}