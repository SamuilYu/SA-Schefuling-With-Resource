#ifndef SA_SCHEDULING_RESOURCES_PARALLELSA_H
#define SA_SCHEDULING_RESOURCES_PARALLELSA_H

#include "SimulatedAnnealing.h"
#include "float.h"
#include "thread"

class ParallelSA: public SimulatedAnnealing {
protected:
    int numThreads;
    std::vector<std::thread> pool;
    std::vector<SimulatedAnnealing> algorithms;

    virtual std::vector<std::shared_ptr<Solution>> prepareSolutions(std::shared_ptr<Solution> solution) {
        std::vector<std::shared_ptr<Solution>> solutions;
        for (int i = 0; i < numThreads; i++) {
            auto current = solution->clone();
            solutions.push_back(current);
        }
        return solutions;
    }

    virtual void run(std::shared_ptr<Solution> solution, std::vector<std::shared_ptr<Solution>>& solutions) {
        for (int i = 0; i < numThreads; i++) {
            pool.emplace_back(&SimulatedAnnealing::Start, algorithms[i], solutions[i]);
        }
        for (auto &th: pool) {
            th.join();
        }
    }

    virtual double chooseBest(std::shared_ptr<Solution> solution, std::vector<std::shared_ptr<Solution>> solutions) {
        auto minError = DBL_MAX;
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

    double Start(std::shared_ptr<Solution> solution) override {
        std::vector<std::shared_ptr<Solution>> solutions = prepareSolutions(solution);
        run(solution, solutions);
        return chooseBest(solution, solutions);
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

class ParallelSAWithSharing: public ParallelSA {
private:
    int actualNumTemps;
    int numStages;
public:

    ParallelSAWithSharing(const std::shared_ptr<CoolingSchedule> &schedule,
                            const std::shared_ptr<TemperatureProvider> &temperatureProvider,
                            const std::shared_ptr<AcceptanceDistribution> &acceptance, int numTemps, int numIterations,
                            int numThreads, int numStages) : ParallelSA(schedule, temperatureProvider, acceptance, numTemps/numStages,
                                                         numIterations, numThreads) {
        actualNumTemps =  numTemps;
        this -> numStages = numStages;
    }
protected:

    void run(std::shared_ptr<Solution> solution, std::vector<std::shared_ptr<Solution>>& solutions) override {
        ParallelSA::run(solution, solutions);
        for (int j = 0; j < numStages - 2; j++) {
            pool.clear();
            for (int i = 0; i < numThreads; i++) {
                pool.emplace_back(&SimulatedAnnealing::Anneal, algorithms[i], solutions[i]);
            }
            for (auto &th: pool) {
                th.join();
            }
            chooseBest(solution, solutions);
            for (auto &each: solutions) {
                each = solution->clone();
            }
        }
        pool.clear();
        for (int i = 0; i < numThreads; i++) {
            pool.emplace_back(&SimulatedAnnealing::Anneal,
                              SimulatedAnnealing(
                                      coolingSchedule->clone(),
                                      temperatureProvider->clone(),
                                      acceptanceDist->clone(),
                                      numTemps % numStages,
                                      numIterations),
                              solutions[i]);
        }
        for (auto &th: pool) {
            th.join();
        }
        chooseBest(solution, solutions);
        for (auto &each: solutions) {
            each = solution->clone();
        }
    }
};

#endif //SA_SCHEDULING_RESOURCES_PARALLELSA_H