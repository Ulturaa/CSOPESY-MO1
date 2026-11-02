#include <string>

enum class ProcessState { NEW, READY, RUNNING, FINISHED };

class Process {
    private:
        std::string name;
        ProcessState pState = NEW;
        std::string creationTime;
        int totalInstructions = 100;
        std::atomic<int> executedInstructions{0};
        std::atomic<int> assignedCore{-1};
        std::atomic<ProcessState> state{ProcessState::READY};

    public:
        Process(const std::string& pName, int cmdCount = 100)
            : name(pName), totalInstructions(cmdCount) {
            auto now = std::chrono::system_clock::now();
            std::time_t in_time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&in_time_t), "%m/%d/%Y %I:%M:%S%p");
            creationTime = ss.str();
        }

        virtual void executeP(){
            cout << "Process Started" << endl;
        }

        void setState(ProcessState ps){
            this->pState = ps;
        }

        void assignProcess(int coreNum){
            this->assignedCore = core;
        }
    }
}

class PRINT: public Process{
    private:
        std::string text = "";
    public:
        Process(const std::string& pName, int cmdCount = 100), name(pName), totalInstructions(cmdCount) {
            auto now = std::chrono::system_clock::now();
            std::time_t in_time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&in_time_t), "%m/%d/%Y %I:%M:%S%p");
            creationTime = ss.str();
        }

        void executeP() {
            std::cout << this->text << endl;
        }

        std::string getName() {
            return name;
        }
}