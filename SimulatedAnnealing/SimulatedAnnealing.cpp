#include <iostream>
#include <utility>
#include "SimulatedAnnealing.h"
#include "CoolingSchedule/CoolingSchedule.h"
#include "InitialTemperature/TemperatureProvider.h"
#include "AcceptanceDistribution/AcceptanceDistribution.h"

SimulatedAnnealing::SimulatedAnnealing(
        std::shared_ptr<CoolingSchedule> schedule,
        const std::shared_ptr<TemperatureProvider>& temperatureProvider,
        std:: shared_ptr<AcceptanceDistribution> acceptance,
        int numTemps,
        int numIterations
    ) {
    this -> numTemps = numTemps;
    this -> numIterations = numIterations;
    initialTemp = temperatureProvider->getTemperature();
    coolingSchedule = std::move(schedule);
    coolingSchedule -> setInitialTemperature(initialTemp);
    acceptanceDist = std::move(acceptance);
    finalTemp = 0.0001;
}

double SimulatedAnnealing::Start(std::shared_ptr<Solution> solution, std::shared_ptr<Solution> wk1, int cycles) {
    double sum = 0;
    double sums = 0;
    int n = cycles;
    for (int i = 0; i < n; i++) {
        Anneal(solution, wk1);
        auto error = solution -> GetError();
        sum += error;
        sums += error * error;
        std::cout << "Did " << i << "th time: " << error << std::endl;
    }
    std::cout << sums / (n - 1) << std::endl << sum * sum / n / (n - 1) << std::endl  <<
    sqrt(sums / (n - 1) - sum * sum / n / (n - 1)) / (sum / n) << std::endl;
    return solution->GetError();
}

double SimulatedAnnealing::Anneal(std::shared_ptr<Solution> solution, std::shared_ptr<Solution> wk1) {
    double error = solution->Initialize();
    double newError;
    double maxError = error;
    double minError = error;

    (*wk1) = (*solution);
//    wk2 = solution;
    double temperature, deltaError;
    coolingSchedule -> restart();
    for (int n = 0; n < numTemps; n++) {
        temperature = coolingSchedule->getNextTemperature();
        for (int i = 0; i < numIterations; i++) {
            newError = wk1->Perturb(temperature, initialTemp);
            deltaError = newError - error;
            if (acceptanceDist -> isAccepted(temperature, deltaError)) {
                error = newError;
                if (error < minError) {
                    (*solution) = (*wk1);
                    minError = error;
                }
                maxError = error > maxError ? error: maxError;
            } else {
                wk1 -> SetPrevious();
            }
        }
    }
    std::cout << "Min Error=" << minError << std::endl;
    std::cout << "Max Error=" << maxError << std::endl;
    return solution->GetError();
}