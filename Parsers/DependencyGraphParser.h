#ifndef SA_SCHEDULING_RESOURCES_DEPENDENCYGRAPHPARSER_H
#define SA_SCHEDULING_RESOURCES_DEPENDENCYGRAPHPARSER_H

#include "fstream"
#include "boost/graph/graphml.hpp"
#include "../UniprocessorSchedulingWithResource/SchedulingConditions.h"

class DependencyGraphParser {
public:
    static std::shared_ptr<DependencyGraph> parse(const std::string& fileName) {
        DependencyGraph g;
        boost::dynamic_properties dp;
        dp.property("potential", get(boost::vertex_potential_t(), g));

        std::ifstream f;
        f.open(fileName);

        int vertex;
        int child;
        double potential;
        std::string line;
        std::getline(f, line);
        while(std::getline(f, line)) {
            std::istringstream str(line);
            str >> vertex >> potential;
            boost::add_edge(vertex, vertex, g);
            boost::remove_edge(vertex, vertex, g);
            boost::put(boost::vertex_potential_t(), g, vertex, potential);
            while (str >> child) {
                boost::add_edge(vertex, child, g);
            }
        }
        f.close();
        return std::make_shared<DependencyGraph>(g);
    }
};


#endif //SA_SCHEDULING_RESOURCES_DEPENDENCYGRAPHPARSER_H
