#include "FCFSScheduler.h"
#include <chrono>
#include <iostream>

FCFSScheduler::FCFSScheduler(const Config& cfg) : config(cfg) {
    cvWorkers = std::vector<std::condition_variable>(config.numCpu);
    coreAssignments = std::vector<std::shared_ptr<Process>>(config.numCpu, nullptr);
    coreBusy = std::vector<bool>(config.numCpu, false);
}

FCFSScheduler::~FCFSScheduler() {
    stop();
}

uint16_t FCFSScheduler::clampUint16(int32_t val) {
    if (val < 0) return 0;
    if (val > 65535) return 65535;
    return static_cast<uint16_t>(val);
}

void FCFSScheduler::executeInstruction(const std::shared_ptr<Process>& proc, const Instruction& inst, int coreId) {
    auto getCurrentTimeFormatted = []() {
        auto now = std::chrono::system_clock::now();
        std::time_t in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%m/%d/%Y %I:%M:%S%p");
        return ss.str();
    };

    if (inst.type == InstructionType::PRINT) {
        proc->addLog(getCurrentTimeFormatted(), coreId, "Hello world from " + proc->name + "!");
    } else if (inst.type == InstructionType::DECLARE) {
        proc->variables[inst.var1] = clampUint16(inst.val1);
        //proc->addLog(getCurrentTimeFormatted(), coreId, "DECLARE from " + proc->name + " | var = " + std::to_string(inst.val1));
    } else if (inst.type == InstructionType::ADD) {
        int res = proc->variables[inst.var1] + inst.val1 + inst.val2;
        proc->variables[inst.var1] = clampUint16(res);
        //proc->addLog(getCurrentTimeFormatted(), coreId, "ADD from " + proc->name + " | " + inst.var2 + " + " + inst.var3 + " = " + inst.var1);
    } else if (inst.type == InstructionType::SUBTRACT) {
        int res = proc->variables[inst.var1] - inst.val1 - inst.val2;
        proc->variables[inst.var1] = clampUint16(res);
        //proc->addLog(getCurrentTimeFormatted(), coreId, "SUBTRACT from " + proc->name + " | " + inst.var2 + " - " + inst.var3 + " = " + inst.var1);
    } else if (inst.type == InstructionType::SLEEP) {
        //proc->addLog(getCurrentTimeFormatted(), coreId, "SLEEP from " + pro->name);
        std::this_thread::sleep_for(std::chrono::milliseconds(inst.sleepTicks * 10));
    }
}

void FCFSScheduler::workerLoop(int coreId) {
    while (running) {
        std::shared_ptr<Process> proc = nullptr;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            cvWorkers[coreId].wait(lock, [this, coreId]() {
                return !running || (coreBusy[coreId] && coreAssignments[coreId] != nullptr);
            });

            if (!running) break;
            proc = coreAssignments[coreId];
        }

        if (proc) {
            while (proc->currentLine < proc->totalLines && running) {
                executeInstruction(proc, proc->flattenedInstructions[proc->currentLine], coreId);
                proc->currentLine++;

                if (config.delayPerExec > 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(config.delayPerExec * 5));
                }
            }

            {
                std::unique_lock<std::mutex> lock(queueMutex);
                proc->state = ProcessState::FINISHED;
                coreAssignments[coreId] = nullptr;
                coreBusy[coreId] = false;
                cvScheduler.notify_one();
            }
        }
    }
}

void FCFSScheduler::schedulerLoop() {
    while (running) {
        std::unique_lock<std::mutex> lock(queueMutex);
        cvScheduler.wait(lock, [this]() {
            return !running || !readyQueue.empty();
        });

        if (!running) break;

        for (int coreId = 0; coreId < config.numCpu; ++coreId) {
            if (!coreBusy[coreId] && !readyQueue.empty()) {
                auto proc = readyQueue.front();
                readyQueue.pop();

                proc->state = ProcessState::RUNNING;
                proc->assignedCore = coreId;
                coreAssignments[coreId] = proc;
                coreBusy[coreId] = true;

                cvWorkers[coreId].notify_one();
            }
        }
    }
}

void FCFSScheduler::start() {
    running = true;
    for (int i = 0; i < config.numCpu; ++i) {
        workerThreads.emplace_back(&FCFSScheduler::workerLoop, this, i);
    }
    schedulerThread = std::thread(&FCFSScheduler::schedulerLoop, this);
}

void FCFSScheduler::stop() {
    if (!running) return;
    running = false;
    cvScheduler.notify_all();
    for (auto& cv : cvWorkers) cv.notify_all();

    if (schedulerThread.joinable()) schedulerThread.join();
    for (auto& t : workerThreads) {
        if (t.joinable()) t.join();
    }
}

void FCFSScheduler::addProcess(std::shared_ptr<Process> proc) {
    std::unique_lock<std::mutex> lock(queueMutex);
    readyQueue.push(proc);
    cvScheduler.notify_one();
}

int FCFSScheduler::getUsedCoresCount() const {
    int count = 0;
    for (bool busy : coreBusy) {
        if (busy) count++;
    }
    return count;
}
