#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <fstream>
#include <cstdint>
#include <iostream>
#include <stdexcept>

enum class SchedulerAlgorithm { FCFS, RR };

struct Config {
    int numCpu = 4;
    SchedulerAlgorithm scheduler = SchedulerAlgorithm::RR;
    uint32_t quantumCycles = 5;
    uint32_t batchProcessFreq = 1;
    uint32_t minIns = 1000;
    uint32_t maxIns = 2000;
    uint32_t delayPerExec = 0;

    const unsigned long long MAX_R = 4294967295ULL;     // Max acceptable range for config

    std::string key, val;
    bool load(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cout << "ERROR: Config file not found";
            return false;
        }

        std::string key, val;
        while (file >> key >> val) {
            try {
                if (!val.empty() && (val.front() == '"' || val.front() == '\'')) {
                    val = val.substr(1, val.size() - 2);
                }
                if (key == "num-cpu") {
                    if(std::stoi(val) < 1 || std::stoi(val) > 128) {
                        std::cout << "ERROR: Num. of CPU cores outside of range (1-128).";
                        return false;
                    } else
                        numCpu = std::stoi(val);
                } else if (key == "scheduler") {
                    if(val != "fcfs" && val != "rr") {
                        std::cout << "ERROR: Invalid CPU Scheduler type.";
                        return false;
                    } else 
                        scheduler = (val == "fcfs") ? SchedulerAlgorithm::FCFS : SchedulerAlgorithm::RR;
                } else if (key == "quantum-cycles") {
                    if(std::stoi(val) < 1 || std::stoi(val) > MAX_R) {
                        std::cout << "ERROR: Num. of Quantum cycles outside of range (1-2^32).";
                        return false;
                    } else
                        quantumCycles = std::stoul(val);
                } else if (key == "batch-process-freq") {
                    if(std::stoi(val) < 1 || std::stoi(val) > MAX_R) {
                        std::cout << "ERROR: Batch process frequency outside of acceptable range (1-2^32)).";
                        return false;
                    } else
                        batchProcessFreq = std::stoul(val);
                } else if (key == "min-ins") {
                    if(std::stoi(val) < 1 || std::stoi(val) > MAX_R) {
                        std::cout << "ERROR: Num. of min instructions outside of range outside of range (1-2^32).";
                        return false;
                    } else
                        minIns = std::stoul(val);
                } else if (key == "max-ins") {
                    if(std::stoi(val) < 1 || std::stoi(val) > MAX_R) {
                        std::cout << "ERROR: Num. of max instructions outside of range outside of range (1-2^32).";
                        return false;
                    } else
                        maxIns = std::stoul(val);
                } else if (key == "delay-per-exec" || key == "delays-per-exec") {
                    if(std::stoi(val) < 1 || std::stoi(val) > MAX_R) {
                        std::cout << "ERROR: Num. of delay outside of range outside of range (1-2^32).";
                        return false;
                    } else
                        delayPerExec = std::stoul(val);
                }
            } catch (const std::invalid_argument&) {
                std::cout << "ERROR: Value '" << val << "' for '" << key << "' is not a valid number.";
                return false;
            } catch (const std::out_of_range&) {
                std::cout << "ERROR: Value '" << val << "' for '" << key << "' is out of range.";
                return false;
            }
        }
        return true;
    }
};

#endif // CONFIG_H
