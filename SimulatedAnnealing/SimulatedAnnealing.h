#ifndef SA_SCHEFULING_WITH_RESOURCE_SIMULATEDANNEALING_H
#define SA_SCHEFULING_WITH_RESOURCE_SIMULATEDANNEALING_H

#include "Problem/Solution.h"
#include "CoolingSchedule//CoolingSchedule.h"
#include "InitialTemperature/TemperatureProvider.h"
#include "AcceptanceDistribution/AcceptanceDistribution.h"

class SimulatedAnnealing {
public:
    SimulatedAnnealing(
            std::shared_ptr<CoolingSchedule> schedule,
            std::shared_ptr<TemperatureProvider> temperatureProvider,
            std:: shared_ptr<AcceptanceDistribution> acceptance,
            int, int
    );

    ~SimulatedAnnealing() = default;
    virtual double Start(std::shared_ptr<Solution> solution);
    double Anneal(std::shared_ptr<Solution> solution);

    int numPruning;
    int iterationsWithoutImprovement;
    int iterationsWithoutApproximation = 0;
    double pruneThreshold;
    double globalMinError = 0.0;
    int numScope;
    int numApproximation = 0;
protected:
    double minError;
    std::shared_ptr<Solution> wk1;
    double initialTemp;
    int numImprovement;
    int numIterations;
    std::shared_ptr<CoolingSchedule> coolingSchedule;
    std::shared_ptr<TemperatureProvider> temperatureProvider;
    std:: shared_ptr<AcceptanceDistribution> acceptanceDist;
};


#endif //SA_SCHEFULING_WITH_RESOURCE_SIMULATEDANNEALING_H
