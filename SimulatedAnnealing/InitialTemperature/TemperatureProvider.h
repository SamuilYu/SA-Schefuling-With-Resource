#ifndef SA_SCHEDULING_RESOURCES_TEMPERATUREPROVIDER_H
#define SA_SCHEDULING_RESOURCES_TEMPERATUREPROVIDER_H

#include <utility>

#include "../Problem/Solution.h"
#include "../Problem/Conditions.h"
#include "../AcceptanceDistribution/AcceptanceDistribution.h"
#include "stdexcept"
#include <cmath>

class TemperatureProvider {
public:
    virtual double getTemperature() = 0;
    virtual std::shared_ptr<TemperatureProvider> clone() = 0;
};

class StatisticalTemperatureProvider : public TemperatureProvider {
private:
    std::shared_ptr<Solution> solution;
    std::shared_ptr<AcceptanceDistribution> dist;
    double threshold;
    int N;
public:
    StatisticalTemperatureProvider(
            std::shared_ptr<Solution> solution,
            std::shared_ptr<AcceptanceDistribution> dist,
            double threshold,
            int n
    ): threshold(threshold), N(n) {
        if (threshold > 1.0 || threshold < 0.0) {
            throw std::logic_error("Invalid threshold");
        }
        this->solution = std::move(solution);
        this -> dist = std::move(dist);
    }

    double getTemperature() override {
        double E;
        double prevE;
        double sum = 0.0;
        prevE = solution -> Initialize();
        double result = 1.0;
        int count = N;
        double delta = 0.0;
        while (threshold > sum/count || count == 0) {
            sum = 0.0;
            count = 0;
            for (int i = 0; i < N; i++) {
                E = solution->Perturb(result, result);
                if (E > prevE) {
                    delta = E-prevE;
                    sum += dist->getProbability(result, delta);
                    count++;
                }
                prevE = E;
            }
            result += delta;
        }

        return result;
    }

    std::shared_ptr<TemperatureProvider> clone() override {
        return std::make_shared<StatisticalTemperatureProvider>(solution->clone(), dist->clone(), threshold, N);
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
        double delta = conditions -> estimateMaxDelta();
        return delta;
    }

    std::shared_ptr<TemperatureProvider> clone() override {
        return std::make_shared<RangeBasedTemperatureProvider>(conditions->clone());
    }
};


#endif //SA_SCHEDULING_RESOURCES_TEMPERATUREPROVIDER_H
