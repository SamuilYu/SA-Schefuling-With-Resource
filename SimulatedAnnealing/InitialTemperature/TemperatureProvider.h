#ifndef SA_SCHEDULING_RESOURCES_TEMPERATUREPROVIDER_H
#define SA_SCHEDULING_RESOURCES_TEMPERATUREPROVIDER_H

#include <utility>

#include "../Problem/Solution.h"
#include "../Problem/Conditions.h"
#include "stdexcept"
#include "math.h"

class TemperatureProvider {
public:
    virtual double getTemperature() = 0;
    virtual std::shared_ptr<TemperatureProvider> clone() = 0;
};

class StatisticalTemperatureProvider : public TemperatureProvider {
private:
    std::shared_ptr<Solution> solution;
    int N;
public:
    StatisticalTemperatureProvider(
            std::shared_ptr<Solution> solution,
            int n
    ) {
        this->solution = std::move(solution);
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
        return sqrt(variance);
    }

    std::shared_ptr<TemperatureProvider> clone() override {
        return std::make_shared<StatisticalTemperatureProvider>(solution->clone(), N);
    }
};

class RangeBasedTemperatureProvider : public TemperatureProvider{
private:
    std::shared_ptr<Conditions> conditions;
public:
    RangeBasedTemperatureProvider(
            std::shared_ptr<Conditions> conditions
    ) {
        this->conditions = std::move(conditions);
    }

    double getTemperature() override {
        double max = conditions->estimateMax();
        double min = conditions->estimateMin();
        return -(max - min);
    }

    std::shared_ptr<TemperatureProvider> clone() override {
        return std::make_shared<RangeBasedTemperatureProvider>(conditions->clone());
    }
};


#endif //SA_SCHEDULING_RESOURCES_TEMPERATUREPROVIDER_H
