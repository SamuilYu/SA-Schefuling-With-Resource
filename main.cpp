#include "iostream"
#include "thread"
#include "Parsers/ParametersParser.h"
#include "Parsers/DependencyGraphParser.h"
#include "UniprocessorSchedulingWithResource/Schedule.h"

int main(int agrc, char* argv[]) {
    std::ostringstream dag;
    dag << "C:\\Users\\samyu\\CLionProjects\\SA-Schefuling-With-Resource\\data\\trees\\tree_9.txt";
    auto dp = DependencyGraphParser::parse(dag.str());
    auto cond = std::make_shared<SchedulingConditions>(*dp);
    std::shared_ptr<Solution> probe = std::make_shared<Schedule>(*cond);
    std::ostringstream par;
    par << "C:\\Users\\samyu\\CLionProjects\\SA-Schefuling-With-Resource\\data\\parameters\\main\\main_2.json";
    auto sa = ParametersParser().parse(par.str(), cond, probe);
    auto schedule = std::make_shared<Schedule>(*cond);
    auto answer = sa->Start(schedule);
    std::ostringstream res;
    res << "C:\\Users\\samyu\\CLionProjects\\SA-Schefuling-With-Resource\\data\\results\\result.txt";
    std::ofstream result;
    result.open(res.str());
    result << answer << "\n";
    for (auto& each: schedule->getValue()) {
        result << " " << each;
    }
    result.close();
}
