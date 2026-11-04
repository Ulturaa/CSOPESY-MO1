#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>
#include <vector>
#include <cstdint>
#include <random>

enum class InstructionType { PRINT, DECLARE, ADD, SUBTRACT, SLEEP, FOR };

struct Instruction {
    InstructionType type;
    std::string var1;
    uint16_t val1 = 0;
    uint16_t val2 = 0;
    uint8_t sleepTicks = 0;
    int loopCount = 1;
    std::vector<Instruction> subInstructions;
};

inline std::vector<Instruction> generateRandomInstructions(int count, const std::string& procName) {
    std::vector<Instruction> list;
    static std::mt19937 rng(1337);

    for (int i = 0; i < count; ++i) {
        Instruction inst;
        if (i % 2 == 0) {
            // Alternating PRINT for Question 5 spec
            inst.type = InstructionType::PRINT;
            inst.var1 = "x";
        } else {
            // Alternating ADD(x, 1..10) for Question 5 spec
            inst.type = InstructionType::ADD;
            inst.var1 = "x";
            inst.val1 = (rng() % 10) + 1;
        }
        list.push_back(inst);
    }
    return list;
}

#endif // INSTRUCTION_H