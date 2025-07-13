#include "RoundRobinScheduler.h"
#include "Process.h"
#include "ICommand.h"
#include "FlatMemoryAllocator.h"

#include <thread>
#include <memory>
#include <iostream>
#include <chrono>

RoundRobinScheduler::RoundRobinScheduler(int coreCount, int timeQuantum, std::shared_ptr<IMemoryAllocator> memoryManager)
    : numCores(coreCount), timeQuantum(timeQuantum), memoryManager(memoryManager) {
}

void RoundRobinScheduler::addProcess(std::shared_ptr<Process> process) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        readyQueue.push(process);
    }
    cv.notify_one();
}

void RoundRobinScheduler::runScheduler() {
    for (int i = 0; i < numCores; ++i) {
        threads.emplace_back(&RoundRobinScheduler::coreWorker, this, i);
    }
}

void RoundRobinScheduler::stopScheduler() {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stop = true;
    }
    cv.notify_all();

    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }
}

void RoundRobinScheduler::coreWorker(int coreId) {
    while (true) {
        std::shared_ptr<Process> process;

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            cv.wait(lock, [this]() { return stop || !readyQueue.empty(); });

            if (stop && readyQueue.empty()) break;
            process = readyQueue.front();
            readyQueue.pop();
        }
        
        void* memBlock = memoryManager->allocate(process->getMemory());

        if (!memBlock) {
            std::cerr << "Could not allocate memory." << std::endl;
            continue;
        }

        process->setCoreId(coreId);
        int pid = process->getPID();
        std::string name = process->getName();
        
        int slice = 0;
        while (!process->hasFinished() && slice < timeQuantum) {
            process->executeInstruction();
            ++slice;
        }

        if (!process->hasFinished()) {
            process->setCoreId(-1);  // Reset core to "Pending"
            std::lock_guard<std::mutex> lock(queueMutex);
            readyQueue.push(process);
            cv.notify_one();
        }
        else {
            memoryManager->deallocate(memBlock);
        }
    }
}
