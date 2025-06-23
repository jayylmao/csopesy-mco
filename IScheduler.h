// IScheduler.h
#ifndef ISCHEDULER_H
#define ISCHEDULER_H

#include <memory>
#include "Process.h"

class IScheduler {
public:
    virtual ~IScheduler() = default;
    virtual void addProcess(std::shared_ptr<Process> process) = 0;
    virtual void runScheduler() = 0;
    virtual void stopScheduler() = 0;
};

#endif // ISCHEDULER_H