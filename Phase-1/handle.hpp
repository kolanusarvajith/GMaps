#pragma once

#include<iostream>
#include "json.hpp"
#include "Graph.hpp"
#include "pathfinding.hpp"

using json = nlohmann::json;

json process_query(const json& query, Graph& graph) {
    std::string type = query["type"];

    if (type == "remove_edge") {
        int edge_id = query["edge_id"];
        graph.removeEdge(graph.edges[edge_id]);
        return {{"done", true}};
    }
    else if (type == "modify_edge") {
        int edge_id = query["edge_id"];
        graph.modifyEdge(edge_id, query["patch"]);
        return {{"done", true}};
    }
    else if (type == "shortest_path") {
        // compute shortest path here
        // return {{"id", query["id"]}, {"possible", true}, {"minimum_time", 123.4}, {"path", {10, 20, 30}}};
        int source = query["source"];
        int target = query["target"];
        std::string mode = query["mode"];

        std::unordered_set<int> forbidden_nodes;
        std::unordered_set<std::string>forbidden_road_types;

        if (query.contains("constraints")) {
            auto cons = query["constraints"];
            if (cons.contains("forbidden_nodes")) {
                for (auto& n : cons["forbidden_nodes"])
                    forbidden_nodes.insert(n.get<int>());
            }
            if (cons.contains("forbidden_road_types")) {
                for (auto& r : cons["forbidden_road_types"])
                    forbidden_road_types.insert(r.get<std::string>());
            }
        }
        auto [found , result] = shortest_path(graph , source , target , mode , forbidden_nodes , forbidden_road_types);

        json out;
        out["id"] = query["id"];
        out["possible"] = found;

        if(found)
            out.update(result);
            
        return out;
    }
    else if (type == "knn") {
        // compute KNN
    }

    return {{"error", "unknown query type"}};
}























































