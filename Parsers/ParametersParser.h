#ifndef SA_SCHEDULING_RESOURCES_PARAMETERSPARSER_H
#define SA_SCHEDULING_RESOURCES_PARAMETERSPARSER_H

#include "../SimulatedAnnealing/CoolingSchedule/CoolingSchedule.h"
#include "../SimulatedAnnealing/InitialTemperature/TemperatureProvider.h"
#include "../SimulatedAnnealing/AcceptanceDistribution/AcceptanceDistribution.h"
#include "../SimulatedAnnealing/Factories/SpecificFactories.h"
#include "../SimulatedAnnealing/SimulatedAnnealing.h"
#include "../SimulatedAnnealing/ParallelSA.h"
#include "memory"
#include <iostream>
#include <boost/property_tree/json_parser.hpp>
#include "boost/property_tree/ptree.hpp"
#include "sstream"

//class SimulatedAnnealingParameters {
//public:
//    std::shared_ptr<CoolingSchedule> coolingSchedule;
//    std::shared_ptr<TemperatureProvider> temperatureProvider;
//    std::shared_ptr<AcceptanceDistribution> acceptanceDistribution;
//
//    SimulatedAnnealingParameters(
//            std::shared_ptr<CoolingSchedule> coolingSchedule,
//            std::shared_ptr<TemperatureProvider> temperatureProvider,
//            std::shared_ptr<AcceptanceDistribution> acceptanceDistribution
//            ) : coolingSchedule(std::move(coolingSchedule)),
//            temperatureProvider(std::move(temperatureProvider)),
//            acceptanceDistribution(std::move(acceptanceDistribution)) {}
//};

class ParametersParser {
private:
    Factories::CoolingScheduleFactory csFactory = Factories::CoolingScheduleFactory();
    Factories::TemperatureProviderFactory tpFactory = Factories::TemperatureProviderFactory();
//    Factories::AcceptanceDistributionFactory adFactory = Factories::AcceptanceDistributionFactory();

    std::shared_ptr<CoolingSchedule>
    parseCoolingSchedule(
        boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char>> ptree
    ) {
        auto type = ptree.get<std::string>("type");
        try {
            auto parameters = ptree.get_child("parameters");
            return csFactory.create<double>(type, parameters.get<double>("factor"));
        } catch (boost::exception& e) {
            return csFactory.create(type);
        }
    }

    std::shared_ptr<TemperatureProvider>
    parseTemperatureProvider(
        const boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char>>& ptree,
        const std::shared_ptr<Conditions>& cond,
        const std::shared_ptr<Solution>& solution,
        const std::shared_ptr<AcceptanceDistribution>& dist
    ) {
        auto type = ptree.get<std::string>("type");
        if (type == "range") {
            return tpFactory.create<std::shared_ptr<Conditions>>(type, cond->clone());
        } else if (type == "statistical") {
            auto numOfRuns = ptree.get<int>("parameters.numOfRuns");
            auto threshold = ptree.get<double>("parameters.threshold");
            return std::make_shared<StatisticalTemperatureProvider>(solution->clone(), dist -> clone(), threshold, numOfRuns);
        }
        throw std::logic_error("Illegal argument for temperature provider");
    }

    std::shared_ptr<AcceptanceDistribution>
    parseAcceptanceDistribution(
        const boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char>>& ptree
    ) {
        auto type = ptree.get<std::string>("type");
        if (type == "metropolis") {
            return std::make_shared<MetropolisDistribution>();
        } else if (type == "hastings") {
            auto parameters = ptree.get_child("parameters");
            auto gamma = parameters.get<double>("gamma");
            return std::make_shared<HastingsDistribution>(gamma);
        } else if (type == "barker") {
            return std::make_shared<BarkerDistribution>();
        }
        throw std::logic_error("Illegal argument for acceptance distribution");
    }

    static std::shared_ptr<SimulatedAnnealing>
    parseSimulatedAnnealing(
        const boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char>>& ptree,
        const std::shared_ptr<CoolingSchedule>& cs,
        const std::shared_ptr<TemperatureProvider>& tp,
        const std::shared_ptr<AcceptanceDistribution>& ad
    ) {
        auto type = ptree.get<std::string>("type");
        auto parameters = ptree.get_child("parameters");

        auto numImprovement = parameters.get<int>("numImprovement");
        if (type == "no") {
            return std::make_shared<SimulatedAnnealing>(cs,tp,ad,numImprovement, 0);
        } else if (type == "multi") {
            auto numThreads = parameters.get<int>("numThreads");
            return std::make_shared<ParallelSA>(cs,tp,ad,numImprovement,numThreads);
        } else if (type == "decomposition") {
            auto numThreads = parameters.get<int>("numThreads");
            return std::make_shared<DecompositionParallelSA>(cs,tp,ad,numImprovement,numThreads);
        } else if (type == "prune") {
            auto numThreads = parameters.get<int>("numThreads");
            auto numPruning = parameters.get<int>("numPruning");
            auto pruneThreshold = parameters.get<double>("threshold");
            return std::make_shared<ParallelSAWithPruning>(cs, tp, ad, numImprovement, numPruning, pruneThreshold, numThreads);
        }
        throw std::logic_error("Illegal argument for simulated annealing algorithm.");
    }
public:

    std::shared_ptr<SimulatedAnnealing> parse(const std::string& fileName,std::shared_ptr<Conditions> cond, std::shared_ptr<Solution> solution) {
        boost::property_tree::ptree pt;
        boost::property_tree::json_parser::read_json(fileName, pt);
        auto coolingScheduleConfig = pt.get_child("SimulatedAnnealingParameters.CoolingSchedule");
        auto temperatureProviderConfig = pt.get_child("SimulatedAnnealingParameters.InitialTemperatureProvider");
        auto acceptanceDistributionConfig = pt.get_child("SimulatedAnnealingParameters.AcceptanceDistribution");
        auto simulatedAnnealingConfig = pt.get_child("SimulatedAnnealingParameters.Parallel");
        auto cs = parseCoolingSchedule(coolingScheduleConfig);
        auto ad = parseAcceptanceDistribution(acceptanceDistributionConfig);
        auto tp = parseTemperatureProvider(temperatureProviderConfig, cond, solution, ad);
        auto sa = parseSimulatedAnnealing(simulatedAnnealingConfig, cs, tp, ad);
        return sa;
    }
};


#endif //SA_SCHEDULING_RESOURCES_PARAMETERSPARSER_H