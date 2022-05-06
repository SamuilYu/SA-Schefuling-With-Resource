#ifndef SA_SCHEDULING_RESOURCES_SCOPEDSCHEDULE_H
#define SA_SCHEDULING_RESOURCES_SCOPEDSCHEDULE_H

#include <utility>
#include "../SimulatedAnnealing/Problem/Solution.h"
#include "boost/graph/topological_sort.hpp"
#include "boost/graph/adjacency_list.hpp"
#include "SchedulingConditions.h"
#include "vector"
#include "stdexcept"
#include "iterator"
#include "boost/graph/reverse_graph.hpp"
#include "boost/graph/depth_first_search.hpp"
#include "boost/graph/copy.hpp"

class collector_visitor: public boost::default_dfs_visitor {
private:
    std::shared_ptr<std::set<int>> collector;
public:
    collector_visitor(const std::shared_ptr<std::set<int>>& c): boost::default_dfs_visitor() {
        collector = c;
    };

    void discover_vertex(int i, const DependencyGraph& g) {
        collector->insert(i);
    }
};

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
        std::mt19937 mt(static_cast<unsigned int>(time(nullptr)));
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
            attempt++;
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
            resultsUsed[index + 1].insert(each);
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

    std::vector<std::shared_ptr<ScopedSchedule>> breakUp(
        std::vector<std::shared_ptr<ScopedSchedule>> previous,
        std::set<std::pair<int, int>> allowedPairs,
        const std::map<int, std::shared_ptr<std::set<int>>>& succ,
        const std::map<int, std::shared_ptr<std::set<int>>>& pred,
        size_t numScopes
    ) {
        auto currentScopes = previous.size();
        size_t remainder;
        if (currentScopes * 2 >= numScopes) {
            remainder = numScopes - currentScopes;
        } else {
            remainder = currentScopes;
        }

        size_t minProduct = LONG_MAX;
        std::pair<int, int> candidate;
        for (auto& each: allowedPairs) {
            size_t newProduct =
                    succ.at(each.first)->size() * pred.at(each.second)->size() +
                    succ.at(each.second)->size() * pred.at(each.first)->size();
            if (newProduct < minProduct) {
                minProduct = newProduct;
                candidate = each;
            }
        }

        std::vector<std::shared_ptr<ScopedSchedule>> current = {};
        for (int i = 0; i < remainder; i++) {
            auto leftScopeGraph = previous[i]->scope.getDependencyGraph();
            auto rightScopeGraph = previous[i]->scope.getDependencyGraph();
            boost::add_edge(candidate.first, candidate.second, leftScopeGraph);
            boost::add_edge(candidate.second, candidate.first, rightScopeGraph);
            auto leftScope = SchedulingConditions(leftScopeGraph);
            auto rightScope = SchedulingConditions(rightScopeGraph);
            current.push_back(std::make_shared<ScopedSchedule>(previous[i]->conditions, leftScope));
            current.push_back(std::make_shared<ScopedSchedule>(previous[i]->conditions, rightScope));
        }
        for (auto i = remainder; i < currentScopes; i++) {
            current.push_back(previous[i]);
        }

        for (auto& pre: *(pred.at(candidate.first))) {
            for (auto& suc: *(succ.at(candidate.second))) {
                allowedPairs.erase(std::make_pair(pre, suc));
            }
        }
        for (auto& pre: *(pred.at(candidate.second))) {
            for (auto& suc: *(succ.at(candidate.first))) {
                allowedPairs.erase(std::make_pair(pre, suc));
            }
        }
        if (remainder < currentScopes || allowedPairs.empty()) {
            return current;
        } else {
            return breakUp(current, allowedPairs, succ, pred, numScopes);
        }
    }

protected:
    std::vector<std::shared_ptr<Solution>> breakScope(int numScopes) override {
        DependencyGraph::vertex_iterator i, j, iend, jend;
        std::set<std::pair<int, int>> pairs = {};
        std::map<int, std::shared_ptr<std::set<int>>> succ = {};
        std::map<int, std::shared_ptr<std::set<int>>> pred = {};

        DependencyGraph reverseGraph;
        for (boost::tie(i, iend) = boost::vertices(scope.getDependencyGraph()); i != iend; i++) {
            boost::add_vertex(*i, reverseGraph);
        }
        for (boost::tie(i, iend) = boost::vertices(scope.getDependencyGraph()); i != iend; i++) {
            for (j = i; j != iend; j++) {
                if (boost::edge(*i, *j, scope.getDependencyGraph()).second) {
                    boost::add_edge(*j, *i, reverseGraph);
                } else if (boost::edge(*j, *i, scope.getDependencyGraph()).second) {
                    boost::add_edge(*i, *j, reverseGraph);
                }
            }
        }

        for (boost::tie(i, iend) = boost::vertices(scope.getDependencyGraph()); i != iend; i++) {
            for (boost::tie(j, jend) = boost::vertices(scope.getDependencyGraph()); j != jend; j++) {
                pairs.insert(std::make_pair(*i, *j));
            }

            auto indexMap1 = boost::get(boost::vertex_index, scope.getDependencyGraph());
            auto colorMap1 = boost::make_vector_property_map<boost::default_color_type>(indexMap1);
            succ[*i] = std::make_shared<std::set<int>>();
            auto vis1 = collector_visitor(succ[*i]);
            boost::depth_first_visit(scope.getDependencyGraph(), *i, vis1, colorMap1);

            auto indexMap2 = boost::get(boost::vertex_index, reverseGraph);
            auto colorMap2 = boost::make_vector_property_map<boost::default_color_type>(indexMap2);
            pred[*i] = std::make_shared<std::set<int>>();
            auto vis2 = collector_visitor(pred[*i]);
            boost::depth_first_visit(reverseGraph, *i, vis2, colorMap2);
        }

        for (auto& each: pred) {
            for (auto v: *(each.second)) {
                pairs.erase(std::make_pair(v, each.first));
                pairs.erase(std::make_pair(each.first, v));
            }
        }

        auto schedules = breakUp({std::make_shared<ScopedSchedule>(*this)}, pairs, succ, pred, numScopes);
        std::vector<std::shared_ptr<Solution>> solutions;
        for (auto& each: schedules) {
            solutions.push_back(each);
        }
        return solutions;
    }
};


#endif //SA_SCHEDULING_RESOURCES_SCOPEDSCHEDULE_H
