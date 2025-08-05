#include "RoundRobinScheduler.h"
#include "Process.h"
#include "ICommand.h"
#include "DemandPagingAllocator.h"

#include <thread>
#include <memory>
#include <iostream>
#include <chrono>

#include <fstream>
#include <ctime>
#include <algorithm>

RoundRobinScheduler::RoundRobinScheduler(int coreCount, int timeQuantum, int snapshotInterval, std::shared_ptr<DemandPagingAllocator> memoryManager)
    : numCores(coreCount), timeQuantum(timeQuantum), snapshotInterval(snapshotInterval), memoryManager(memoryManager) {
}

void RoundRobinScheduler::addProcess(std::shared_ptr<Process> process) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        readyQueue.push_back(process);
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

std::string RoundRobinScheduler::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t timeNow = std::chrono::system_clock::to_time_t(now);
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%m/%d/%Y, %I:%M:%S %p", std::localtime(&timeNow));
    return std::string(buffer);
}

void RoundRobinScheduler::coreWorker(int coreId) {    
    while (true) {
        std::shared_ptr<Process> process;

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            cv.wait(lock, [this]() { return stop || !readyQueue.empty(); });

            if (stop && readyQueue.empty()) break;
            process = readyQueue.front();
            readyQueue.pop_front();
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

            readyQueue.push_back(process);
            cv.notify_one();
        }
        else {
            memoryManager->deallocate(process->getPID());
        }
    }
}