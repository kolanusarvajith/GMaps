#pragma once

#include<iostream>
#include "json.hpp"
using json = nlohmann::json;

bool check_graph(json graphJson){
    // Checking format of graph.json

        // Checking format of meta 
        auto meta = graphJson["meta"];

        if(!meta.contains("id") || !meta.contains("nodes") || !meta.contains("description") || meta.size() != 3){
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
        for(auto&node : nodes){

            if(!node.contains("id") || !node.contains("lat") || !node.contains("lon") || !node.contains("pois") || node.size() != 4){
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

            for(auto s : node["pois"]){
                if(!s.is_string()){
                    std::cerr << "Fields in node > pois must be a string\n";
                    return false;
                }
            }
        }

        // Checking format of edges
        auto edges = graphJson["edges"];
        
        for(auto&edge : edges){
            if(!edge.contains("id") || !edge.contains("u") || !edge.contains("v") || !edge.contains("length") || !edge.contains("average_time") || !edge.contains("oneway") || !edge.contains("road_type") || (edge.size() != 8 && edge.size() != 7) || ((edge.size() == 8) && !edge.contains("speed_profile"))){
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

            if(!edge["length"].is_number()){
                std::cerr << "Edge length must be a float\n";
                return false;
            }

            if(!edge["average_time"].is_number()){
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
                if(edge["speed_profile"].size() != 96){
                    // Need to change this for Phase-2
                    std::cerr << "Edge speed profile must have 96 values\n";
                    return false;
                }
                for(auto val : edge["speed_profile"]){
                    if(!val.is_number()){
                        std::cerr << "Edge speed profiles values must be number\n";
                        return false;
                    }
                }
            }
        }
        return true;
}