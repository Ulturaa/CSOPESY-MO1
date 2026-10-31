// Abstract class for Scheduler class
// Will be the skeleton for building the First Come First Serve or Round Robin scheduler
class Scheduler {
    private:

    public:
    virtual void print() const = 0;
};

class FCFS : public Scheduler {


    public:
        virtual void print() const {
            std::cout << "Im FCFS\n";
        }
};