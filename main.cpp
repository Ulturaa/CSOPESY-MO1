#include <iostream>
#include <sstream>
#include <string>
#include "SystemEmulator.h"

#ifdef _WIN32
#include <windows.h>
#endif

void printBanner() {
#ifdef _WIN32
    UINT oldCP = GetConsoleOutputCP();
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::cout << "  ___  ____   ___  ____  _____ ____  _  _\n";
    std::cout << " / __)/ ___)/ _ \\(  _ \\(  _  )/ ___)( \\/ )\n";
    std::cout << "( (__ \\___ \\( (_) ))___/ )(_)( \\___ \\ \\  /\n";
    std::cout << " \\___)(____/ \\___/(__)  (____/ (____/ (__/ \n\n";
    std::cout << "Welcome to CSOPESY Emulator!\n";
    std::cout << "Developers:\n  Del Gallego, Neil Patrick\n";
    std::cout << "Last updated: 01-18-2024\n\n";

#ifdef _WIN32
    SetConsoleOutputCP(oldCP);
#endif
}

void enterProcessScreen(const std::shared_ptr<Process>& proc) {
    while (true) {
        {
            std::lock_guard<std::mutex> lock(proc->processMutex);
            std::cout << "\nProcess name: " << proc->name << "\n";
            std::cout << "ID: " << proc->id << "\n";
            std::cout << "Logs:\n";
            for (const auto& log : proc->logs) {
                std::cout << log << "\n";
            }
            if (proc->state == ProcessState::FINISHED) {
                std::cout << "\nFinished!\n";
            } else {
                std::cout << "\nCurrent instruction line: " << proc->currentLine << "\n";
                std::cout << "Lines of code: " << proc->totalLines << "\n";
            }
        }

        std::cout << "\nroot:\\> ";
        std::string cmd;
        if (!std::getline(std::cin, cmd)) break;

        if (cmd == "process-smi") {
            continue;
        } else if (cmd == "exit") {
            break;
        } else if (!cmd.empty()) {
            std::cout << "Unknown screen command: " << cmd << "\n";
        }
    }
}

int main() {
    SystemEmulator emulator;
    printBanner();

    std::string input;
    while (true) {
        std::cout << "root:\\> ";
        if (!std::getline(std::cin, input)) break;

        std::stringstream ss(input);
        std::string cmd;
        ss >> cmd;

        if (cmd == "exit") {
            break;
        } else if (cmd == "initialize") {
            if (emulator.initialize()) {
                std::cout << "Processor configuration initialized successfully.\n";
            } else {
                std::cout << "Error: Could not load config.txt.\n";
            }
        } else if (!emulator.isInitialized()) {
            std::cout << "Please initialize the application first using 'initialize'.\n";
        } else if (cmd == "screen") {
            std::string flag, procName;
            ss >> flag >> procName;

            if (flag == "-s" && !procName.empty()) {
                emulator.createProcess(procName);
                auto proc = emulator.findProcess(procName);
                if (proc) enterProcessScreen(proc);
            } else if (flag == "-r" && !procName.empty()) {
                auto proc = emulator.findProcess(procName);
                if (proc && proc->state != ProcessState::FINISHED) {
                    enterProcessScreen(proc);
                } else {
                    std::cout << "Process " << procName << " not found.\n";
                }
            } else if (flag == "-ls") {
                std::cout << emulator.getScreenListOutput() << "\n";
            } else {
                std::cout << "Usage: screen -s <name> | screen -r <name> | screen -ls\n";
            }
        } else if (cmd == "scheduler-start") {
            emulator.startSchedulerGenerator();
            std::cout << "Batch process generator started.\n";
        } else if (cmd == "scheduler-stop") {
            emulator.stopSchedulerGenerator();
            std::cout << "Batch process generator stopped.\n";
        } else if (cmd == "report-util") {
            emulator.generateReportUtil();
        } else if (!cmd.empty()) {
            std::cout << "Unknown command: " << cmd << "\n";
        }
    }

    emulator.stop();
    return 0;
}
