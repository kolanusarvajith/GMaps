#include <iostream>
#include <fstream>
#include <chrono>
#include "Graph.hpp"
#include "json.hpp"
#include "check_graph.hpp"

using json = nlohmann::json;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <graph.json> <queries.json>" << std::endl;
        return 1;
    }

    // If no.of arguments are fine then we access the input files
    std::ifstream graph(argv[1]);
    json graphJson;
    graph >> graphJson;

    if(!check_graph(graphJson)) 
        return 1;

    std::vector<Node> nodes ;
    std::vector<Edge> edges;
    for (auto& node : graphJson["nodes"]) {
        Node n(
            node["id"],
            node["lat"],
            node["lon"],
            node["pois"].get<std::vector<std::string>>()
        );
        nodes.emplace_back(std::move(n)); 
    }

    for(auto& edge : graphJson["edges"]){
        if(!edge.contains("speed_profile")){
            edge["speed_profile"] = std::vector<double>(96, edge["length"].get<double>() / edge["average_time"].get<double>());
        }
        Edge e(edge["id"],
            edge["u"],
            edge["v"],
            edge["length"],
            edge["average_time"],
            edge["oneway"],
            edge["road_type"],
            edge["speed_profile"].get<std::vector<double>>(),
            false
        );
    }

    
    std::ifstream queries_file(argv[2]);
    if (!queries_file.is_open()) {
        std::cerr << "Failed to open " << argv[2] << std::endl;
        return 1;
    }
    json queries_json;
    queries_file >> queries_json;

    std::ofstream output_file("output.json");
    if (!output_file.is_open()) {
        std::cerr << "Failed to open output.json for writing" << std::endl;
        return 1;
    }

    for (const auto& query : queries_json) {
        auto start_time = std::chrono::high_resolution_clock::now();

        /*
            Add your query processing code here
            Each query should return a json object which should be printed to sample.json
        */

        // Answer each query replacing the function process_query using 
        // whatever function or class methods that you have implemented
        json result = process_query(query);

        auto end_time = std::chrono::high_resolution_clock::now();
        result["processing_time"] = std::chrono::duration<double, std::milli>(end_time - start_time).count();

        output_file << result.dump(4) << '\n';
    }

    output_file.close();
    return 0;
}




