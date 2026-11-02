#ifndef FCFSSCHEDULER_H
#define FCFSSCHEDULER_H

#include "Scheduler.h"
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

class FCFSScheduler : public Scheduler {
private:
    Config config;
    std::vector<std::thread> workerThreads;
    std::thread schedulerThread;

    std::queue<std::shared_ptr<Process>> readyQueue;
    std::mutex queueMutex;
    std::condition_variable cvScheduler;
    std::vector<std::condition_variable> cvWorkers;

    std::vector<std::shared_ptr<Process>> coreAssignments;
    std::vector<bool> coreBusy;
    std::atomic<bool> running{false};

    uint16_t clampUint16(int32_t val);
    void executeInstruction(const std::shared_ptr<Process>& proc, const Instruction& inst, int coreId);
    void workerLoop(int coreId);
    void schedulerLoop();

public:
    FCFSScheduler(const Config& cfg);
    ~FCFSScheduler() override;

    void start() override;
    void stop() override;
    void addProcess(std::shared_ptr<Process> proc) override;
    int getUsedCoresCount() const override;
};

#endif // FCFSSCHEDULER_H
