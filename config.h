#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <fstream>
#include <cstdint>

enum class SchedulerAlgorithm { FCFS, RR };

struct Config {
    int numCpu = 4;
    SchedulerAlgorithm scheduler = SchedulerAlgorithm::RR;
    uint32_t quantumCycles = 5;
    uint32_t batchProcessFreq = 1;
    uint32_t minIns = 1000;
    uint32_t maxIns = 2000;
    uint32_t delayPerExec = 0;

    bool load(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) return false;

        std::string key, val;
        while (file >> key >> val) {
            if (!val.empty() && (val.front() == '"' || val.front() == '\'')) {
                val = val.substr(1, val.size() - 2);
            }
            if (key == "num-cpu") {
                numCpu = std::stoi(val);
            } else if (key == "scheduler") {
                scheduler = (val == "fcfs") ? SchedulerAlgorithm::FCFS : SchedulerAlgorithm::RR;
            } else if (key == "quantum-cycles") {
                quantumCycles = std::stoul(val);
            } else if (key == "batch-process-freq") {
                batchProcessFreq = std::stoul(val);
            } else if (key == "min-ins") {
                minIns = std::stoul(val);
            } else if (key == "max-ins") {
                maxIns = std::stoul(val);
            } else if (key == "delay-per-exec" || key == "delays-per-exec") {
                delayPerExec = std::stoul(val);
            }
        }
        return true;
    }
};

#endif // CONFIG_H
