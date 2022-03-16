#include "iostream"
#include "Parsers/ParametersParser.h"

int main() {
    auto parameters = ParametersParser().parse("/home/samuil/CLionProjects/SA-Schefuling-With-Resource/Parsers/example.json", nullptr, nullptr);

    auto cs = parameters.coolingSchedule;
    cs->setInitialTemperature(100);
    std::cout << cs->getNextTemperature() << "," << cs->getNextTemperature() << "," << cs->getNextTemperature() << "," << cs->getNextTemperature() << std::endl;
}
