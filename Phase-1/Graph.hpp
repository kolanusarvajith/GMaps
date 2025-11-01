#pragma once
// Even if Graph.hpp was included multiple time to avoid redefinition error

#include<vector>
#include<string>
#include<unordered_map>
#include<unordered_set>
#include "json.hpp"

using json = nlohmann::json;
using json = nlohmann::json;

struct Node{
    int id;
    double lat , lon;
    std::vector<std::string> pois;

    Node() : id(0), lat(0.0), lon(0.0), pois({}) {}

    Node(int id, double lat, double lon, const std::vector<std::string>& pois)
    : id(id), lat(lat), lon(lon), pois(pois) {}

};

struct Edge{
    int id , u , v;
    double length , average_time;
    bool oneway;
    std::string road_type;
    std::vector<double> speed_profile;
    bool reverse;
    // To check if two edges are equal we are checking by id
    bool operator==(const Edge& other) const{
        return id == other.id;
    }

    Edge Reverse() const{
        Edge rev = *this;
        std::swap(rev.u , rev.v);
        rev.id *= -1;
        rev.reverse = !rev.reverse;  
        return rev;
    }

    Edge() : id(0), u(0), v(0), length(0.0), average_time(0.0),
            oneway(false), road_type(""), speed_profile({}), reverse(false) {}

    Edge(int id,
         int u,
         int v,
         double length,
         double average_time,
         bool oneway,
         const std::string& road_type,
         const std::vector<double>& speed_profile,
         bool reverse = false)
        : id(id),
          u(u),
          v(v),
          length(length),
          average_time(average_time),
          oneway(oneway),
          road_type(road_type),
          speed_profile(speed_profile),
          reverse(reverse) {}
};


// Custom hash
struct EdgeHash{
    size_t operator()(const Edge& e) const{
        return std::hash<int>()(e.id);
    }
};

class Graph{
public:
    std::unordered_map<int , Node>nodes;//Access node via node id
    std::unordered_map<int , Edge> edges;// Access edge via edge id
    std::unordered_set<Edge , EdgeHash> edgeList;
    std::unordered_map<int , std::unordered_set<Edge, EdgeHash>> adjList;

    // constructor
    Graph(std::vector<Node>& nodes , std::vector<Edge>& edges){
        nodes.clear();
        edgeList.clear();
        adjList.clear();

        for(Node& node : nodes){
            this->nodes[node.id] = node;
        }

        for(Edge&edge : edges){
            this->edges[edge.id] = edge;
            edgeList.insert(edge);
            adjList[edge.u].insert(edge);

            if(!edge.oneway){
                Edge rev = edge.Reverse();
                adjList[rev.u].insert(rev);
                edgeList.insert(rev);
            }
        }
        
    }

    void addNode(const Node& node){
        nodes[node.id] = node;
    }

    void addEdge(const Edge&e){
        edgeList.insert(e);
        adjList[e.u].insert(e); 
        

        // If not oneway add reverse edge also
        if(!e.oneway){
            Edge rev = e.Reverse();
            adjList[rev.u].insert(rev);
            edgeList.insert(rev);
        }  
    }

    void removeEdge(const Edge&e){
        adjList[e.u].erase(e);
        edgeList.erase(e);

        // If not oneway we should also remove reverse edges
        if(!e.oneway){
            Edge rev = e.Reverse();
            adjList[rev.u].erase(rev);
            edgeList.erase(rev);
        }
    }

    void modifyEdge(int id , const json& patch){
        Edge& e = edges[id];
        if(patch.contains("length")) e.length = patch["length"];
        if(patch.contains("average_time")) e.average_time = patch["average_time"];
        if(patch.contains("oneway")) e.oneway = patch["oneway"];
        if(patch.contains("road_type")) e.road_type = patch["road_type"];
        if (patch.contains("speed_profile")) {
            e.speed_profile = patch["speed_profile"].get<std::vector<double>>();
        }
    }

};




