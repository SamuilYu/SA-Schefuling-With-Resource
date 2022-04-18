#ifndef SA_SCHEDULING_RESOURCES_SCHEDULE_H
#define SA_SCHEDULING_RESOURCES_SCHEDULE_H

#include "../SimulatedAnnealing/Problem/Solution.h"
#include "boost/graph/topological_sort.hpp"
#include "SchedulingConditions.h"
#include "vector"
#include "stdexcept"
#include "iterator"

class Schedule: public Solution {
private:
    std::vector<int> value;
    SchedulingConditions conditions;
public:
    explicit Schedule(const SchedulingConditions &conditions) : conditions(conditions) {}

    Solution& operator=(const Solution &init) override {
        if (typeid(init) == typeid(Schedule)) {

            this->value = dynamic_cast<Schedule&>(const_cast<Solution&>(init)).value;
            this->conditions = dynamic_cast<Schedule&>(const_cast<Solution&>(init)).conditions;
            return dynamic_cast<Solution&>(*this);
        } else {
            throw std::logic_error("Cannot assign non-schedule object to schedule.");
        }
    }

    const std::vector<int> &getValue() const {
        return value;
    }

private:
    static long Random(long a) {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<long> dist(0, a - 1);
        return dist(mt);
    }

    void OnInitialize(void) override {
        boost::topological_sort(conditions.getDependencyGraph(), std::back_inserter(value));
        std::reverse(value.begin(), value.end());
    }

    void OnPerturb(double temperature, double initialTemperature) override {
        auto currentVertex = value.begin() + Random((int) value.size());
        auto rCurrentVertex = std::make_reverse_iterator(currentVertex);
        auto closestDescendant = currentVertex;
        auto closestPredecessor = currentVertex;

        for (; closestPredecessor != value.begin(); closestPredecessor--) {
            if (boost::edge(*closestPredecessor, *currentVertex, conditions.getDependencyGraph()).second) {
                break;
            }
        }
        if (closestPredecessor == value.begin()) {
            if (!boost::edge(*closestPredecessor, *currentVertex, conditions.getDependencyGraph()).second) {
                closestPredecessor = value.end();
            }
        }

        for (; closestDescendant != value.end(); closestDescendant++) {
            if (boost::edge(*currentVertex, *closestDescendant, conditions.getDependencyGraph()).second) {
                break;
            }
        }

        auto windowBetweenPredAndCur = closestPredecessor == value.end()?
                currentVertex - value.begin() : (currentVertex - closestPredecessor - 1);
        auto windowBetweenCurAndPost = (closestDescendant - currentVertex - 1);
        if ( windowBetweenCurAndPost + windowBetweenPredAndCur == 0) {
            return;
        }
        auto numberToSwitchWith = Random( windowBetweenCurAndPost + windowBetweenPredAndCur);
        std::vector<int>::iterator theOneToSwitchWith;
        if (numberToSwitchWith < windowBetweenPredAndCur) {
            theOneToSwitchWith = (currentVertex - numberToSwitchWith - 1);
        } else {
            theOneToSwitchWith = currentVertex + (numberToSwitchWith - windowBetweenPredAndCur) + 1;
        }

        std::vector<int> newValue = {};
        for (auto it = value.begin(); it != value.end(); it++) {
            if (it == theOneToSwitchWith) {
                if (theOneToSwitchWith > currentVertex) {
                    newValue.emplace_back(*theOneToSwitchWith);
                    newValue.emplace_back(*currentVertex);
                } else {
                    newValue.emplace_back(*currentVertex);
                    newValue.emplace_back(*theOneToSwitchWith);
                }
            } else if (it != currentVertex) {
                newValue.emplace_back(*it);
            }
        }
        value = newValue;
    }

    double getAllocation(const std::set<long> &set) {
        double allocation = 0.0;
        for (auto each: set) {
            allocation += boost::get(boost::vertex_potential_t(), conditions.getDependencyGraph(), each);
        }
        return allocation;
    }

    double OnComputeError(void) override {
        std::map<long, std::set<long>> adjacentUnvisited;

        DependencyGraph::vertex_iterator i, end;
        for (boost::tie(i, end) = boost::vertices(conditions.getDependencyGraph()); i != end; i++) {
            boost::graph_traits<DependencyGraph>::adjacency_iterator ai, a_end;
            adjacentUnvisited[*i] = {};

            boost::tie(ai, a_end) = boost::adjacent_vertices(*i, conditions.getDependencyGraph());
            for (; ai != a_end; ai++) {
                (adjacentUnvisited[*i]).insert(*ai);
            }
        }

        std::vector<std::set<long>> resultsUsed;
        resultsUsed.emplace_back();
        int index = 0;
        for (auto each: value) {
            resultsUsed.emplace_back(resultsUsed[index]);
            if (!adjacentUnvisited[each].empty()) {
                resultsUsed[index + 1].insert(each);
            }
            for (const auto& entry: adjacentUnvisited) {
                adjacentUnvisited[entry.first].erase(each);
                if (adjacentUnvisited[entry.first].empty()) {
                    resultsUsed[index + 1].erase(entry.first);
                }
            }
            index++;
        }

        double max = 0.0;
        for (const auto& each: resultsUsed) {
            double currentAllocation = getAllocation(each);
            if (currentAllocation > max) {
                max = currentAllocation;
            }
        }

        return max;
    }
};


#endif //SA_SCHEDULING_RESOURCES_SCHEDULE_H
