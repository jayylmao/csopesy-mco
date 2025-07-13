#pragma once
#include "IScheduler.h"
#include "IMemoryAllocator.h"
#include <vector>
#include <queue>
#include <mutex>

class Process;
class ICommand;

class RoundRobinScheduler : public IScheduler {
public:
    RoundRobinScheduler(int coreCount, int timeQuantum, std::shared_ptr<IMemoryAllocator> memoryManager);
    void addProcess(std::shared_ptr<Process> process) override;
    void runScheduler() override;
    void stopScheduler() override;

private:
    int numCores;
    int timeQuantum;
    bool stop = false;

    int maxMemory;
    int memPerFrame;

    std::queue<std::shared_ptr<Process>> readyQueue;
    std::mutex queueMutex;
    std::condition_variable cv;
    std::vector<std::thread> threads;

    std::shared_ptr<IMemoryAllocator> memoryManager;

    void coreWorker(int coreId);
};
