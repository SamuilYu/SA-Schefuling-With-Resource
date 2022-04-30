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

    std::shared_ptr<CoolingSchedule> clone() override {
        return std::make_shared<GeometricCoolingSchedule>(factor);
    }


public:
    void restart() override {
        for (index; 1 < index; index--) {
            value = value / factor;
        }
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

    void restart() override {
        index = 1;
    }
};

class BoltzmannCoolingSchedule: public CoolingScheduleFromInitialValue {
private:
    void coolDown() override {
        value = initialValue / log(index + 1);
        index++;
    }

public:
    std::shared_ptr<CoolingSchedule> clone() override {
        return std::make_shared<BoltzmannCoolingSchedule>();
    }
};


class CauchyCoolingSchedule: public CoolingScheduleFromInitialValue {
private:
    void coolDown() override {
        value = initialValue / (index + 1);
        index++;
    }

public:
    std::shared_ptr<CoolingSchedule> clone() override {
        return std::make_shared<CauchyCoolingSchedule>();
    }
};

class HybridCoolingSchedule: public CoolingScheduleFromInitialValue {
private:
    void coolDown() override {
        value = initialValue * log(index + 1) / (index + 1);
        index++;
    }

public:
    std::shared_ptr<CoolingSchedule> clone() override {
        return std::make_shared<HybridCoolingSchedule>();
    }
};

#endif //SA_SCHEDULING_RESOURCES_SPECIFICSCHEDULES_H
