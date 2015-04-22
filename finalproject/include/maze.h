#ifndef MAZE_H
#define MAZE_H
#include<vector>
#include<iostream>

struct vertex;

struct adjVertex{
    vertex *v;
    int risk;
};

struct vertex{
    std::string name;
    std::vector<adjVertex> adj;
    bool visited = false;
    //This int will check the graph after its created to check for unconnected rooms.
    int mazeCheck = -1;
    //ID number for each room.
    int ID = -1;
    //The % chance of falling into a trap going from room to room (weight).
    int risk = 0;
};

class maze
{
    public:
        maze();
        ~maze();
        void createMaze(std::vector<std::string> rooms, std::vector< std::vector<int> > connections);
        void addEdge(std::string v1, std::string v2, int weight);
        void addVertex(std::string name);
        void displayEdges();
        void findDistricts();
        void findDistrictsBFT(std::string startingCity, int id);
        void findShortestPath();
        void findShortestDistance();
    protected:
    private:
        //Flag to determine if the maze was created successfully.
        bool mazeSet = false;
        //vector<edge> edges;
        std::vector<vertex> vertices;

};

#endif // MAZE_H