#pragma once

#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include "Graph.hpp"
#include "json.hpp"

using json = nlohmann::json;

constexpr double EARTH_RADIUS = 6371000.0;

inline double deg_to_rad(double deg){
    return deg * M_PI / 180.0 ;
}

double haversine_distance(const Node& a , const Node& b){
    double lat1 = deg_to_rad(a.lat);
    double lon1 = deg_to_rad(a.lon);
    double lat2 = deg_to_rad(b.lat);
    double lon2 = deg_to_rad(b.lon);

    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    double h = std::pow(std::sin(dlat / 2), 2) + std::cos(lat1) * std::cos(lat2) * std::pow(std::sin(dlon / 2), 2);

    return 2 * EARTH_RADIUS * std::asin(std::sqrt(h));
}

double heuristic(const Node& a , const Node& b){
    return haversine_distance(a,b);
}

inline std::pair<bool , json> shortest_path(
    Graph& graph,
    int source,
    int target,
    const std::string&mode,
    const std::unordered_set<int>& forbidden_nodes,
    const std::unordered_set<std::string>& forbidden_road_types
){
    if(!graph.nodes.count(source) || !graph.nodes.count(target)){
        return {false,{}};
    }

    if(mode != "distance" && mode != "time"){
        return {false , {}};
    }
    struct State {
        int node;
        double cost;
        double est_total;
        bool operator>(const State& other) const {
            return est_total > other.est_total;
        }
    };

    std::priority_queue<State, std::vector<State> , std::greater<State>> pq;
    std::unordered_map<int, double> g_cost;
    std::unordered_map<int, int> parent;
    std::unordered_set<int> closed_set;

    for(auto& [id, node] : graph.nodes)
        g_cost[id] = std::numeric_limits<double>::infinity();

    g_cost[source] = 0.0;
    pq.push({source , 0.0 , heuristic(graph.nodes[source] , graph.nodes[target])});

    while(!pq.empty()){
        auto [u, cost_u , est_total] = pq.top();
        pq.pop();

        if(closed_set.count(u)) 
            continue;
        closed_set.insert(u);

        if(forbidden_nodes.count(u))
            continue;

        if(u == target){
            if (!parent.count(target) && source != target)
                return {false, {}};
            std::vector<int> path;
            for (int curr = target; curr != source; curr = parent[curr])
                path.push_back(curr);
            path.push_back(source);
            std::reverse(path.begin(), path.end());

            double total_cost = g_cost[target];
            json result;

            if (mode == "distance")
                result["minimum_distance"] = total_cost;
            else
                result["minimum_time"] = total_cost;

            result["path"] = path;
            return {true , result};
        }
        for(auto& edge : graph.adjList[u]){
            if(!graph.nodes.count(edge.v)) continue;
            if(forbidden_road_types.count(edge.road_type))
                continue;
            if(forbidden_nodes.count(edge.v))
                continue;
            
            double edge_cost = (mode == "distance") ? edge.length : edge.average_time;
            double new_cost = g_cost[u] + edge_cost;

            if(new_cost + 1e-9 < g_cost[edge.v]){
                g_cost[edge.v] = new_cost;
                parent[edge.v] = u;

                double h = heuristic(graph.nodes[edge.v] , graph.nodes[target]);
                pq.push({edge.v , new_cost, new_cost + h});
            }
        }
    }
    return {false , {}};
}




