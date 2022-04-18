#include "SimulatedAnnealing.h"
#include "CoolingSchedule/CoolingSchedule.h"
#include "InitialTemperature/TemperatureProvider.h"
#include "AcceptanceDistribution/AcceptanceDistribution.h"

SimulatedAnnealing::SimulatedAnnealing(
        CoolingSchedule* schedule,
        TemperatureProvider* temperatureProvider,
        AcceptanceDistribution* acceptance
    ) {
    numTemps = 100;
    numIterations = 100;
    initialTemp = temperatureProvider->getTemperature();
    coolingSchedule = schedule;
    coolingSchedule -> setInitialTemperature(initialTemp);
    acceptanceDist = acceptance;
    finalTemp = 0.0001;
}

double SimulatedAnnealing::Start(Solution* solution, Solution* wk1, Solution* wk2, int cycles) {
    for (int i = 0; i < cycles; i++) {
        Anneal(solution, wk1, wk2);
    }
    return solution->GetError();
}

double SimulatedAnnealing::Anneal(Solution* solution, Solution* wk1, Solution* wk2) {
    double error = solution->Initialize();
    wk1 = solution;
    wk2 = solution;
    bool hasImproved = false;
    double temperature, deltaError;
    int i;
    for (int n = 0; n < numTemps; n++) {
        temperature = coolingSchedule->getNextTemperature();
        hasImproved = false;

        for (i = 0; i < numIterations; i++) {
            deltaError = wk1->Perturb(temperature, initialTemp) - error;
            if (acceptanceDist -> isAccepted(temperature, deltaError)) {
                wk2 = wk1;
                hasImproved = true;
            }
        }
        if (hasImproved) // If saw improvement at this temperature
        {
            wk1 = wk2;
            solution = wk2;
            error = solution->GetError();
        }
    }
    return solution->GetError();
}