#include "iostream"
#include "thread"
#include "Parsers/ParametersParser.h"
#include "Parsers/DependencyGraphParser.h"
#include "UniprocessorSchedulingWithResource/Schedule.h"
#include "filesystem"

int main() {
    std::ostringstream dag;
    dag << std::filesystem::current_path() << "\\data\\graphs\\dag0.txt";
    auto dp = DependencyGraphParser::parse(dag.str());
    auto cond = std::make_shared<SchedulingConditions>(*dp);
    std::shared_ptr<Solution> probe = std::make_shared<Schedule>(*cond);
    std::ostringstream par;
    par << std::filesystem::current_path() << "\\parameters\\main\\main_1.json";
    auto sa = ParametersParser().parse(par.str(), cond, probe);
    auto schedule = std::make_shared<Schedule>(*cond);
    auto answer = sa->Start(schedule);
    std::ostringstream res;
    res << std::filesystem::current_path() << "\\data\\results\\main\\result.txt";
    std::ofstream result;
    result.open(res.str());
    result << answer << "\n";
    for (auto& each: schedule->getValue()) {
        result << " " << each;
    }
    result.close();
}
