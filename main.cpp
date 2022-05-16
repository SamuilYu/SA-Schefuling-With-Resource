#include "iostream"
#include "thread"
#include "Parsers/ParametersParser.h"
#include "Parsers/DependencyGraphParser.h"
#include "UniprocessorSchedulingWithResource/Schedule.h"

void collectMetrics(int i, int j) {
    std::ostringstream dag;
    dag << "C:\\Users\\samyu\\CLionProjects\\SA-Schefuling-With-Resource/data/trees/tree" << j <<".txt";
    std::ostringstream opt_name;
    opt_name << "C:\\Users\\samyu\\CLionProjects\\SA-Schefuling-With-Resource/data/trees/opt" << j <<".txt";
    double opt = 0.0;
    std::ifstream opt_file;
    opt_file.open(opt_name.str());
    opt_file >> opt;
    opt_file.close();
    auto dp = DependencyGraphParser::parse(dag.str());
    auto cond = std::make_shared<SchedulingConditions>(*dp);
    std::shared_ptr<Solution> probe = std::make_shared<Schedule>(*cond);
    std::ostringstream par;
    par << "C:\\Users\\samyu\\CLionProjects\\SA-Schefuling-With-Resource/data/parameters/main/main"<< i << ".json";
    auto sa = ParametersParser().parse(par.str(), cond, probe);
    auto maxMilli = 0L;
    auto maxAnswer = 0.0;
    auto minMilli = LONG_MAX;
    auto minAnswer = std::numeric_limits<double>::max();
    std::vector<double> opt_vec = [];
    for (int k=0; k < 5; k++) {
        auto schedule = std::make_shared<Schedule>(*cond);
        auto begin = std::chrono::steady_clock::now();
        auto answer = sa->Start(schedule);
        opt_vec.push_back((answer - opt) / opt);
        auto end = std::chrono::steady_clock::now();
        auto milli = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
//        maxAnswer = answer > maxAnswer? answer: maxAnswer;
//        minAnswer = answer < minAnswer? answer: minAnswer;
//        maxMilli = milli > maxMilli? milli: maxMilli;
//        minMilli = milli < minMilli? milli: minMilli;
    }
    std::ostringstream res;
    res << "C:\\Users\\samyu\\CLionProjects\\SA-Schefuling-With-Resource/data/results/main/accuracy" << i<< "_" << j <<".txt";
    std::ofstream result;
    result.open(res.str());
    result << opt_vec[0] << " " << opt_vec[1] << " " << opt_vec[2] << " " << opt_vec[3] << " " << opt_vec[4];
    result.close();
    std::cout << "Wrote results for (" << i << "," << j << ")" << std::endl;
}

int main() {
    std::cout << std::thread::hardware_concurrency() << std::endl;
    std::vector<int> dags = {471, 423, 455, 310, 480, 280, 387, 142, 213, 143, 244, 250, 66, 483, 281, 282, 484, 316, 334, 67};
//    std::vector<int> easy = {26, 456, 487, 330, 345, 455, 247, 458};
//    std::vector<int> hard = {499, 55, 437, 8, 371, 12};
//    std::vector<int> medium = {466, 79, 14, 183, 277, 364};
//    std::vector<int> easiest = {26, 47, 494, 420, 6, 63, 86, 487, 345, 232, 456, 330, 455};
//    std::vector<int> easy  = {84, 309, 498, 112, 443, 193, 297, 379, 270, 377, 247, 458};
//    std::vector<int> easier_medium = {321, 62, 326, 272, 89, 41, 277, 69, 391, 364};
//    std::vector<int> harder_medium = {276, 14, 52, 362, 16, 50, 51, 466, 79};
//    std::vector<int> hard = {395, 98, 33, 499, 18, 453, 263, 237, 229};
//    std::vector<int> hardest = {68, 437, 448, 8, 310, 424, 230};
    std::vector<int> easy = {3, 7, 8, 5, 4, 1, 0, 6, 2, 9, 13, 17};
    std::vector<int> hard = {14, 11, 18, 15, 12, 10 ,19, 16};
    std::vector<int> medium = {24, 27, 25, 20, 21, 28, 22, 26, 23, 29};

    std::vector<std::thread> pool;
//    for (int i = 0; i < 3; i++) {
//        if (i == 0) {
//            for (auto each: easiest) {
//                pool.emplace_back(&collectMetrics, i, each);
////            collectMetrics(i, each);
//            }
//            for (auto& th: pool) {
//                if (th.joinable()) {
//                    th.join();
//                }
//            }
//            pool.clear();
//        } else if (i >= 1) {
//            for (auto each: easiest) {
////                pool.emplace_back(&collectMetrics, i, each);
//                collectMetrics(i, each);
//            }
//        }
//
//    }
    for (int i = 0; i < 3; i++) {
        if (i==0) {
            for (auto each: easy) {
                pool.emplace_back(&collectMetrics, i, each);
//            collectMetrics(i, each);
            }
            for (auto& th: pool) {
                if (th.joinable()) {
                    th.join();
                }
            }
            pool.clear();
        } else if (i>=1) {
            for (auto each: easy) {
//                pool.emplace_back(&collectMetrics, i, each);
                collectMetrics(i, each);
            }
        }

    }
    for (int i = 0; i < 3; i++) {
        if (i == 0) {
            for (auto each: medium) {
                pool.emplace_back(&collectMetrics, i, each);
//            collectMetrics(i, each);
            }
            for (auto& th: pool) {
                if (th.joinable()) {
                    th.join();
                }
            }
            pool.clear();
        } else if (i >= 1) {
            for (auto each: medium) {
//                pool.emplace_back(&collectMetrics, i, each);
                collectMetrics(i, each);
            }
        }

    }
//    for (int i = 0; i < 4; i++) {
//        if (i == 0) {
//            for (auto each: harder_medium) {
//                pool.emplace_back(&collectMetrics, i, each);
////            collectMetrics(i, each);
//            }
//            for (auto& th: pool) {
//                if (th.joinable()) {
//                    th.join();
//                }
//            }
//            pool.clear();
//        } else if (i == 1) {
//            for (auto each: harder_medium) {
////                pool.emplace_back(&collectMetrics, i, each);
//                collectMetrics(i, each);
//            }
//        }
//
//    }
    for (int i = 0; i < 2; i++) {
        if (i==0) {
            for (auto each: hard) {
                pool.emplace_back(&collectMetrics, i, each);
//            collectMetrics(i, each);
            }
            for (auto& th: pool) {
                if (th.joinable()) {
                    th.join();
                }
            }
            pool.clear();
        } else if (i==1) {
            for (auto each: hard) {
//                pool.emplace_back(&collectMetrics, i, each);
                collectMetrics(i, each);
            }
        }

    }
//    for (int i = 0; i < 2; i++) {
//        if (i == 0) {
//            for (auto each: hardest) {
//                pool.emplace_back(&collectMetrics, i, each);
////            collectMetrics(i, each);
//            }
//            for (auto& th: pool) {
//                if (th.joinable()) {
//                    th.join();
//                }
//            }
//            pool.clear();
//        } else if (i == 1) {
//            for (auto each: hardest) {
////                pool.emplace_back(&collectMetrics, i, each);
//                collectMetrics(i, each);
//            }
//        }
//
//    }
}
