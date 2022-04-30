#include "iostream"
#include "thread"
#include "Parsers/ParametersParser.h"
#include "Parsers/DependencyGraphParser.h"
#include "UniprocessorSchedulingWithResource/Schedule.h"
#include "SimulatedAnnealing/SimulatedAnnealing.h"

int main() {
//    using DependencyGraph =
//            boost::adjacency_list<
//            boost::vecS, boost::vecS, boost::directedS,
//            boost::property<boost::vertex_potential_t, double>>;
//
//    DependencyGraph g;
//    boost::add_vertex(0, g);
//    boost::add_vertex(1, g);
//    boost::add_vertex(2, g);
//    boost::add_vertex(3, g);
//    boost::add_vertex(4, g);
//    boost::add_vertex(5, g);
//    boost::add_edge(0, 1 , g);
//    boost::add_edge(1, 2 , g);
//    boost::add_edge(0, 3 , g);
//    boost::add_edge(3, 4 , g);
//    boost::add_edge(4, 2 , g);
//    boost::graph_traits<DependencyGraph>::vertex_iterator v, v_end;
//    for (std::tie(v, v_end) = vertices(g); v != v_end; v++) {
//        boost::put(boost::vertex_potential_t(), g, *v, 3.0);
//    }
//    boost::dynamic_properties dp;
//    dp.property("potential", get(boost::vertex_potential_t(), g));
//
//    boost::write_graphml(std::cout, g, dp);
//    std::cout << BOOST_VERSION << std::endl;
    std::cout << std::thread::hardware_concurrency << std::endl;
    auto dp = DependencyGraphParser::parse("/home/samuil/CLionProjects/SA-Schefuling-With-Resource/data/graphs/dag156.txt");
    auto conditions = SchedulingConditions(*dp);
    auto probe = Schedule(conditions);
    auto parameters = ParametersParser().parse("/home/samuil/CLionProjects/SA-Schefuling-With-Resource/Parsers/example.json", &conditions, &probe);
    auto sa = SimulatedAnnealing(parameters.coolingSchedule.get(),
                                 parameters.temperatureProvider.get(),
                                 parameters.acceptanceDistribution.get(),
                                 100,100);
    auto schedule = Schedule(conditions);
    auto wk1 = Schedule(conditions);
    auto wk2 = Schedule(conditions);
    sa.Start(&schedule, &wk1, &wk2, 30);
    for (auto each: schedule.getValue()) {
        std::cout << each << " ";
    }
    std::cout << std::endl << schedule.GetError();
    //    auto cs = parameters.coolingSchedule;
//    cs->setInitialTemperature(100);
//    std::cout << cs->getNextTemperature() << "," << cs->getNextTemperature() << "," << cs->getNextTemperature() << "," << cs->getNextTemperature() << std::endl;

}
