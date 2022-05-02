#ifndef SA_SCHEDULING_RESOURCES_SCHEDULINGCONDITIONS_H
#define SA_SCHEDULING_RESOURCES_SCHEDULINGCONDITIONS_H

#include "../SimulatedAnnealing/Problem/Conditions.h"
#include "boost/graph/copy.hpp"

using DependencyGraph =
boost::adjacency_list<
        boost::vecS, boost::vecS, boost::directedS,
        boost::property<boost::vertex_potential_t, double>>;

class SchedulingConditions: public Conditions {
private:
    DependencyGraph g;
public:
    explicit SchedulingConditions(const DependencyGraph &g) : g(g) {}

    SchedulingConditions& operator=(const SchedulingConditions& other) {
        this -> g = other.g;
    }

    [[nodiscard]] const DependencyGraph& getDependencyGraph() const {
        return g;
    }

    double estimateMax() override {
        auto sum = 0.0;
        boost::graph_traits<DependencyGraph>::vertex_iterator v, v_end;
        for (std::tie(v, v_end) = vertices(g); v != v_end; v++) {
            sum += boost::get(boost::vertex_potential_t(), g, *v);
        }
        return sum;
    }

    double estimateMin() override {
        auto max = boost::get(boost::vertex_potential_t(), g, *(vertices(g).first));
        boost::graph_traits<DependencyGraph>::vertex_iterator v, v_end;
        for (std::tie(v, v_end) = vertices(g); v != v_end; v++) {
            auto cur = boost::get(boost::vertex_potential_t(), g, *v);
            if (max < cur) {
                max = cur;
            }
        }
        return max;
    }

    std::shared_ptr<Conditions> clone() override {
        return std::make_shared<SchedulingConditions>(g);
    }
};


#endif //SA_SCHEDULING_RESOURCES_SCHEDULINGCONDITIONS_H
