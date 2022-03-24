#include "iostream"
#include "Parsers/ParametersParser.h"
#include "Parsers/DependencyGraphParser.h"
#include "boost/graph/adjacency_list.hpp"
#include "boost/pending/property.hpp"
#include "boost/property_map/property_map.hpp"
#include "boost/graph/graphml.hpp"
#include "boost/version.hpp"

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
    DependencyGraphParser::parse("/home/samuil/CLionProjects/SA-Schefuling-With-Resource/Parsers/example2.xml");
//    auto cs = parameters.coolingSchedule;
//    cs->setInitialTemperature(100);
//    std::cout << cs->getNextTemperature() << "," << cs->getNextTemperature() << "," << cs->getNextTemperature() << "," << cs->getNextTemperature() << std::endl;

}
