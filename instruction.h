#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>
#include <vector>
#include <cstdint>
#include <random>
#include <algorithm>

enum class InstructionType { PRINT, DECLARE, ADD, SUBTRACT, SLEEP, FOR };

struct Instruction {
    InstructionType type;
    std::string var1;
    std::string var2;
    std::string var3;
    uint16_t val1 = 0;
    uint16_t val2 = 0;
    uint8_t sleepTicks = 0;
    int loopCount = 1;
    std::vector<Instruction> subInstructions;
};

inline std::vector<Instruction> generateRandomInstructions(int count, const std::string& procName, int depth = 0) {
    std::vector<Instruction> list;
    static std::mt19937 rng(1337 + depth);
    std::uniform_int_distribution<int> typeDist(0, 4);

    for (int i = 0; i < count; ++i) {
        Instruction inst;
        // Restrict nested FOR loops at deeper levels to prevent exponential growth
        int choice = (depth >= 2) ? (typeDist(rng) % 4) : typeDist(rng);

        if (choice == 0) {
            inst.type = InstructionType::PRINT;
        } else if (choice == 1) {
            inst.type = InstructionType::DECLARE;
            inst.var1 = "x";
            inst.val1 = rng() % 100;
        } else if (choice == 2) {
            inst.type = InstructionType::ADD;
            inst.var1 = "x";
            inst.val1 = rng() % 50;
            inst.val2 = rng() % 50;
        } else if (choice == 3) {
            inst.type = InstructionType::SUBTRACT;
            inst.var1 = "x";
            inst.val1 = 100;
            inst.val2 = rng() % 50;
        } else if (choice == 4 && depth < 2) {
            inst.type = InstructionType::FOR;
            inst.loopCount = (rng() % 3) + 2; // Repeat 2-4 times max
            inst.subInstructions = generateRandomInstructions(std::max(1, count / 5), procName, depth + 1);
        } else {
            inst.type = InstructionType::SLEEP;
            inst.sleepTicks = (rng() % 5) + 1;
        }
        list.push_back(inst);
    }
    return list;
}

#endif // INSTRUCTION_H