#include "iostream"
#include "thread"
#include "Parsers/ParametersParser.h"
#include "Parsers/DependencyGraphParser.h"
#include "UniprocessorSchedulingWithResource/Schedule.h"

void collectMetrics(int i, int j) {
    std::ostringstream dag;
    dag << "C:\\Users\\samyu\\CLionProjects\\SA-Schefuling-With-Resource/data/graphs/dag" << j <<".txt";
    auto dp = DependencyGraphParser::parse(dag.str());
    auto cond = std::make_shared<SchedulingConditions>(*dp);
    std::shared_ptr<Solution> probe = std::make_shared<Schedule>(*cond);
    std::ostringstream par;
    par << "C:\\Users\\samyu\\CLionProjects\\SA-Schefuling-With-Resource/data/parameters/iterations/iterations_parameters_"<< i << ".json";
    auto sa = ParametersParser().parse(par.str(), cond, probe);
    auto schedule = std::make_shared<Schedule>(*cond);
    auto maxMilli = 0L;
    auto maxAnswer = 0.0;
    auto minMilli = LONG_MAX;
    auto minAnswer = std::numeric_limits<double>::max();
    for (int k=0; k < 10; k++) {
        auto begin = std::chrono::steady_clock::now();
        auto answer = sa->Start(schedule);
        auto end = std::chrono::steady_clock::now();
        auto milli = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        maxAnswer = answer > maxAnswer? answer: maxAnswer;
        minAnswer = answer < minAnswer? answer: minAnswer;
        maxMilli = milli > maxMilli? milli: maxMilli;
        minMilli = milli < minMilli? milli: minMilli;
    }
    std::ostringstream res;
    res << "C:\\Users\\samyu\\CLionProjects\\SA-Schefuling-With-Resource/data/results/iterations/iterations_result" << i<< "_" << j <<".txt";
    std::ofstream result;
    result.open(res.str());
    result << minAnswer << " " << maxAnswer << " " << maxAnswer/minAnswer << " " << minMilli << " " << maxMilli;
    result.close();
    std::cout << "Wrote results for (" << i << "," << j << ")" << std::endl;
}

int main() {
    std::cout << std::thread::hardware_concurrency() << std::endl;
    std::vector<int> dags = {471, 423, 455, 310, 480, 280, 387, 142, 213, 143, 244, 250, 66, 483, 281, 282, 484, 316, 334, 67};
    std::vector<int> easy = {224, 420, 63,86,303,466, 140};
    std::vector<int> hard = {402,33,34,499,8,36};
    std::vector<int> medium = {193,95,480,440,353,198,287};
    std::vector<std::thread> pool;
//    for (int j = 0; j < 500; j++) {
//        for (int i = 0; i < 5; i++) {
//            if (pool.size() < std::thread::hardware_concurrency()) {
//                pool.emplace_back(&collectMetrics, i, j);
//            }
//        }
//        for (auto& th: pool) {
//            if (th.joinable()) {
//                th.join();
//            }
//        }
//        pool.clear();
//    }
    for (int i = 0; i < 5; i++) {
        for (auto each: easy) {
            if (pool.size() < std::thread::hardware_concurrency()) {
                pool.emplace_back(&collectMetrics, i, each);
            }
        }
        for (auto& th: pool) {
            if (th.joinable()) {
                th.join();
            }
        }
        pool.clear();
    }
    for (int i = 0; i < 5; i++) {
        for (auto each: medium) {
            if (pool.size() < std::thread::hardware_concurrency()) {
                pool.emplace_back(&collectMetrics, i, each);
            }
        }
        for (auto& th: pool) {
            if (th.joinable()) {
                th.join();
            }
        }
        pool.clear();
    }
    for (int i = 0; i < 5; i++) {
        for (auto each: hard) {
            if (pool.size() < std::thread::hardware_concurrency()) {
                pool.emplace_back(&collectMetrics, i, each);
            }
        }
        for (auto& th: pool) {
            if (th.joinable()) {
                th.join();
            }
        }
        pool.clear();
    }
}
