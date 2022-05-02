#include <iostream>
#include <utility>
#include "SimulatedAnnealing.h"
#include "CoolingSchedule/CoolingSchedule.h"
#include "InitialTemperature/TemperatureProvider.h"
#include "AcceptanceDistribution/AcceptanceDistribution.h"

SimulatedAnnealing::SimulatedAnnealing(
        std::shared_ptr<CoolingSchedule> schedule,
        std::shared_ptr<TemperatureProvider> temperatureProvider,
        std:: shared_ptr<AcceptanceDistribution> acceptance,
        int numTemps,
        int numIterations
    ) {
    this -> numTemps = numTemps;
    this -> numIterations = numIterations;
    this -> temperatureProvider = std::move(temperatureProvider);
    coolingSchedule = std::move(schedule);
    acceptanceDist = std::move(acceptance);
    finalTemp = 0.0001;
}

double SimulatedAnnealing::Start(std::shared_ptr<Solution> solution) {
    initialTemp = temperatureProvider->getTemperature();
    coolingSchedule -> setInitialTemperature(initialTemp);
    coolingSchedule -> restart();
    solution->Initialize();
    Anneal(solution);
    return solution->GetError();
}

double SimulatedAnnealing::Anneal(std::shared_ptr<Solution> solution) {
    double error = solution->GetError();
    double newError;
    double maxError = error;
    double minError = error;

    auto wk1 = solution->clone();
    double temperature, deltaError;
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
//    std::cout << "Min Error=" << minError << std::endl;
//    std::cout << "Max Error=" << maxError << std::endl;
    return solution->GetError();
}