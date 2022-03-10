#include "iostream"
#include "SimulatedAnnealing/Factories/CoolingScheduleFactory.h"

int main() {
    auto factory = new Factories::CoolingScheduleFactory();
    auto cs = factory->create<double>("geometric", 0.95);
    cs->setInitialTemperature(100);
    std::cout << cs->getNextTemperature() << std::endl;
}
