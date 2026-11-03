#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cstdint>
#include "Instruction.h"

enum class ProcessState { READY, RUNNING, FINISHED };

class Process {
public:
    int id;
    std::string name;
    std::string creationTime;
    std::vector<Instruction> flattenedInstructions;
    
    std::atomic<int> currentLine{0};
    int totalLines = 0;
    std::atomic<int> assignedCore{-1};
    std::atomic<ProcessState> state{ProcessState::READY};

    std::vector<std::string> logs;
    std::map<std::string, uint16_t> variables;
    std::mutex processMutex;

    Process(int pId, const std::string& pName, const std::vector<Instruction>& insts, int maxAllowedLines = 100)
        : id(pId), name(pName) {
        
        auto now = std::chrono::system_clock::now();
        std::time_t in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%m/%d/%Y %I:%M:%S%p");
        creationTime = ss.str();

        // Safely unroll instructions up to maxAllowedLines (prevents bad_alloc)
        flattenInstructions(insts, maxAllowedLines);
        totalLines = static_cast<int>(flattenedInstructions.size());
    }

    void addLog(const std::string& timestamp, int coreId, const std::string& msg) {
        std::lock_guard<std::mutex> lock(processMutex);
        std::stringstream ss;
        ss << "(" << timestamp << ") Core:" << coreId << " \"" << msg << "\"";
        logs.push_back(ss.str());
    }

private:
    void flattenInstructions(const std::vector<Instruction>& list, int maxAllowedLines) {
        for (const auto& inst : list) {
            if (static_cast<int>(flattenedInstructions.size()) >= maxAllowedLines) {
                break; // Stop unrolling if line cap is reached
            }

            if (inst.type == InstructionType::FOR) {
                for (int r = 0; r < inst.loopCount; ++r) {
                    if (static_cast<int>(flattenedInstructions.size()) >= maxAllowedLines) break;
                    flattenInstructions(inst.subInstructions, maxAllowedLines);
                }
            } else {
                flattenedInstructions.push_back(inst);
            }
        }
    }
};

#endif // PROCESS_H