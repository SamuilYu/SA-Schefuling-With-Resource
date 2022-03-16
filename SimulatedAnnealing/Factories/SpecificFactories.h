#ifndef SA_SCHEDULING_RESOURCES_SPECIFICFACTORIES_H
#define SA_SCHEDULING_RESOURCES_SPECIFICFACTORIES_H

#include "Factory.h"
#include "map"
#include "../CoolingSchedule/CoolingSchedule.h"
#include "../CoolingSchedule/SpecificSchedules.h"
#include "../InitialTemperature/TemperatureProvider.h"
#include "../AcceptanceDistribution/AcceptanceDistribution.h"

namespace Factories {
    template<> template<> Factory<CoolingSchedule>::Registered<> Factory<CoolingSchedule>::registered<> = {};
    template<> template<> Factory<CoolingSchedule>::Registered<double> Factory<CoolingSchedule>::registered<double> = {};
    template<> template<> Factory<AcceptanceDistribution>::Registered<double, double> Factory<AcceptanceDistribution>::registered<double, double> = {};
    template<> template<> Factory<AcceptanceDistribution>::Registered<double> Factory<AcceptanceDistribution>::registered<double> = {};
    template<> template<> Factory<TemperatureProvider>::Registered<Conditions*> Factory<TemperatureProvider>::registered<Conditions*> = {};
    template<> template<> Factory<TemperatureProvider>::Registered<Solution*, int> Factory<TemperatureProvider>::registered<Solution*, int> = {};


    class CoolingScheduleFactory: public Factory<CoolingSchedule> {
    public:
        CoolingScheduleFactory() {
            registered<double>["geometric"] = std::make_shared<Creator<GeometricCoolingSchedule, double>>();
            registered<>["boltzmann"] = std::make_shared<Creator<BoltzmannCoolingSchedule>>();
            registered<>["cauchy"] = std::make_shared<Creator<CauchyCoolingSchedule>>();
            registered<>["hybrid"] = std::make_shared<Creator<HybridCoolingSchedule>>();
        }
    };

    class TemperatureProviderFactory: public Factory<TemperatureProvider> {
    public:
        TemperatureProviderFactory() {
            registered<Conditions*>["range"] = std::make_shared<Creator<RangeBasedTemperatureProvider, Conditions*>>();
            registered<Solution*, int>["boltzmann"] = std::make_shared<Creator<StatisticalTemperatureProvider, Solution*, int>>();
        }
    };

    class AcceptanceDistributionFactory: public Factory<AcceptanceDistribution> {
    public:
        AcceptanceDistributionFactory() {
            registered<double>["one-wing"] = std::make_shared<Creator<RangeBasedTemperatureProvider, double>>();
            registered<double, double>["two-wing"] = std::make_shared<Creator<StatisticalTemperatureProvider, double, double>>();
        }
    };

}

#endif //SA_SCHEDULING_RESOURCES_SPECIFICFACTORIES_H
