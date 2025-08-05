#pragma once
#include "IScheduler.h"
#include "DemandPagingAllocator.h"
#include "ProcessManager.h"
#include <vector>
#include <deque>
#include <mutex>
#include <atomic>

class Process;
class ICommand;

class RoundRobinScheduler : public IScheduler {
public:
    RoundRobinScheduler(int coreCount, int timeQuantum, int snapshotInterval, std::shared_ptr<DemandPagingAllocator> memoryManager, std::shared_ptr<ProcessManager> processManager);
    void addProcess(std::shared_ptr<Process> process) override;
    void runScheduler() override;
    void stopScheduler() override;

private:
    int numCores;
    int timeQuantum;
    
    bool stop = false;

    int snapshotInterval;

    std::mutex snapshotMutex;

    std::deque<std::shared_ptr<Process>> readyQueue;
    std::mutex queueMutex;
    std::condition_variable cv;
    std::vector<std::thread> threads;

    std::shared_ptr<DemandPagingAllocator> memoryManager;
    std::shared_ptr<ProcessManager> processManager;

    void coreWorker(int coreId);

    std::string getCurrentTimestamp();

    std::atomic<int> globalQuantumCounter{ 0 };
    std::atomic<bool> quantumStop{ false };
};