#ifndef SA_SCHEDULING_RESOURCES_PARALLELSA_H
#define SA_SCHEDULING_RESOURCES_PARALLELSA_H

#include "SimulatedAnnealing.h"
#include "thread"

class ParallelSA: public SimulatedAnnealing {
protected:
    int numThreads;
private:
    std::vector<std::thread> pool;
    std::vector<SimulatedAnnealing> algorithms;

    virtual std::vector<std::shared_ptr<Solution>> prepareSolutions(std::shared_ptr<Solution> solution) {
        std::vector<std::shared_ptr<Solution>> solutions;
        for (int i = 0; i < numThreads && i < std::thread::hardware_concurrency(); i++) {
            auto current = solution->clone();
            solutions.push_back(current);
        }
        return solutions;
    }

    virtual double run(std::shared_ptr<Solution> solution, std::vector<std::shared_ptr<Solution>> solutions) {
        for (int i = 0; i < numThreads; i++) {
            pool.emplace_back(&SimulatedAnnealing::Anneal, algorithms[i], solutions[i]);
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
        this->numThreads = std::min(numThreads, int(std::thread::hardware_concurrency()));
        for (int i = 0; i < this->numThreads; i++) {
            algorithms.emplace_back(
                    schedule->clone(),
                    temperatureProvider->clone(),
                    acceptance->clone(),
                    numTemps,
                    numIterations

            );
        }
    }

    double Start(std::shared_ptr<Solution> solution, int cycles) override {
        std::vector<std::shared_ptr<Solution>> solutions = prepareSolutions(solution);
        return run(solution, solutions);
    }
};

class DecompositionParallelSA: public ParallelSA {
public:
    DecompositionParallelSA(const std::shared_ptr<CoolingSchedule> &schedule,
                            const std::shared_ptr<TemperatureProvider> &temperatureProvider,
                            const std::shared_ptr<AcceptanceDistribution> &acceptance, int numTemps, int numIterations,
                            int numThreads) : ParallelSA(schedule, temperatureProvider, acceptance, numTemps,
                                                         numIterations, numThreads) {}

    std::vector<std::shared_ptr<Solution>> prepareSolutions(std::shared_ptr<Solution> solution) override {
        return solution->breakScope(numThreads);
    }
};

#endif //SA_SCHEDULING_RESOURCES_PARALLELSA_H