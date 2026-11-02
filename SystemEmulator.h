#ifndef SYSTEMEMULATOR_H
#define SYSTEMEMULATOR_H

#include <memory>
#include <vector>
#include <string>
#include <mutex>
#include <thread>
#include <atomic>
#include "Config.h"
#include "Process.h"
#include "Scheduler.h"

class SystemEmulator {
private:
    Config config;
    bool initialized = false;

    std::unique_ptr<Scheduler> scheduler;
    std::vector<std::shared_ptr<Process>> allProcesses;
    std::mutex systemMutex;

    std::thread generatorThread;
    std::atomic<bool> generatorActive{false};
    std::atomic<bool> running{false};

    int processIdCounter = 1;

    void generatorLoop();

public:
    SystemEmulator() = default;
    ~SystemEmulator();

    bool initialize();
    bool isInitialized() const;
    void stop();

    void createProcess(const std::string& name);
    void startSchedulerGenerator();
    void stopSchedulerGenerator();

    std::shared_ptr<Process> findProcess(const std::string& name);
    std::string getScreenListOutput();
    void generateReportUtil();
};

#endif // SYSTEMEMULATOR_H
