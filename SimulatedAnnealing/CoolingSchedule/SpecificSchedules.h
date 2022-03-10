#ifndef SA_SCHEDULING_RESOURCES_SPECIFICSCHEDULES_H
#define SA_SCHEDULING_RESOURCES_SPECIFICSCHEDULES_H

#include "CoolingSchedule.h"
#include "cmath"

class GeometricCoolingSchedule: public CoolingSchedule {
private:
    double factor;
    void coolDown() override {
        value *= factor;
        index++;
    }
public:
    GeometricCoolingSchedule(double q) {
        factor = q;
    }
};

class CoolingScheduleFromInitialValue: public CoolingSchedule {
protected:
    double initialValue;
public:
    void setInitialTemperature(double val) override {
        if(!started) {
            CoolingSchedule::setInitialTemperature(val);
            initialValue = val;
        }
    }
};

class BoltzmannCoolingSchedule: public CoolingScheduleFromInitialValue {
private:
    void coolDown() override {
        value = initialValue / log(index + 1);
        index++;
    }
};


class CauchyCoolingSchedule: public CoolingScheduleFromInitialValue {
private:
    void coolDown() override {
        value = initialValue / (index + 1);
        index++;
    }
};

class HybridCoolingSchedule: public CoolingScheduleFromInitialValue {
private:
    void coolDown() override {
        value = initialValue * log(index + 1) / (index + 1);
        index++;
    }
};

#endif //SA_SCHEDULING_RESOURCES_SPECIFICSCHEDULES_H
