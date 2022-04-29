#ifndef SA_SCHEFULING_WITH_RESOURCE_SIMULATEDANNEALING_H
#define SA_SCHEFULING_WITH_RESOURCE_SIMULATEDANNEALING_H

#include "Problem/Solution.h"
#include "CoolingSchedule//CoolingSchedule.h"
#include "InitialTemperature/TemperatureProvider.h"
#include "AcceptanceDistribution/AcceptanceDistribution.h"

class SimulatedAnnealing {
public:
    SimulatedAnnealing(CoolingSchedule *schedule,
                       TemperatureProvider *temperatureProvider,
                       AcceptanceDistribution *acceptance, int, int);

    ~SimulatedAnnealing() = default;
    int numTemps;
    int numIterations;
    double initialTemp;
    double Start(Solution* solution, Solution* wk1, Solution* wk2, int cycles);
private:
    CoolingSchedule* coolingSchedule;
    AcceptanceDistribution* acceptanceDist;
    double Anneal(Solution* solution, Solution* wk1, Solution* wk2);
    double finalTemp;
};


#endif //SA_SCHEFULING_WITH_RESOURCE_SIMULATEDANNEALING_H
