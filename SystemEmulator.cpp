#include "SystemEmulator.h"
#include "FCFSScheduler.h"
#include "RoundRobinScheduler.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>

SystemEmulator::~SystemEmulator() {
    stop();
}

bool SystemEmulator::initialize() {
    if (initialized) {
        std::cout << "ERROR: System already initialized.\n";
        return false;
    }
    if (!config.load("config.txt")) {
        std::cout << "System failed to initialize.\n";
        return false;
    }

    if (config.scheduler == SchedulerAlgorithm::FCFS) {
        scheduler = std::unique_ptr<Scheduler>(new FCFSScheduler(config));
    } else {
        scheduler = std::unique_ptr<Scheduler>(new RoundRobinScheduler(config));
    }

    running = true;
    scheduler->start();
    generatorThread = std::thread(&SystemEmulator::generatorLoop, this);

    initialized = true;
    return true;
}

bool SystemEmulator::isInitialized() const {
    return initialized;
}

void SystemEmulator::stop() {
    if (!running) return;
    running = false;
    generatorActive = false;

    if (generatorThread.joinable()) generatorThread.join();
    if (scheduler) scheduler->stop();
}

void SystemEmulator::generatorLoop() {
    while (running) {
        // Sleep in seconds according to config.batchProcessFreq (1 tick = 1 second)
        std::this_thread::sleep_for(std::chrono::seconds(config.batchProcessFreq));
        if (!generatorActive) continue;

        std::stringstream ss;
        ss << "p" << std::setw(2) << std::setfill('0') << processIdCounter++;
        createProcess(ss.str());
    }
}

void SystemEmulator::createProcess(const std::string& name) {
    std::unique_lock<std::mutex> lock(systemMutex);
    int insCount = config.minIns;
    if (config.maxIns > config.minIns) {
        insCount += (rand() % (config.maxIns - config.minIns + 1));
    }

    auto insts = generateRandomInstructions(insCount, name);
    auto proc = std::make_shared<Process>(processIdCounter++, name, insts);
    allProcesses.push_back(proc);

    if (scheduler) {
        scheduler->addProcess(proc);
    }
}

void SystemEmulator::startSchedulerGenerator() {
    generatorActive = true;
}

void SystemEmulator::stopSchedulerGenerator() {
    generatorActive = false;
}

std::shared_ptr<Process> SystemEmulator::findProcess(const std::string& name) {
    std::lock_guard<std::mutex> lock(systemMutex);
    for (const auto& proc : allProcesses) {
        if (proc->name == name) return proc;
    }
    return nullptr;
}

std::string SystemEmulator::getScreenListOutput() {
    std::lock_guard<std::mutex> lock(systemMutex);
    std::stringstream ss;

    int usedCores = scheduler ? scheduler->getUsedCoresCount() : 0;
    int availableCores = config.numCpu - usedCores;
    double utilization = (config.numCpu > 0) ? ((double)usedCores / config.numCpu) * 100.0 : 0.0;

    ss << "CPU utilization: " << (int)utilization << "%\n";
    ss << "Cores used: " << usedCores << "\n";
    ss << "Cores available: " << availableCores << "\n\n";
    ss << "--------------------------------------------------\n";
    ss << "Running processes:\n";

    for (const auto& proc : allProcesses) {
        if (proc->state == ProcessState::RUNNING) {
            ss << std::left << std::setw(12) << proc->name
               << "(" << proc->creationTime << ")    "
               << "Core: " << proc->assignedCore << "    "
               << proc->currentLine << " / " << proc->totalLines << "\n";
        }
    }

    ss << "\nFinished processes:\n";
    for (const auto& proc : allProcesses) {
        if (proc->state == ProcessState::FINISHED) {
            ss << std::left << std::setw(12) << proc->name
               << "(" << proc->creationTime << ")    "
               << "Finished    "
               << proc->currentLine << " / " << proc->totalLines << "\n";
        }
    }
    ss << "--------------------------------------------------\n";
    return ss.str();
}

void SystemEmulator::generateReportUtil() {
    std::ofstream report("csopesy-log.txt");
    if (report.is_open()) {
        report << "CSOPESY Utilization Report\n";
        report << getScreenListOutput();
        report.close();
        std::cout << "Report generated at C:/csopesy-log.txt!\n";
    } else {
        std::cout << "Failed to generate report file.\n";
    }
}