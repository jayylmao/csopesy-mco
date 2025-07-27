#include "RoundRobinScheduler.h"
#include "Process.h"
#include "ICommand.h"
#include "FlatMemoryAllocator.h"

#include <thread>
#include <memory>
#include <iostream>
#include <chrono>

#include <fstream>
#include <ctime>
#include <algorithm>

RoundRobinScheduler::RoundRobinScheduler(int coreCount, int timeQuantum, int snapshotInterval, std::shared_ptr<IMemoryAllocator> memoryManager)
    : numCores(coreCount), timeQuantum(timeQuantum), snapshotInterval(snapshotInterval), memoryManager(memoryManager) {
}

void RoundRobinScheduler::addProcess(std::shared_ptr<Process> process) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        readyQueue.push_front(process);
    }
    cv.notify_one();
}

void RoundRobinScheduler::runScheduler() {
    for (int i = 0; i < numCores; ++i) {
        threads.emplace_back(&RoundRobinScheduler::coreWorker, this, i);
    }
    quantumThread = std::thread(&RoundRobinScheduler::quantumTracker, this);
}

void RoundRobinScheduler::quantumTracker() {

    while (!quantumStop) {
        std::this_thread::sleep_for(std::chrono::milliseconds(4000)); // Adjustable delay between cycles
        int current = ++globalQuantumCounter;

        if (globalQuantumCounter % snapshotInterval == 0) {
            std::lock_guard<std::mutex> lock(snapshotMutex);
            std::ofstream file("memory_stamp_" + std::to_string(globalQuantumCounter) + ".txt");

            file << "TimeStamp: (" << getCurrentTimestamp() << ")\n";
            file << "Number of processes in memory: " << memoryManager->getProcessCount() << "\n";
            file << "Total External fragmentation in KB: "
                << memoryManager->getExternalFragmentation() / 1024 << "\n\n";

            auto flatMem = std::dynamic_pointer_cast<FlatMemoryAllocator>(memoryManager);
            if (!flatMem) {
                file << "[Error: Memory manager is not FlatMemoryAllocator]\n";
                return;
            }

            auto blocks = flatMem->getBlocks();
            std::sort(blocks.begin(), blocks.end(), [](const MemoryBlock& a, const MemoryBlock& b) {
                return (a.start + a.size) > (b.start + b.size);
                });

            file << "----end---- = " << flatMem->getMaxSize() << "\n\n";
            for (const auto& block : blocks) {
                file << block.start + block.size << "\n";
                file << "P" << block.pid << "\n";
                file << block.start << "\n\n";
            }
            file << "----start---- = 0\n";
            file.close();
        }
    }
}

void RoundRobinScheduler::stopScheduler() {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stop = true;
       
    } 
    quantumStop = true;
        
    cv.notify_all();

    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }
    
    if (quantumThread.joinable()) quantumThread.join();
}

std::string RoundRobinScheduler::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t timeNow = std::chrono::system_clock::to_time_t(now);
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%m/%d/%Y, %I:%M:%S %p", std::localtime(&timeNow));
    return std::string(buffer);
}

void RoundRobinScheduler::coreWorker(int coreId) {
    void* memBlock;
    
    while (true) {
        timeQuantum++;
        std::shared_ptr<Process> process;

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            cv.wait(lock, [this]() { return stop || !readyQueue.empty(); });

            if (stop && readyQueue.empty()) break;
            process = readyQueue.front();
            readyQueue.pop_front();
            memBlock = memoryManager->allocate(process->getMemory(), process->getPID());
        }

        

        if (!memBlock) {
            //std::cerr << "Could not allocate memory for PID " << process->getPID() << "\n";
            std::lock_guard<std::mutex> lock(queueMutex);
            readyQueue.push_back(process); // Put it back in the queue
            cv.notify_one();
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

            readyQueue.push_back(process);
            cv.notify_one();
        }
        else {
            memoryManager->deallocate(process->getPID());
        }
    }
}