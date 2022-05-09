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
    template<> template<> Factory<TemperatureProvider>::Registered<std::shared_ptr<Conditions>> Factory<TemperatureProvider>::registered<std::shared_ptr<Conditions>> = {};
    template<> template<> Factory<TemperatureProvider>::Registered<std::shared_ptr<Solution>, int> Factory<TemperatureProvider>::registered<std::shared_ptr<Solution>, int> = {};


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
            registered<std::shared_ptr<Conditions>>["range"] = std::make_shared<Creator<RangeBasedTemperatureProvider, std::shared_ptr<Conditions>>>();
//            registered<std::shared_ptr<Solution>, int>["statistical"] = std::make_shared<Creator<StatisticalTemperatureProvider, std::shared_ptr<Solution>, int>>();
        }
    };
//
//    class AcceptanceDistributionFactory: public Factory<AcceptanceDistribution> {
//    public:
//        AcceptanceDistributionFactory() {
//            registered<double>["one-wing"] = std::make_shared<Creator<OneWingDistribution, double>>();
//            registered<double, double>["two-wing"] = std::make_shared<Creator<TwoWingDistribution, double, double>>();
//        }
//    };

}

#endif //SA_SCHEDULING_RESOURCES_SPECIFICFACTORIES_H
