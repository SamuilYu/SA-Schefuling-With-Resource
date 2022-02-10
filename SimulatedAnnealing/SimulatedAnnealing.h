#ifndef SA_SCHEFULING_WITH_RESOURCE_SIMULATEDANNEALING_H
#define SA_SCHEFULING_WITH_RESOURCE_SIMULATEDANNEALING_H

#include "./Solution.h"

class SimulatedAnnealing {
public:
    SimulatedAnnealing();
    ~SimulatedAnnealing() = default;
    int numTemps;
    int numIterations;
    double initialTemp;
    bool isCoolingScheduleLinear;
    int cycles;
    double Start(Solution& solution, Solution& wk1, Solution& wk2, double goal);
private:
    double GetTemperature(int index);
    bool IsAcceptedByMetropolis(double temperature, double deltaError);
    double Anneal(Solution& solution, Solution& wk1, Solution& wk2, double goal);
    double EstimateK(Solution& solution, int N);
    double finalTemp;
    double k;
};


#endif //SA_SCHEFULING_WITH_RESOURCE_SIMULATEDANNEALING_H
