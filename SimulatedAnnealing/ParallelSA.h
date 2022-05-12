#ifndef SA_SCHEDULING_RESOURCES_PARALLELSA_H
#define SA_SCHEDULING_RESOURCES_PARALLELSA_H

#include "SimulatedAnnealing.h"
#include "float.h"
#include "thread"
#include "algorithm"

class ParallelSA: public SimulatedAnnealing {
protected:
    int numThreads;
    std::vector<std::thread> pool;
    std::vector<std::shared_ptr<SimulatedAnnealing>> algorithms;

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
            pool.emplace_back(&SimulatedAnnealing::Start, std::ref(*(algorithms[i])), solutions[i]);
        }
        for (auto &th: pool) {
            th.join();
        }
    }

    virtual std::shared_ptr<Solution> chooseBest(std::shared_ptr<Solution> solution, std::vector<std::shared_ptr<Solution>> solutions) {
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
        return minSolution;
    }
public:
    ParallelSA(
            std::shared_ptr<CoolingSchedule> schedule,
            std::shared_ptr<TemperatureProvider> temperatureProvider,
            std::shared_ptr<AcceptanceDistribution> acceptance,
            int numImprovement,
            int numThreads
    ) : SimulatedAnnealing(
            schedule,
            temperatureProvider,
            acceptance,
            numImprovement, 0
    ) {
        this->numThreads = numThreads;
        for (int i = 0; i < this->numThreads; i++) {
            algorithms.emplace_back(
                std::make_shared<SimulatedAnnealing>(
                    schedule->clone(),
                    temperatureProvider->clone(),
                    acceptance->clone(),
                    numImprovement,
                    0
                )
            );
        }
    }

    double Start(std::shared_ptr<Solution> solution) override {
        std::vector<std::shared_ptr<Solution>> solutions = prepareSolutions(solution);
        run(solution, solutions);
        chooseBest(solution, solutions);
        return solution->GetError();
    }
};

class DecompositionParallelSA: public ParallelSA {
public:
    DecompositionParallelSA(const std::shared_ptr<CoolingSchedule> &schedule,
                            const std::shared_ptr<TemperatureProvider> &temperatureProvider,
                            const std::shared_ptr<AcceptanceDistribution> &acceptance, int numImprovement,
                            int numThreads) : ParallelSA(schedule, temperatureProvider, acceptance,
                                                         numImprovement, numThreads) {}

    std::vector<std::shared_ptr<Solution>> prepareSolutions(std::shared_ptr<Solution> solution) override {
        return solution->breakScope(numThreads);
    }
};

class ParallelSAWithPruning: public DecompositionParallelSA {
private:
    int numPruning;
public:

    ParallelSAWithPruning(const std::shared_ptr<CoolingSchedule> &schedule,
                          const std::shared_ptr<TemperatureProvider> &temperatureProvider,
                          const std::shared_ptr<AcceptanceDistribution> &acceptance,
                          int numImprovement,
                          int numPruning,
                          double pruneThreshold,
                          int numThreads) : DecompositionParallelSA(schedule, temperatureProvider, acceptance,
                                                         numImprovement, numThreads) {
        this -> numPruning = numPruning;
        for (auto& algorithm: algorithms) {
            algorithm->numPruning = numPruning;
            algorithm->iterationsWithoutApproximation = numPruning;
            algorithm->pruneThreshold = pruneThreshold;
        }
    }
protected:

    void run(std::shared_ptr<Solution> solution, std::vector<std::shared_ptr<Solution>>& solutions) override {
        std::shared_ptr<Solution> withoutImprovement = nullptr;
        ParallelSA::run(solution, solutions);
        double bestError;
        while (true) {
            std::vector<std::shared_ptr<Solution>> v(solutions);
            if (withoutImprovement != nullptr) {
                v.push_back(withoutImprovement);
            }
            auto best = chooseBest(solution, v);
            bestError = solution->GetError();

            if (withoutImprovement != best) {
                withoutImprovement = nullptr;
            }

            auto iter = solutions.begin();
            if ((iter = std::find(solutions.begin(), solutions.end(), best)) != solutions.end()) {
                if (algorithms[iter - solutions.begin()]->iterationsWithoutImprovement == numImprovement) {
                    withoutImprovement = best;
                }
            }

            std::vector<std::shared_ptr<Solution>> newSolutions = {};
            std::vector<std::shared_ptr<SimulatedAnnealing>> newAlgorithms = {};
            for (int i = 0; i < solutions.size(); i++) {
                auto anotherError = solutions[i]->GetError();
                if ((anotherError - bestError)/bestError < pruneThreshold) {
                    if (algorithms[i]->iterationsWithoutImprovement != numImprovement) {
                        std::cout << algorithms[i]->iterationsWithoutImprovement << std::endl;
                        newSolutions.push_back(solutions[i]);
                        newAlgorithms.push_back(algorithms[i]);
                    }
                }
            }
            solutions = newSolutions;
            algorithms = newAlgorithms;
            if (solutions.empty()) {
                solutions.push_back(best);
                if (withoutImprovement != nullptr) {
                    solutions.push_back(withoutImprovement);
                }
                break;
            }
            pool.clear();
            for (int i = 0; i < algorithms.size(); i++) {
                pool.emplace_back(&SimulatedAnnealing::Anneal, std::ref(*(algorithms[i])), solutions[i]);
            }
            for (auto &th: pool) {
                th.join();
            }
        }
    }
};

#endif //SA_SCHEDULING_RESOURCES_PARALLELSA_H