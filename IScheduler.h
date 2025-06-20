#ifndef ISCHEDULER_H
#define ISCHEDULER_H

#include <memory>
#include "Process.h"

class IScheduler {
public:
    virtual ~IScheduler() = default;

    // Add a new process to the scheduler
    virtual void addProcess(std::shared_ptr<Process> process) = 0;

    // Start running the scheduler
    virtual void runScheduler() = 0;
};

#endif // ISCHEDULER_H
