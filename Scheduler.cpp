#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <memory>

// Abstract class for Scheduler class
// Will be the skeleton for building the First Come First Serve or Round Robin scheduler
class Scheduler {
    private:
        int no_cores = 0;
        std::vector<std::thread> workerThreads;
        std::thread schedulerThread;

        std::queue<std::shared_ptr<Process>> readyQueue;
        std::vector<std::shared_ptr<Process>> allProcesses;

        std::mutex queueMutex;
        std::mutex fileMutex;
        std::condition_variable cvScheduler;
        std::condition_variable cvWorkers[NUM_CORES];

        std::shared_ptr<Process> coreAssignedProcess[NUM_CORES];
        bool coreBusy[NUM_CORES] = {false};
        std::atomic<bool> stopScheduler{false};

        std::string getCurrentFormattedTime() {
            auto now = std::chrono::system_clock::now();
            std::time_t in_time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&in_time_t), "%m/%d/%Y %I:%M:%S%p");
            return ss.str();
        }

    public:
    virtual void print() const = 0;
};

class FCFS : public Scheduler {


    public:
        virtual void print() const {
            std::cout << "Im FCFS\n";
        }
};

class RR : public Scheduler {


    public:
        virtual void print() const {
            std::cout << "Im FCFS\n";
        }
};