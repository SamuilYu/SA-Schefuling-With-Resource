#ifndef SA_SCHEDULING_RESOURCES_TEMPERARURE_H
#define SA_SCHEDULING_RESOURCES_TEMPERARURE_H

#include <stdexcept>

class CoolingSchedule {
protected:
    double value = 0.0;
    unsigned int index = 1;
    bool started = false;
    virtual void coolDown() = 0;
public:
    explicit CoolingSchedule() = default;

    double getNextTemperature() {
        started = true;
        this -> coolDown();
        return value;
    }

    virtual void setInitialTemperature(double val) {
        if (!started) {
            value = val;
        }
    }

    virtual void restart() = 0;
};

#endif //SA_SCHEDULING_RESOURCES_TEMPERARURE_H
