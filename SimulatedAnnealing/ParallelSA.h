#ifndef SA_SCHEDULING_RESOURCES_PARALLELSA_H
#define SA_SCHEDULING_RESOURCES_PARALLELSA_H

#include "SimulatedAnnealing.h"
#include "thread"

class ParallelSA: public SimulatedAnnealing {
private:
    std::vector<std::thread> pool;
    std::vector<SimulatedAnnealing> algorithms;
    int numThreads;
public:
    ParallelSA(
            std::shared_ptr<CoolingSchedule> schedule,
            std::shared_ptr<TemperatureProvider> temperatureProvider,
            std::shared_ptr<AcceptanceDistribution> acceptance,
            int numTemps,
            int numIterations,
            int numThreads
    ) : SimulatedAnnealing(
            schedule,
            temperatureProvider,
            acceptance,
            numTemps,
            numIterations
    ) {
        this->numThreads = numThreads;
        for (int i = 0; i < this->numThreads && i < std::thread::hardware_concurrency(); i++) {
            algorithms.emplace_back(
                    schedule->clone(),
                    temperatureProvider->clone(),
                    acceptance->clone(),
                    numTemps,
                    numIterations

            );
        }

    }

    double Start(std::shared_ptr<Solution> solution, std::shared_ptr<Solution> wk1, int cycles) override {
        std::vector<std::shared_ptr<Solution>> solutions;
        for (int i = 0; i < numThreads && i < std::thread::hardware_concurrency(); i++) {
            auto current = solution->clone();
            solutions.push_back(current);
            pool.emplace_back(&SimulatedAnnealing::Anneal, algorithms[i], current, wk1->clone());
        }
        for (auto &th: pool) {
            th.join();
        }

        double minError = MAXFLOAT;
        std::shared_ptr<Solution> minSolution;
        for (const auto& s: solutions) {
            auto newError = s->GetError();
            if (minError > newError) {
                minError = newError;
                minSolution = s;
            }
        }
        (*solution) = (*minSolution);
        return minError;
    }
};

#endif //SA_SCHEDULING_RESOURCES_PARALLELSA_H