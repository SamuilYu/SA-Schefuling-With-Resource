#ifndef SA_SCHEDULING_RESOURCES_TEMPERATUREPROVIDER_H
#define SA_SCHEDULING_RESOURCES_TEMPERATUREPROVIDER_H

#include "../Problem/Solution.h"
#include "../Problem/Conditions.h"
#include "stdexcept"
#include "math.h"

class TemperatureProvider {
public:
    virtual double getTemperature() = 0;
};

class StatisticalTemperatureProvider : public TemperatureProvider {
private:
    Solution* solution;
    int N;
public:
    StatisticalTemperatureProvider(
            Solution* solution,
            int n
    ) {
        this->solution = solution;
        this->N = n;
    }

    double getTemperature() override {
        double E = 1.0;
        double sum = 0.0;
        double sums = 0.0;
        double seed = 1000.0;
        solution -> Initialize();
        for (int i = 0; i < N; i++) {
            E = solution->Perturb(seed, seed);
            sum += E;
            sums += (E * E);
        }
        double variance = sums / (N - 1) - (sum * sum) / (N * (N - 1));
        return -sqrt(variance) / seed;
    }
};

class RangeBasedTemperatureProvider : public TemperatureProvider{
private:
    Conditions* conditions;
public:
    RangeBasedTemperatureProvider(
            Conditions* conditions
    ) {
        this->conditions = conditions;
    }

    double getTemperature() override {
        double max = conditions->estimateMax();
        double min = conditions->estimateMin();
        return -(max - min);
    }
};


#endif //SA_SCHEDULING_RESOURCES_TEMPERATUREPROVIDER_H
