#pragma once

#include<iostream>
#include "json.hpp"
using json = nlohmann::json;

bool check_graph(json& graphJson){
    // Checking format of graph.json
    if(!graphJson.contains("meta") || !graphJson.contains("nodes") || !graphJson.contains("edges") || graphJson.size() != 3){
        std::cerr<<"Graph.json should have 3 correct parameters\n";
    }
        // Checking format of meta 
        auto meta = graphJson["meta"];

        if(!meta.contains("id") || !meta.contains("nodes") || !meta.contains("description") || meta.size() != 3 || !meta.is_object()){
            std::cerr << "Fields missing or extra in meta in graph.json\n";
            return false;
        }

        if(!meta["id"].is_string()){
            std::cerr << "Meta id must be a string\n";
            return false;
        }

        if(!meta["nodes"].is_number_integer()){
            std::cerr << "Meta nodes must be a integer\n";
            return false;
        }

        if(!meta["description"].is_string()){
            std::cerr << "Meta description must be a string\n";
            return false;
        }

        // Checking format of nodes
        auto nodes = graphJson["nodes"];
        if(!nodes.is_array()){
            std::cerr<<"Nodes is an array\n";
        }
        for(auto&node : nodes){

            if(!node.contains("id") || !node.contains("lat") || !node.contains("lon") || !node.contains("pois") || node.size() != 4 || !node.is_object()){
                std::cerr << "Fields missing or extra in nodes in graph.json\n";
                return false;
            }

            if(!node["id"].is_number_integer()){
                std::cerr << "Node id must be a integer\n";
                return false;
            }
            
            if(!node["lat"].is_number()){
                std::cerr << "Node lat must be a float\n";
                return false;
            }

            if(!node["lon"].is_number()){
                std::cerr << "Node lon must be a float\n";
                return false;
            }
            
            if(!node["pois"].is_array()){
                std::cerr<<"Node is a array\n";
            }
            for(auto s : node["pois"]){
                if(!s.is_string()){
                    std::cerr << "Fields in node > pois must be a string\n";
                    return false;
                }
            }
        }

        // Checking format of edges
        auto edges = graphJson["edges"];
        if(!edges.is_array()){
            std::cerr<<"Edges should be a array\n";
        }
        for(auto&edge : edges){
            if(!edge.contains("id") || !edge.contains("u") || !edge.contains("v") || !edge.contains("length") || !edge.contains("average_time") || !edge.contains("oneway") || !edge.contains("road_type") || (edge.size() != 8 && edge.size() != 7) || ((edge.size() == 8) && !edge.contains("speed_profile")) || !edge.is_object()){
                std::cerr << "Fields missing or extra in edges in graph.json\n";
                return false;
            }

            if(!edge["id"].is_number_integer()){
                std::cerr << "Edge id must be a integer\n";
                return false;
            }

            if(!edge["u"].is_number_integer()){
                std::cerr << "Edge u must be a integer\n";
                return false;
            }

            if(!edge["v"].is_number_integer()){
                std::cerr << "Edge v must be a integer\n";
                return false;
            }

            if(!edge["length"].is_number() || edge["length"].get<double>() <= 0){
                std::cerr << "Edge length must be a float\n";
                return false;
            }

            if(!edge["average_time"].is_number() || edge["average_time"].get<double>() <= 0){
                std::cerr << "Edge average time must be a float\n";
                return false;
            }

            if(!edge["oneway"].is_boolean()){
                std::cerr << "Egde oneway must be a boolean\n";
                return false;
            }

            if(!edge["road_type"].is_string()){
                std::cerr << "Edge road type must be a string\n";
                return false;
            }
            if(edge.contains("speed_profile")){
                if(!edges["speed_profile"].is_array()){
                    std::cerr<<"Speed_profile is an array\n";
                }
                if(edge["speed_profile"].size() != 96){
                    // Need to change this for Phase-2
                    std::cerr << "Edge speed profile must have 96 values\n";
                    return false;
                }
                for(auto val : edge["speed_profile"]){
                    if(!val.is_number() || val.get<double>() <= 0){
                        std::cerr << "Edge speed profiles values must be number\n";
                        return false;
                    }
                }
            }
        }
        return true;
}

bool check_queries(json& queriesJson) {
    // Check that "meta" exists
    if (!queriesJson.contains("meta") || !queriesJson.contains("events") || queriesJson.size() != 2 ) {
        std::cerr << "Missing or Extra 'meta' or 'events' in queries.json\n";
        return false;
    }

    // --- META CHECK ---
    auto meta = queriesJson["meta"];
    if (!meta.contains("id") || !meta["id"].is_string() || meta.size() != 1 || !meta.is_object()) {
        std::cerr << "Meta must contain only a string field 'id'\n";
        return false;
    }

    // --- EVENTS CHECK ---
    auto events = queriesJson["events"];
    if (!events.is_array()) {
        std::cerr << "'events' must be an array\n";
        return false;
    }

    // Check each event
    for (auto& event : events) {
        if (!event.contains("type") || !event["type"].is_string() || !event.is_object()) {
            std::cerr << "Each event must have a string field 'type'\n";
            return false;
        }

        std::string type = event["type"];

        // ---- REMOVE_EDGE ----
        if (type == "remove_edge") {
            if (!event.contains("edge_id") || !event["edge_id"].is_number_integer() || event.size() != 2) {
                std::cerr << "remove_edge must have integer 'edge_id' and no extra fields\n";
                return false;
            }
        }

        // ---- MODIFY_EDGE ----
        else if (type == "modify_edge") {
            if (!event.contains("edge_id") || !event["edge_id"].is_number_integer()) {
                std::cerr << "modify_edge must have integer 'edge_id'\n";
                return false;
            }
            if (!event.contains("patch") || !event["patch"].is_object()) {
                std::cerr << "modify_edge must have 'patch' object\n";
                return false;
            }
            if(event.size() != 3){
                std::cerr<<"Query no.of parameter mismatch\n";
            }

            // check patch content keys (optional)
            auto patch = event["patch"];
            for (auto it = patch.begin(); it != patch.end(); ++it) {
                std::string key = it.key();
                if (key == "length" && (!it.value().is_number() || it.value() <= 0)){
                    std::cerr<<"Invalid in length of patch - "<<patch<<"\n";
                    return false;
                }
                else if (key == "average_time" && (!it.value().is_number()  || it.value() <= 0)) return false;
                else if (key == "oneway" && !it.value().is_boolean()) return false;
                else if (key == "road_type" && !it.value().is_string()) return false;
                else if (key == "speed_profile") {
                    if (!it.value().is_array()) {
                        std::cerr << "speed_profile must be an array\n";
                        return false;
                    }
                    if(it.value().size() != 96){
                        std::cerr<<"Speed profile must have 96 values\n";
                    }
                    for (auto val : it.value())
                        if (!val.is_number() || val <= 0) {
                            std::cerr << "speed_profile values must be positive numbers\n";
                            return false;
                        }
                }
                else if (key != "length" && key != "average_time" &&
                         key != "oneway" && key != "road_type" &&
                         key != "speed_profile") {
                    std::cerr << "Invalid field in patch: " << key << "\n";
                    return false;
                }
            }
        }

        // ---- SHORTEST_PATH ----
        else if (type == "shortest_path") {
            if (!event.contains("id") || !event["id"].is_number_integer()) {
                std::cerr << "shortest_path must contain integer 'id'\n";
                return false;
            }
            if (!event.contains("source") || !event["source"].is_number_integer()) {
                std::cerr << "shortest_path missing 'source'\n";
                return false;
            }
            if (!event.contains("target") || !event["target"].is_number_integer()) {
                std::cerr << "shortest_path missing 'target'\n";
                return false;
            }
            if (!event.contains("mode") || !event["mode"].is_string()) {
                std::cerr << "shortest_path missing 'mode'\n";
                return false;
            }
            std::string mode = event["mode"];
            if (mode != "time" && mode != "distance") {
                std::cerr << "mode must be 'time' or 'distance'\n";
                return false;
            }

            if(event.contains("constraints") && event.size() != 6){
                std::cerr<<"No.of parametres in event not matching\n";
            }
            if(!event.contains("constraints") && event.size() != 5){
                std::cerr<<"No.of parametres in event not matching\n";
            }
            if (event.contains("constraints")) {
                auto c = event["constraints"];
                if (!c.is_object()) {
                    std::cerr << "constraints must be an object\n";
                    return false;
                }
                if (c.contains("forbidden_nodes")) {
                    if (!c["forbidden_nodes"].is_array()) {
                        std::cerr << "forbidden_nodes must be an array\n";
                        return false;
                    }
                    for (auto id : c["forbidden_nodes"])
                        if (!id.is_number_integer()) {
                            std::cerr << "forbidden_nodes must contain integers\n";
                            return false;
                        }
                }
                if (c.contains("forbidden_road_types")) {
                    if (!c["forbidden_road_types"].is_array()) {
                        std::cerr << "forbidden_road_types must be an array\n";
                        return false;
                    }
                    for (auto s : c["forbidden_road_types"])
                        if (!s.is_string()) {
                            std::cerr << "forbidden_road_types must contain strings\n";
                            return false;
                        }
                }
            }
        }

        // ---- KNN ----
        else if (type == "knn") {
            if (!event.contains("id") || !event["id"].is_number_integer()) {
                std::cerr << "knn must contain integer 'id'\n";
                return false;
            }
            if (!event.contains("type") || !event["type"].is_string()) {
                std::cerr << "knn must contain string 'type' (POI type)\n";
                return false;
            }
            if (!event.contains("query_point") || !event["query_point"].is_object()) {
                std::cerr << "knn must contain 'query_point' object\n";
                return false;
            }
            auto qp = event["query_point"];
            if (!qp.contains("lat") || !qp["lat"].is_number() ||
                !qp.contains("lon") || !qp["lon"].is_number()) {
                std::cerr << "query_point must have numeric 'lat' and 'lon'\n";
                return false;
            }
            if (!event.contains("k") || !event["k"].is_number_integer()) {
                std::cerr << "knn must contain integer 'k'\n";
                return false;
            }
            if (!event.contains("metric") || !event["metric"].is_string()) {
                std::cerr << "knn must contain string 'metric'\n";
                return false;
            }
            if(event.size() != 6){
                std::cerr<<"No.of parameters in event not matching\n";
            }
        }

        else {
            std::cerr << "Unknown query type: " << type << "\n";
            return false;
        }
    }

    return true; // all good
}
