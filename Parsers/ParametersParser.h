#ifndef SA_SCHEDULING_RESOURCES_PARAMETERSPARSER_H
#define SA_SCHEDULING_RESOURCES_PARAMETERSPARSER_H

#include "../SimulatedAnnealing/CoolingSchedule/CoolingSchedule.h"
#include "../SimulatedAnnealing/InitialTemperature/TemperatureProvider.h"
#include "../SimulatedAnnealing/AcceptanceDistribution/AcceptanceDistribution.h"
#include "../SimulatedAnnealing/Factories/SpecificFactories.h"
#include "memory"
#include <iostream>
#include <boost/property_tree/json_parser.hpp>
#include "boost/property_tree/ptree.hpp"
#include "sstream"

class SimulatedAnnealingParameters {
public:
    std::shared_ptr<CoolingSchedule> coolingSchedule;
    std::shared_ptr<TemperatureProvider> temperatureProvider;
    std::shared_ptr<AcceptanceDistribution> acceptanceDistribution;

    SimulatedAnnealingParameters(
            std::shared_ptr<CoolingSchedule> coolingSchedule,
            std::shared_ptr<TemperatureProvider> temperatureProvider,
            std::shared_ptr<AcceptanceDistribution> acceptanceDistribution
            ) : coolingSchedule(std::move(coolingSchedule)),
            temperatureProvider(std::move(temperatureProvider)),
            acceptanceDistribution(std::move(acceptanceDistribution)) {}
};

class ParametersParser {
private:
    Factories::CoolingScheduleFactory csFactory = Factories::CoolingScheduleFactory();
    Factories::TemperatureProviderFactory tpFactory = Factories::TemperatureProviderFactory();
    Factories::AcceptanceDistributionFactory adFactory = Factories::AcceptanceDistributionFactory();

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
        Conditions* cond,
        Solution* solution
    ) {
        auto type = ptree.get<std::string>("type");
        if (type == "range") {
            return tpFactory.create<Conditions*>(type, cond);
        } else if (type == "statistical") {
            auto numOfRuns = ptree.get<int>("parameters.numOfRuns");
            return tpFactory.create<Solution*, int>(type, solution, numOfRuns);
        }
        throw std::logic_error("Illegal argument for temperature provider");
    }

    std::shared_ptr<AcceptanceDistribution>
    parseAcceptanceDistribution(
        const boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char>>& ptree
    ) {
        auto type = ptree.get<std::string>("type");
        auto parameters = ptree.get_child("parameters");
        if (type == "one-wing") {
            auto threshold = parameters.get<double>("threshold");
            return adFactory.create<double>(type, threshold);
        } else if (type == "two-wing") {
            auto deteriorationThreshold = parameters.get<double>("deteriorationThreshold");
            auto improvementThreshold = parameters.get<double>("improvementThreshold");
            return adFactory.create<double, double>(type, improvementThreshold, deteriorationThreshold);
        }
        throw std::logic_error("Illegal argument for acceptance distribution");
    }
public:

    SimulatedAnnealingParameters parse(const std::string& fileName, Conditions* cond, Solution* solution) {
        boost::property_tree::ptree pt;
        boost::property_tree::json_parser::read_json(fileName, pt);
        auto coolingScheduleConfig = pt.get_child("SimulatedAnnealingParameters.CoolingSchedule");
        auto temperatureProviderConfig = pt.get_child("SimulatedAnnealingParameters.InitialTemperatureProvider");
        auto acceptanceDistributionConfig = pt.get_child("SimulatedAnnealingParameters.AcceptanceDistribution");
        auto cs = parseCoolingSchedule(coolingScheduleConfig);
        auto tp = parseTemperatureProvider(temperatureProviderConfig, cond, solution);
        auto ad = parseAcceptanceDistribution(acceptanceDistributionConfig);
        return {cs, tp, ad};
    }
};


#endif //SA_SCHEDULING_RESOURCES_PARAMETERSPARSER_H
