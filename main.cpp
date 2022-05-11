#include "iostream"
#include "thread"
#include "Parsers/ParametersParser.h"
#include "Parsers/DependencyGraphParser.h"
#include "UniprocessorSchedulingWithResource/Schedule.h"

int main() {
    std::cout << std::thread::hardware_concurrency() << std::endl;
    auto dp = DependencyGraphParser::parse("C:\\Users\\samyu\\CLionProjects\\SA-Schefuling-With-Resource/data/graphs/dag0.txt");

    auto cond = std::make_shared<SchedulingConditions>(*dp);
    std::shared_ptr<Solution> probe = std::make_shared<Schedule>(*cond);
    auto sa = ParametersParser().parse("C:\\Users\\samyu\\CLionProjects\\SA-Schefuling-With-Resource/data/parameters/parameters_0.json", cond, probe);

    auto schedule = std::make_shared<Schedule>(*cond);
    auto begin = std::chrono::steady_clock::now();
    auto answer = sa->Start(schedule);
    auto end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
    for (auto each: schedule->getValue()) {
        std::cout << each << " ";
    }
    std::cout << std::endl << answer;
}
