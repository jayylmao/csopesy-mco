#include "RoundRobinScheduler.h"
#include "PrintCommand.h"
#include <iostream>
#include <chrono>
#include <thread>

RoundRobinScheduler::RoundRobinScheduler(int coreCount, int timeQuantum)
    : numCores(coreCount), timeQuantum(timeQuantum) {
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

        process->setCoreId(coreId);
        int pid = process->getPID();
        std::string name = process->getName();

        int slice = 0;
        while (!process->hasFinished() && slice < timeQuantum) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            std::string msg = "[" + std::to_string(process->getCurrentLine() + 1) + "] Hello world from " + name;
            PrintCommand cmd(msg, coreId, name, pid);
            cmd.execute();

            process->executeInstruction();
            ++slice;
        }

        if (!process->hasFinished()) {
            process->setCoreId(-1);  // Reset core to "Pending"
            std::lock_guard<std::mutex> lock(queueMutex);
            readyQueue.push(process);
            cv.notify_one();
        }
    }
}
