#include <cmath>
#include "SimulatedAnnealing.h"
#include "CoolingSchedule/CoolingSchedule.h"

SimulatedAnnealing::SimulatedAnnealing(CoolingSchedule* schedule) {
    numTemps = 100;
    numIterations = 100;
    initialTemp = 100.0;
    coolingSchedule = schedule;
    coolingSchedule -> setInitialTemperature(initialTemp);
    finalTemp = 0.0001;
    isCoolingScheduleLinear = false;
    k = 10;
    cycles = 4;
}

double SimulatedAnnealing::Start(Solution &solution, Solution &wk1, Solution &wk2, double goal) {
    for (int i = 0; i < cycles; i++) {
        if (Anneal(solution, wk1, wk2, goal) <= goal) break;
    }
    return solution.GetError();
}

double SimulatedAnnealing::EstimateK(Solution &solution, int N) {
    double E = 1.0;
    double sum = 0.0;
    double sums = 0.0;
    for (int i = 0; i < N; i++) {
        E = solution.Perturb(initialTemp, initialTemp);
        sum += E;
        sums += (E * E);
    }
    double variance = sums / (N - 1) - (sum * sum) / (N * (N - 1));
    return -log(0.8) * initialTemp / sqrt(variance);
}

double SimulatedAnnealing::Anneal(Solution &solution, Solution &wk1, Solution &wk2, double goal) {
    double error = solution.Initialize();
    if (error <= goal) return error;
    k = EstimateK(solution, 1000);
    wk1 = solution;
    wk2 = solution;
    finalTemp = goal;
    bool hasImproved = false;
    double temperature, deltaError;
    int i;
    for (int n = 0; n < numTemps; n++) {
        temperature = coolingSchedule->getNextTemperature();
        hasImproved = false;

        for (i = 0; i < numIterations; i++) {
            deltaError = wk1.Perturb(temperature, initialTemp) - error;
            if (IsAcceptedByMetropolis(temperature, deltaError)) {
                wk2 = wk1;
                hasImproved = true;
                if (wk1.GetError() <= goal) break;
            }
        }
        if (hasImproved == true) // If saw improvement at this temperature
        {
            wk1 = wk2;
            solution = wk2;
            error = solution.GetError();
            if (error <= goal) break;
        }
    }
    return solution.GetError();
}

bool SimulatedAnnealing::IsAcceptedByMetropolis(double temperature, double deltaError) {
    if (deltaError <= 0) return true;
    return Random(0.0, 1.0) < exp(-k * deltaError / temperature);
}

double SimulatedAnnealing::GetTemperature() {
    return coolingSchedule -> getNextTemperature();
}