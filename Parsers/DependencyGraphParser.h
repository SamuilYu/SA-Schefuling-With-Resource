#ifndef SA_SCHEDULING_RESOURCES_DEPENDENCYGRAPHPARSER_H
#define SA_SCHEDULING_RESOURCES_DEPENDENCYGRAPHPARSER_H

#include "fstream"
#include "boost/graph/graphml.hpp"
#include "../UniprocessorSchedulingWithResource/SchedulingConditions.h"

class DependencyGraphParser {
public:
    static std::shared_ptr<DependencyGraph> parse(const std::string& fileName) {
        std::ifstream f;
        f.open(fileName);
        DependencyGraph g;
        boost::dynamic_properties dp;
        dp.property("potential", get(boost::vertex_potential_t(), g));
        boost::read_graphml(f, g, dp);
        f.close();
        return std::make_shared<DependencyGraph>(g);
    }
};


#endif //SA_SCHEDULING_RESOURCES_DEPENDENCYGRAPHPARSER_H
