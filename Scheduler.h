#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <memory>
#include "Process.h"
#include "Config.h"

class Scheduler {
public:
    virtual ~Scheduler() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void addProcess(std::shared_ptr<Process> proc) = 0;
    virtual int getUsedCoresCount() const = 0;
};

#endif // SCHEDULER_H