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
            const std::shared_ptr<TemperatureProvider>& temperatureProvider,
            std:: shared_ptr<AcceptanceDistribution> acceptance,
            int numTemps,
            int numIterations
    );

    ~SimulatedAnnealing() = default;
    int numTemps;
    int numIterations;
    double initialTemp;
    virtual double Start(std::shared_ptr<Solution> solution, int cycles);

    double Anneal(std::shared_ptr<Solution> solution);

protected:
    std::shared_ptr<CoolingSchedule> coolingSchedule;
    std:: shared_ptr<AcceptanceDistribution> acceptanceDist;
    double finalTemp;
};


#endif //SA_SCHEFULING_WITH_RESOURCE_SIMULATEDANNEALING_H
