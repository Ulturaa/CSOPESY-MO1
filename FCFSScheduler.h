#ifndef FCFSSCHEDULER_H
#define FCFSSCHEDULER_H

#include "Scheduler.h"
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

class FCFSScheduler : public Scheduler {
private:
    Config config;
    std::vector<std::thread> workerThreads;
    std::thread schedulerThread;

    std::queue<std::shared_ptr<Process>> readyQueue;
    std::vector<std::shared_ptr<Process>> coreAssignments;
    std::vector<bool> coreBusy;

    mutable std::mutex queueMutex;
    std::condition_variable cvScheduler;
    std::vector<std::condition_variable> cvWorkers;

    bool running = false;

    void workerLoop(int coreId);
    void schedulerLoop();
    void executeInstruction(const std::shared_ptr<Process>& proc, const Instruction& inst, int coreId);
    uint32_t clampUint32(int64_t val);

public:
    FCFSScheduler(const Config& cfg);
    ~FCFSScheduler() override;

    void start() override;
    void stop() override;
    void addProcess(std::shared_ptr<Process> proc) override;
    int getUsedCoresCount() const override;
};

#endif