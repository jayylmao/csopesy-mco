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
<<<<<<< Updated upstream
    RoundRobinScheduler(int coreCount, int timeQuantum);
=======
    RoundRobinScheduler(int coreCount, int timeQuantum, int snapshotInterval, std::shared_ptr<IMemoryAllocator> memoryManager);

>>>>>>> Stashed changes
    void addProcess(std::shared_ptr<Process> process) override;
    void runScheduler() override;
    void stopScheduler() override;

private:
    int numCores;
    int timeQuantum;
    bool stop = false;

<<<<<<< Updated upstream
=======
    int snapshotInterval;

    int currentQuantum = 0;
    std::mutex snapshotMutex;

>>>>>>> Stashed changes
    std::queue<std::shared_ptr<Process>> readyQueue;
    std::mutex queueMutex;
    std::condition_variable cv;
    std::vector<std::thread> threads;

<<<<<<< Updated upstream
=======
    std::shared_ptr<IMemoryAllocator> memoryManager;


>>>>>>> Stashed changes
    void coreWorker(int coreId);

    std::string getCurrentTimestamp();
};
