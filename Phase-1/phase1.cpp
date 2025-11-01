#include <iostream>
#include <fstream>
#include <chrono>
#include <filesystem>
#include "Graph.hpp"
#include "json.hpp"
#include "check.hpp"
#include "handle.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc != 3 || fs::path(argv[1]).extension() != ".json" || fs::path(argv[2]).extension() != ".json") {
        std::cerr << "Usage: " << argv[0] << " <graph.json> <queries.json>" << std::endl;
        return 1;
    }

    // --- Load graph.json ---
    std::ifstream graph_file(argv[1]);
    if (!graph_file.is_open()) {
        std::cerr << "Failed to open " << argv[1] << std::endl;
        return 1;
    }
    json graphJson;
    graph_file >> graphJson;

    if (!check_graph(graphJson))
        return 1;

    // --- Construct nodes and edges ---
    std::vector<Node> nodes;
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

    for (auto& edge : graphJson["edges"]) {
        if (!edge.contains("speed_profile")) {
            edge["speed_profile"] = std::vector<double>(96, edge["length"].get<double>() / edge["average_time"].get<double>());
        }
        Edge e(
            edge["id"],
            edge["u"],
            edge["v"],
            edge["length"],
            edge["average_time"],
            edge["oneway"],
            edge["road_type"],
            edge["speed_profile"].get<std::vector<double>>(),
            false
        );
        edges.emplace_back(std::move(e));
    }

    Graph graph(nodes, edges);

    // --- Load queries.json ---
    std::ifstream queries_file(argv[2]);
    if (!queries_file.is_open()) {
        std::cerr << "Failed to open " << argv[2] << std::endl;
        return 1;
    }
    json queriesJson;
    queries_file >> queriesJson;

    if (!check_queries(queriesJson))
        return 1;

    if (!queriesJson.contains("events") || !queriesJson["events"].is_array()) {
        std::cerr << "Invalid queries.json: missing 'events' array\n";
        return 1;
    }

    // --- Open output.json ---
    std::ofstream output_file("output.json");
    if (!output_file.is_open()) {
        std::cerr << "Failed to open output.json for writing" << std::endl;
        return 1;
    }

    // --- Process each query in events ---
   for (const auto& query : queriesJson["events"]) {
        auto start_time = std::chrono::high_resolution_clock::now();

        json result = process_query(query, graph);

        auto end_time = std::chrono::high_resolution_clock::now();
        result["processing_time"] =
            std::chrono::duration<double, std::milli>(end_time - start_time).count();

        output_file << result.dump(4) << '\n';
    }


    output_file.close();
    return 0;
}
