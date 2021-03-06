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
        std::vector<std::shared_ptr<Solution>> solutions = {};
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
            if (th.joinable()) {
                th.join();
            }
        }
        pool.clear();
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
        auto solutions =  solution->breakScope(numThreads);
        if (solutions.size()<numThreads) {
            numThreads = solutions.size();
        }
        algorithms.resize(numThreads);
        return solutions;
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
        this-> pruneThreshold = pruneThreshold;
        for (auto& algorithm: algorithms) {
            algorithm->numPruning = numPruning;
            algorithm->numApproximation = numPruning;
            algorithm->pruneThreshold = pruneThreshold;
        }
    }
protected:

    void run(std::shared_ptr<Solution> solution, std::vector<std::shared_ptr<Solution>>& solutions) override {
        for (int i = 0; i < algorithms.size(); i++) {
            algorithms[i]->numScope = i;
        }
        std::shared_ptr<Solution> withoutImprovement = nullptr;
        ParallelSA::run(solution, solutions);
        auto currentAlg = algorithms;
        double bestError = 0.0;
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

            auto iter = std::find(solutions.begin(), solutions.end(), best);
            if (iter != solutions.end()) {
                if (currentAlg[iter - solutions.begin()]->iterationsWithoutImprovement >= numImprovement) {
                    withoutImprovement = best;
                }
            }

            std::vector<std::shared_ptr<Solution>> newSolutions = {};
            std::vector<std::shared_ptr<SimulatedAnnealing>> newAlgorithms = {};
            for (int i = 0; i < solutions.size(); i++) {
                auto anotherError = solutions[i]->GetError();
                if ((anotherError - bestError)/bestError < pruneThreshold) {
                    if (currentAlg[i]->iterationsWithoutImprovement < numImprovement) {
                        currentAlg[i]->globalMinError = bestError;
//                        std::cout << algorithms[i]->iterationsWithoutImprovement << std::endl;
                        newSolutions.push_back(solutions[i]);
                        newAlgorithms.push_back(currentAlg[i]);
                    } else {
//                        std::cout << "Scope " << currentAlg[i]->numScope << " died without improvement. F=" << solutions[i]-> GetError() << " Best_F=" << bestError << std::endl;
                    }
                } else {
//                    std::cout << "Scope " << currentAlg[i]->numScope << " died by pruning. F=" << solutions[i] -> GetError()  << " Best_F=" << bestError << std::endl;
                }
            }

            if (algorithms.size() == 1) {
                std::cout << "Last scope " << algorithms[0] -> numScope << ". F=" << solutions[0] << std::endl;
            }
            solutions = newSolutions;
            currentAlg = newAlgorithms;
            if (solutions.empty()) {
                solutions.push_back(best);
                if (withoutImprovement != nullptr) {
                    solutions.push_back(withoutImprovement);
                }
                break;
            }
            pool.clear();
            for (int i = 0; i < currentAlg.size(); i++) {
                pool.emplace_back(&SimulatedAnnealing::Anneal, std::ref(*(currentAlg[i])), solutions[i]);
            }
            for (auto &th: pool) {
                if (th.joinable()) {
                    th.join();
                }
            }
            pool.clear();
        }
    }
};

#endif //SA_SCHEDULING_RESOURCES_PARALLELSA_H