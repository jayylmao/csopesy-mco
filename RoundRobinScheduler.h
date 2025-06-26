#pragma once
#include "Process.h"
#include "ICommand.h"
#include "IScheduler.h"
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <iostream>
#include <chrono>

class RoundRobinScheduler : public IScheduler {
public:
    RoundRobinScheduler(int coreCount, int timeQuantum);
    void addProcess(std::shared_ptr<Process> process) override;
    void runScheduler() override;
    void stopScheduler() override;

private:
    int numCores;
    int timeQuantum;
    bool stop = false;

    std::queue<std::shared_ptr<Process>> readyQueue;
    std::mutex queueMutex;
    std::condition_variable cv;
    std::vector<std::thread> threads;

    void coreWorker(int coreId);
};
