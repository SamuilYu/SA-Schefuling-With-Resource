#ifndef SA_SCHEDULING_RESOURCES_TEMPERATUREPROVIDER_H
#define SA_SCHEDULING_RESOURCES_TEMPERATUREPROVIDER_H

#include "../Problem/Solution.h"
#include "../Problem/Conditions.h"
#include "stdexcept"

class TemperatureProvider {
private:
    double threshold;
public:
    TemperatureProvider(double threshold) {
        if (threshold > 1.0 || threshold < 0.0) {
            throw std::logic_error("Illegal threshold for probability");
        }
        this->threshold = threshold;
    }

    virtual double getTemperature() = 0;
};

class StatisticalTemperatureProvider : public TemperatureProvider {
private:
    Solution* solution;
    int N;
public:
    StatisticalTemperatureProvider(
            double threshold,
            Solution* solution,
            int n
    ) : TemperatureProvider(threshold) {
        this->solution = solution;
        this->N = n;
    }

    double getTemperature() override {
        double E = 1.0;
        double sum = 0.0;
        double sums = 0.0;
        double seed = 1000.0;
        for (int i = 0; i < N; i++) {
            E = solution->Perturb(seed, seed);
            sum += E;
            sums += (E * E);
        }
        double variance = sums / (N - 1) - (sum * sum) / (N * (N - 1));
        return -sqrt(variance) / (log(threshold) * seed);
    }
};

class RangeBasedTemperatureProvider : public TemperatureProvider{
private:
    Conditions* conditions;
public:
    RangeBasedTemperatureProvider(
            double threshold,
            Conditions* conditions
    ) : TemperatureProvider(threshold) {
        this->conditions = conditions;
    }

    double getTemperature() override {
        double max = conditions->estimateMax();
        double min = conditions->estimateMin();
        return -(max - min) / log(threshold) ;
    }
};


#endif //SA_SCHEDULING_RESOURCES_TEMPERATUREPROVIDER_H
