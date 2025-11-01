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

inline double deg_to_rad(double deg) {
    return deg * M_PI / 180.0;
}

double haversine_distance(const Node& a, const Node& b) {
    double lat1 = deg_to_rad(a.lat);
    double lon1 = deg_to_rad(a.lon);
    double lat2 = deg_to_rad(b.lat);
    double lon2 = deg_to_rad(b.lon);

    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    double h = std::pow(std::sin(dlat / 2), 2) +
               std::cos(lat1) * std::cos(lat2) *
               std::pow(std::sin(dlon / 2), 2);

    return 2 * EARTH_RADIUS * std::asin(std::sqrt(h));
}

double heuristic(const Node& a, const Node& b) {
    return haversine_distance(a, b);
}

inline std::pair<bool, json> shortest_path(
    Graph& graph,
    int source,
    int target,
    const std::string& mode,
    const std::unordered_set<int>& forbidden_nodes,
    const std::unordered_set<std::string>& forbidden_road_types
) {
    if (!graph.nodes.count(source) || !graph.nodes.count(target)) {
        return {false, {}};
    }

    if (mode != "distance" && mode != "time") {
        return {false, {}};
    }

    struct State {
        int node;
        double cost;
        double est_total;
        bool operator>(const State& other) const {
            return est_total > other.est_total;
        }
    };

    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;
    std::unordered_map<int, double> g_cost;
    std::unordered_map<int, int> parent;
    std::unordered_set<int> closed_set;

    for (auto& [id, node] : graph.nodes)
        g_cost[id] = std::numeric_limits<double>::infinity();

    g_cost[source] = 0.0;
    pq.push({source, 0.0, heuristic(graph.nodes[source], graph.nodes[target])});

    while (!pq.empty()) {
        auto [u, cost_u, est_total] = pq.top();
        pq.pop();

        if (closed_set.count(u))
            continue;
        closed_set.insert(u);

        if (forbidden_nodes.count(u))
            continue;

        if (u == target) {
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
            return {true, result};
        }

        for (auto& edge : graph.adjList[u]) {
            if (!graph.nodes.count(edge.v)) continue;
            if (forbidden_road_types.count(edge.road_type)) continue;
            if (forbidden_nodes.count(edge.v)) continue;

            double edge_cost = (mode == "distance") ? edge.length : edge.average_time;
            double new_cost = g_cost[u] + edge_cost;

            if (new_cost + 1e-9 < g_cost[edge.v]) {
                g_cost[edge.v] = new_cost;
                parent[edge.v] = u;

                double h = heuristic(graph.nodes[edge.v], graph.nodes[target]);
                pq.push({edge.v, new_cost, new_cost + h});
            }
        }
    }
    return {false, {}};
}


std::vector<int> knn_euclidean(const Graph& graph,
                               int source_node_id,
                               double query_lat,
                               double query_lon,
                               const std::string& poi_type,
                               int k) {
    if (!graph.nodes.count(source_node_id))
        return {};

    std::priority_queue<std::pair<double, int>> pq;

    for (auto& [node_id, node] : graph.nodes) {
        bool is_poi = false;
        for (auto& t : node.pois) {
            if (t == poi_type) {
                is_poi = true;
                break;
            }
        }
        if (!is_poi) continue;

        double dx = node.lon - query_lon;
        double dy = node.lat - query_lat;
        double dist = std::sqrt(dx * dx + dy * dy);

        pq.push({dist, node_id});
        if ((int)pq.size() > k)
            pq.pop();
    }

    std::vector<int> result;
    while (!pq.empty()) {
        result.push_back(pq.top().second);
        pq.pop();
    }
    std::reverse(result.begin(), result.end());
    return result;
}

std::vector<int> knn_shortest_path(const Graph& graph,
                                   int source_node_id,
                                   const std::string& poi_type,
                                   int k) {
    if (!graph.nodes.count(source_node_id))
        return {};

    std::unordered_map<int, double> dist;
    for (auto& [id, _] : graph.nodes)
        dist[id] = std::numeric_limits<double>::infinity();

    using PDI = std::pair<double, int>;
    std::priority_queue<PDI, std::vector<PDI>, std::greater<PDI>> pq;
    dist[source_node_id] = 0.0;
    pq.push({0.0, source_node_id});

    std::priority_queue<std::pair<double, int>> nearest_pois;
    double max_found_dist = std::numeric_limits<double>::infinity();

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (!nearest_pois.empty() && d > max_found_dist)
            break;

        if (d > dist[u]) continue;

        if (std::find(graph.nodes.at(u).pois.begin(), graph.nodes.at(u).pois.end(), poi_type)
            != graph.nodes.at(u).pois.end()) {
            nearest_pois.push({d, u});
            if ((int)nearest_pois.size() > k)
                nearest_pois.pop();
            if ((int)nearest_pois.size() == k)
                max_found_dist = nearest_pois.top().first;
        }

        for (const auto& edge : graph.adjList.at(u)) {
            double new_dist = d + edge.length;
            if (new_dist < dist[edge.v]) {
                dist[edge.v] = new_dist;
                pq.push({new_dist, edge.v});
            }
        }
    }

    std::vector<int> result;
    while (!nearest_pois.empty()) {
        result.push_back(nearest_pois.top().second);
        nearest_pois.pop();
    }
    std::reverse(result.begin(), result.end());
    return result;
}
