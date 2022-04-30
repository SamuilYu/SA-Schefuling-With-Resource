#include "iostream"
#include "thread"
#include "Parsers/ParametersParser.h"
#include "Parsers/DependencyGraphParser.h"
#include "UniprocessorSchedulingWithResource/ScopedSchedule.h"
#include "SimulatedAnnealing/SimulatedAnnealing.h"
#include "SimulatedAnnealing/ParallelSA.h"

int main() {
    std::cout << std::thread::hardware_concurrency() << std::endl;
    auto dp = DependencyGraphParser::parse("/home/samuil/CLionProjects/SA-Schefuling-With-Resource/data/graphs/dag156.txt");

    auto cond = std::make_shared<SchedulingConditions>(*dp);
    std::shared_ptr<Solution> probe = std::make_shared<ScopedSchedule>(*cond);
    auto sa = ParametersParser().parse("/home/samuil/CLionProjects/SA-Schefuling-With-Resource/Parsers/example.json", cond, probe);

    auto schedule = std::make_shared<ScopedSchedule>(*cond);
    auto wk1 = std::make_shared<ScopedSchedule>(*cond);
    auto answer = sa->Start(schedule, wk1, 1);
    for (auto each: schedule->getValue()) {
        std::cout << each << " ";
    }
    std::cout << std::endl << answer;
}
