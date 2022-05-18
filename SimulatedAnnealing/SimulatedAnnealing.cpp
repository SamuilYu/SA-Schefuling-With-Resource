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
        int numImprovement, int numPruning
    ) {
    this -> numImprovement = numImprovement;
    this -> numIterations = 1000;
    this -> temperatureProvider = std::move(temperatureProvider);
    this -> numPruning = numPruning;
    coolingSchedule = std::move(schedule);
    acceptanceDist = std::move(acceptance);
    globalMinError = 0.0;
}

double SimulatedAnnealing::Start(std::shared_ptr<Solution> solution) {
    initialTemp = temperatureProvider->getTemperature();
    coolingSchedule -> setInitialTemperature(initialTemp);
    coolingSchedule -> restart();
    iterationsWithoutImprovement = 0;
    iterationsWithoutApproximation = 0;
    solution->Initialize();
    for (int j = 0; j < 4000; j++) {
        solution ->Perturb(initialTemp, initialTemp);
    }
    double error = solution->GetError();
    minError = error;
    wk1 = solution->clone();
    Anneal(solution);
    return solution->GetError();
}

double SimulatedAnnealing::Anneal(std::shared_ptr<Solution> solution) {
    double error = solution->GetError();
    double newError;
    int totalIterations = 0;
    double temperature, deltaError;
    while (true) {
        temperature = coolingSchedule->getNextTemperature();
        for (int i = 0; i < numIterations; i++) {
            newError = wk1->Perturb(temperature, initialTemp);
            deltaError = newError - error;
            if ((newError - minError)/minError > -0.01) {
                iterationsWithoutImprovement++;
            } else {
                iterationsWithoutImprovement = 0;
            }
            if (acceptanceDist -> isAccepted(temperature, deltaError)) {
                error = newError;
                if (error < minError) {
                    (*solution) = (*wk1);
                    minError = error;
                    if (globalMinError != 0.0 && (minError - globalMinError)/globalMinError < pruneThreshold) {
                        iterationsWithoutApproximation = 0;
                    } else if (globalMinError != 0.0) {
                        iterationsWithoutApproximation++;
                    }
                }
            } else {
                wk1 -> SetPrevious();
            }
            totalIterations++;
            if (iterationsWithoutImprovement >= numImprovement || totalIterations == numPruning) {
                break;
            }
            if (iterationsWithoutApproximation >= numApproximation && globalMinError != 0.0) {
                break;
            }
        }
        if (iterationsWithoutImprovement >= numImprovement || totalIterations == numPruning) {
            break;
        }
        if (iterationsWithoutApproximation >= numApproximation && globalMinError != 0.0) {
//            std::cout << "Scope " << this->numScope << " died by pruning inside. F=" << solution -> GetError() << std::endl;
            break;
        }
    }
    return solution->GetError();
}