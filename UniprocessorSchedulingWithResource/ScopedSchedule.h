#ifndef SA_SCHEDULING_RESOURCES_SCOPEDSCHEDULE_H
#define SA_SCHEDULING_RESOURCES_SCOPEDSCHEDULE_H

#include <utility>

#include "../SimulatedAnnealing/Problem/Solution.h"
#include "boost/graph/topological_sort.hpp"
#include "SchedulingConditions.h"
#include "vector"
#include "stdexcept"
#include "iterator"

class ScopedSchedule: public Solution {
private:
    std::vector<int> value;
    std::vector<int> previousValue;
    SchedulingConditions conditions;
    SchedulingConditions scope;
public:
    explicit ScopedSchedule(const SchedulingConditions& conditions, const SchedulingConditions& scope) :
    conditions(conditions), scope(scope) {}


    ScopedSchedule& operator=(const ScopedSchedule& other) {
        this -> value = other.value;
        this -> previousValue = other.previousValue;
        this -> conditions = other.conditions;
        this -> scope = other.scope;
        return *this;
    }

    std::shared_ptr<Solution> clone() override {
        return std::make_shared<ScopedSchedule>(*this);
    }

    Solution& operator=(const Solution &init) override {
        if (typeid(init) == typeid(ScopedSchedule)) {

            this->value = dynamic_cast<ScopedSchedule&>(const_cast<Solution&>(init)).value;
            this->previousValue = dynamic_cast<ScopedSchedule&>(const_cast<Solution&>(init)).previousValue;
            this->conditions = dynamic_cast<ScopedSchedule&>(const_cast<Solution&>(init)).conditions;
            this->scope = dynamic_cast<ScopedSchedule&>(const_cast<Solution&>(init)).scope;
            return dynamic_cast<Solution&>(*this);
        } else {
            throw std::logic_error("Cannot assign non-schedule object to schedule.");
        }
    }

    const std::vector<int> &getValue() const {
        return value;
    }

    void SetPrevious() override {
        value = previousValue;
    }

private:
    static long Random(long a) {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<long> dist(0, a - 1);
        return dist(mt);
    }

    void OnInitialize(void) override {
        value = {};
        boost::topological_sort(scope.getDependencyGraph(), std::back_inserter(value));
        std::reverse(value.begin(), value.end());
    }

    void OnPerturb(double temperature, double initialTemperature) override {
        bool squeezed = true;
        int attempt = 0;
        long windowBetweenPredAndCur;
        long windowBetweenCurAndPost;
        auto currentVertex = value.begin() + Random((int) value.size());
        auto closestDescendant = currentVertex;
        auto closestPredecessor = currentVertex;
        while (squeezed) {
            currentVertex = value.begin() + Random((int) value.size());
            closestDescendant = currentVertex;
            closestPredecessor = currentVertex;

            for (; closestPredecessor != value.begin(); closestPredecessor--) {
                if (boost::edge(*closestPredecessor, *currentVertex, scope.getDependencyGraph()).second) {
                    break;
                }
            }
            if (closestPredecessor == value.begin()) {
                if (!boost::edge(*closestPredecessor, *currentVertex, scope.getDependencyGraph()).second) {
                    closestPredecessor = value.end();
                }
            }

            for (; closestDescendant != value.end(); closestDescendant++) {
                if (boost::edge(*currentVertex, *closestDescendant, scope.getDependencyGraph()).second) {
                    break;
                }
            }
            windowBetweenPredAndCur = closestPredecessor == value.end()?
                                           currentVertex - value.begin() : (currentVertex - closestPredecessor - 1);
            windowBetweenCurAndPost = (closestDescendant - currentVertex - 1);
            squeezed = (attempt < value.size()) && (windowBetweenCurAndPost + windowBetweenPredAndCur == 0);
        }

        if ( windowBetweenCurAndPost + windowBetweenPredAndCur == 0) {
            previousValue = value;
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
        previousValue = value;
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

    std::vector<std::shared_ptr<Solution>> breakScope(int numScopes) override {
        return {};
    }
};


#endif //SA_SCHEDULING_RESOURCES_SCOPEDSCHEDULE_H
