#pragma once

#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <vector>
#include <cmath>
#include <limits>
#include "Graph.hpp"
#include "json.hpp"

using json = nlohmann::json;

constexpr double EARTH_RADIUS_KM = 6371000.0;

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

    return 2 * EARTH_RADIUS_KM * std::asin(std::sqrt(h));
}

double heuristic(const Node& a , const Node& b){
    return haversine_distance(a,b);
}

inline std::pair<bool , json> shortest_path(
    Graph& graph,
    int source,
    int target,
    const std::string&node,
    const std::unordered_set<int>& forbidden_nodes,
    const std::unordered_set<std::string>& forbidden_road_types
){
    struct State{
        int node. 
    }
}




