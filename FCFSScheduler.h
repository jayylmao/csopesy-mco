#pragma once
#include "IScheduler.h"
#include "DemandPagingAllocator.h"
#include "ProcessManager.h"
#include <queue>
#include <vector>
#include <atomic>
#include <memory>
#include <thread>
#include <mutex>

class Process;

/**
 * @class FCFSScheduler
 * @brief Implements a multi-threaded First-Come-First-Serve process scheduler.
 */
class FCFSScheduler : public IScheduler {
public:
    FCFSScheduler(int cores, std::shared_ptr<DemandPagingAllocator> memoryManager, std::shared_ptr<ProcessManager> processManager);
    ~FCFSScheduler();

    /**
     * @brief Start the scheduler loop. Should be called once.
     */
    void runScheduler() override;

    /**
     * @brief Add a new process to the ready queue.
     * @param process Shared pointer to the process.
     */
    void addProcess(std::shared_ptr<Process> process) override;

    int getIdleTicks() const override;
    int getActiveTicks() const override;

    /*
    * @brief Stops the scheduler.
    */
    void stopScheduler() override;



private:
    int numCores;
    std::vector<std::thread> coreThreads;

    std::queue<std::shared_ptr<Process>> readyQueue;
    std::mutex queueMutex;
    std::condition_variable cv;

    std::atomic<bool> stop;
    std::shared_ptr<DemandPagingAllocator> memoryManager;
    std::shared_ptr<ProcessManager> processManager;
    std::atomic<int> idleTicks = 0;
    std::atomic<int> activeTicks = 0;

    /**
     * @brief Worker thread loop for each CPU core.
     * @param coreId ID of the current core.
     */
    void coreWorker(int coreId);
};
