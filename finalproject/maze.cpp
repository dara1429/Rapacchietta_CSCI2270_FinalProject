#include "maze.h"
#include <vector>
#include <string>
#include <queue>
#include <climits>
using namespace std;
maze::maze()
{
    //Initializes class maze.
}

maze::~maze()
{
    //destructor.
}

bool maze::createMaze(vector<string> rooms, vector< vector<int> > connections)
{
    // These strings are used for creating the edges.
    string startingRoom;
    string connectedRoom;
    int counter = 0;
    int risk = 0;
    int option = 0;

    //Creates Vertexes
    for(int i = 0; i < rooms.size()-1; i++)
    {
        addVertex(rooms[i]);
    }

    //Creates Edges.
    while(counter < rooms.size())
    {
        startingRoom = rooms[counter];
        for(int i = 0; i < rooms.size()-1; i++)
        {
            connectedRoom = rooms[i];
            risk = connections[counter][i];
            if(risk != -1 and risk != -2)
            {
                addEdge(startingRoom,connectedRoom,risk,option);
            }
        }
        counter++;
    }

    //Assign room ID's
    int id = 1;
    for(int i = 0; i < vertices.size(); i++)
    {
        vertices[i].ID = id;
        id++;
    }

    //Assign mazeCheck ID's (for maze error checking)
    mazeErrorCheck();

    //Checks to see if graph is valid. (No disconnected rooms).
    bool error = false;
    for(int i = 0; i < vertices.size(); i++)
    {
        if(vertices[i].mazeCheck != 1)
        {
            error = true;
            cout << "ERROR : ROOM -> " << vertices[i].name << " is not connected to the rest of your maze!" << endl;
            cout << "----------------------------------------------------------------" << endl;
            cout << "Please edit your maze.txt file and make sure " << vertices[i].name << " is connected to the rest of your maze" << endl;
        }
    }
    //Checks to see if any two rooms have different weights(risks) between them.
    int check = 0;
    int riskCheck = 0;
    for(int i = 0; i < vertices.size(); i++)
    {
        for(int j = 0; j < vertices[i].adj.size(); j++)
        {
            riskCheck = vertices[i].adj[j].risk;
            if(riskCheck > 50)
            {
                error = true;
                cout << vertices[i].adj[j].v->name << " has risk value greater than 50!" << endl;
                cout << "---------------------------------------------------------------" << endl;
                cout << "Please choose risk values lower than 50 (50%)" << endl;
            }
            check++;
        }
    }
    if(check % 2)
    {
        error = true;
        cout << "ERROR : One of your rooms contains a one way path!" << endl;
        cout << "Example : room1 has risk 50 to go room2 but room2 has -1 risk to go to room1" << endl;
        cout << "-----------------------------------------------------------------------" << endl;
        cout << "Please check your maze.txt file for -1's that you forgot to change" << endl;
    }
    return error;
}

void maze::addVertex(string room)
{
    bool found = false;
    for(int i = 0; i < vertices.size(); i++){
        if(vertices[i].name == room){
            found = true;
            cout<<vertices[i].name<<" already in maze. Continuing to add connection..."<<endl;
        }
    }
    if(found == false){
        vertex v;
        v.name = room;
        vertices.push_back(v);

    }
}

void maze::addEdge(string room1, string room2, int risk, int option)
{
    //cout << "Adding " << room1 << " " << room2 << endl;
    for(int i = 0; i < vertices.size(); i++){
        if(vertices[i].name == room1){
            for(int j = 0; j < vertices.size(); j++){
                if(vertices[j].name == room2 && i != j){
                    adjVertex av;
                    av.v = &vertices[j];
                    av.risk = risk;
                    vertices[i].adj.push_back(av);
                    //If user uses the add room option in the menu.
                    if(option == 1)
                    {
                        //another vertex for edge in other direction.
                        adjVertex av2;
                        av2.v = &vertices[i];
                        av2.risk = risk;
                        vertices[j].adj.push_back(av2);
                    }
                }
            }
        }
    }
}

void maze::mazeErrorCheck()
{
    //Initializes errorCheck.
    int errorCheck = 1;
    string startingRoom;
    for(int i = 0; i < vertices.size(); i++)
    {
        if(vertices[i].mazeCheck == -1)
        {
            startingRoom = vertices[i].name;
            mazeErrorCheckBFT(startingRoom,errorCheck);
            errorCheck++;
        }
    }
}

void maze::mazeErrorCheckBFT(string startingRoom, int errorCheck)
{
    //Room is assigned an errorCheck number. If its disconnected from the rest of the graph. will return with -1 errorCheck.
    vertex v;
    for(int i = 0; i < vertices.size(); i++)
    {
        if(vertices[i].name == startingRoom)
        {
            v = vertices[i];
            //Mark room as visited.
            vertices[i].visited = true;
            vertices[i].mazeCheck = errorCheck;
        }
    }
    //Create queue for BFS
    queue<vertex> q;

    //Add starting room to queue.
    q.push(v);

    while(!q.empty())
    {
        v = q.front();
        q.pop();

        for(int i = 0;i < v.adj.size(); i++)
        {
            if(v.adj[i].v->visited == false)
            {
                v.adj[i].v->visited = true;
                v.adj[i].v->mazeCheck = errorCheck;
                q.push(*v.adj[i].v);
            }
        }
    }

}

void maze::findSafestPath()
{
    //Holds path to shortest route.
    vector<vertex> solved;

    //Used for calculating average risk.
    int counter = -1;
    //Entrance
    vertex *room1;
    //Exit
    vertex *room2;

    for(int i = 0; i < vertices.size(); i++)
    {
        if(vertices[i].name == "Entrance" or vertices[i].name == "entrance")
        {
            room1 = &vertices[i];
            vertices[i].risk = 0;
            //Adds origin to solved.
            solved.push_back(vertices[i]);
        }
    }
    for(int i = 0; i < vertices.size(); i++)
    {
        if(vertices[i].name == "Exit" or vertices[i].name == "exit")
        {
            room2 = &vertices[i];
        }
    }

    //Previous room
    int previousSize = vertices.size();
    int previous[previousSize];
    //Keeps track of current distance and finds the minimum amount of risk from one room to another.
    int minDistance = 0;
    int distance = 0;
    vertex *minimumRoom;
    //Initializing all rooms.
    for(int i = 0; i < vertices.size(); i++)
    {
        vertices[i].visited = false;
        vertices[i].risk = INT_MAX;
        previous[vertices[i].ID] = -1;
    }

    //Initialize first room so we don't check it.
    room1->visited = true;
    room1->risk = 0;

    //While loop to find the minimum weighted(riskiest) path.
    //Distance = risk.
    while(room2->visited == false)
    {
        minDistance = INT_MAX;
        for(int i = 0; i < solved.size(); i++)
        {
            for(int j = 0; j < solved[i].adj.size(); j++)
            {
                if(solved[i].adj[j].v->visited == false)
                {
                    distance = (solved[i].risk + solved[i].adj[j].risk);
                    if(distance < minDistance)
                    {
                        minDistance = distance;
                        minimumRoom = solved[i].adj[j].v;
                        solved[i].adj[j].v->risk = minDistance;
                        previous[minimumRoom->ID] = solved[i].ID;
                    }
                }
            }

        }
        //Update Solved.
        minimumRoom->visited = true;
        solved.push_back(*minimumRoom);
        //Checks to see if solved room was the exit.
        for(int k = 0; k < vertices.size(); k++)
        {
            if(room2->name == minimumRoom->name)
            {
                room2->visited = true;
            }
            if(minimumRoom->name == vertices[k].name)
            {
                vertices[k].visited = true;
            }
        }
    }
    //Traverses the previous pointers and puts them into a vector for printing.
    distance = minDistance;
    vector<vertex> printPath;
    int currentID = minimumRoom->ID;
    while(currentID != -1)
    {
        for(int i = 0; i < vertices.size(); i++)
        {
            if(currentID == vertices[i].ID)
            {
                printPath.push_back(vertices[i]);
                counter++;
            }

        }
        currentID = previous[currentID];
    }
    //Prints the safest path and average risk of traversing that path.
    double avgrisk = distance / counter;
    int i = 0;
    cout << "The safest path in the maze is :" << endl;
    while(!printPath.empty())
    {
        if(i != counter)
        {cout << printPath.back().name << "->";}
        else
        {
            cout << printPath.back().name;
        }
        printPath.pop_back();
        i++;
    }
    cout << endl;
    cout << "The average change of falling into a trap while taking this path is : " << avgrisk << "%" << endl;
    cout << endl;
}

void maze::findShortestPath()
{
    //Entrance
    vertex *room1;
    //Exit
    vertex *room2;
    for(int i = 0; i < vertices.size(); i++)
    {
        if(vertices[i].name == "entrance" or vertices[i].name == "Entrance")
        {
            room1 = &vertices[i];
        }
    }
    for(int i = 0; i < vertices.size(); i++)
    {
        if(vertices[i].name == "exit" or vertices[i].name == "Exit")
        {
            room2 = &vertices[i];
        }
    }

    //Initialize rooms
    for(int i = 0; i < vertices.size(); i++)
    {
        vertices[i].visited = false;
    }

    //Mark origin as visited.
    room1->visited = true;
    room1->distance = 0;

    //Initialize empty queue and blank vertex
    queue<vertex*> q;
    vertex u;

    //Initialize path/minimum distance/pointer to previous node
    int minDistance = INT_MAX;
    vector<vertex> path;
    int previousSize = vertices.size();
    int previous[previousSize];
    previous[room1->ID] = -1;

    //Add Origin to q.
    q.push(room1);

    //While the queue is not empty find the shortest path from room1->room2
    while(!q.empty())
    {
        u = *q.front();
        q.pop();
        for(int i = 0; i < u.adj.size(); i++)
        {
            if(u.adj[i].v->visited == false)
            {
                u.adj[i].v->distance = (u.distance + 1); //u is the parent.
                if(u.adj[i].v->name == room2->name)
                {
                    if(u.adj[i].v->distance < minDistance)
                    {
                        previous[u.adj[i].v->ID] = u.ID;
                        minDistance = u.adj[i].v->distance;
                    }
                }
                else
                {
                    previous[u.adj[i].v->ID] = u.ID;
                    u.adj[i].v->visited = true;
                    q.push(u.adj[i].v);
                }
            }
        }
    }
    int counter = -1;
    int currentID = room2->ID;
    while(currentID != -1)
    {
        for(int i = 0; i < vertices.size(); i++)
        {
            if(currentID == vertices[i].ID)
            {
                path.push_back(vertices[i]);
                counter++;
            }

        }
        currentID = previous[currentID];
    }
    //Prints the shortest path.
    int i = 0;
    while(!path.empty())
    {
        if(i != counter)
        {cout << path.back().name << "->";}
        else
        {
            cout << path.back().name;
        }
        path.pop_back();
        i++;
    }
    cout << endl;
    cout << "The length of the shortest path in your maze is : " << minDistance << endl;
    cout << endl;
}

void maze::addDoorAndKey()
{
    //string room is the room the user wants to put a door/key in.
    string room;
    bool roomFound = false;
    //booleans used to check if the rooms the user inputted already have a door/key
    bool flag1 = false;
    bool flag2 = false;
    cout << "Enter the name of the room you want to put a door in :" << endl;
    getline(cin,room);
    while(roomFound == false)
    {
        flag1 = false;
        flag1 = false;
        getline(cin,room);
        for(int i = 0; i < vertices.size(); i++)
        {
            if(room == vertices[i].name)
            {
                if(vertices[i].door == false and vertices[i].key == false)
                {
                    roomFound = true;
                    vertices[i].door = true;
                    cout << room << " is now locked" << endl;
                }
                else if(vertices[i].door == true)
                {
                    cout << room << " is already locked. Enter another room" << endl;
                    flag1 = true;
                }
                else
                {
                    cout << room << " has a key in it. Enter another room" << endl;
                    flag2 = true;
                }
            }
        }
        if(roomFound == false and flag1 == false and flag2 == false)
        {
            cout << "Room not found! Please try again" << endl;
        }
    }
    //Resets roomFound boolean and now string room is the room the user wants to put a key in.
    roomFound = false;
    cout << "Now enter the name of the room you want to put a key in :" << endl;
    while(roomFound == false)
    {
        flag1 = false;
        flag2 = false;
        getline(cin,room);
        for(int i = 0; i < vertices.size(); i++)
        {
            if(room == vertices[i].name)
            {
                if(vertices[i].door == false and vertices[i].key == false)
                {
                    roomFound = true;
                    vertices[i].key = true;
                    cout << room << " now has a key in it" << endl;
                }
                else if(vertices[i].door == true)
                {
                    cout << room << " has a door already. Enter a room thats not locked" << endl;
                    flag1 = true;
                }
                else
                {
                    cout << room << " has a key already. Enter a room that does not have a key" << endl;
                    flag2 = true;
                }
            }
        }
        if(roomFound == false and flag1 == false and flag2 == false)
        {
            cout << "Room not found! Please try again" << endl;
        }
    }
}

void maze::addRoom()
{
    //String room1 is the new room to be added.
    string room1;
    //String room2 are the rooms the user connects to room1.
    string room2;
    //risk holds the risk value going from room1 to room2.
    int risk;
    //True when room1 is successfully added.
    bool roomOk = false;
    //error flags set to true if room1 already exists or is another exit,entrance.
    bool flag1 = false;
    bool flag2 = false;
    cout << "Please enter the name of the room you want to add to the maze :" << endl;
    getline(cin,room1);
    while(roomOk == false)
    {
        flag1 = false;
        flag2 =false;
        getline(cin,room1);
        for(int i = 0; i < vertices.size(); i++)
        {
            if(vertices[i].name == room1)
            {
                cout << "This room already exists! Please try again." << endl;
                flag1 = true;
            }
            else if(room1 == "exit" or room1 == "Exit" or room1 == "entrance" or room1 == "Entrance")
            {
                cout << "You cannot add another exit or entrance to your maze. Please try again." << endl;
                flag2 = true;
            }
        }
        if(flag1 == false and flag2 == false)
        {
            roomOk = true;
        }
    }
    //Now asks user to input a room to connect to room1.
    string doneConnecting = "y";
    //While the user is not done adding connections.
    while(doneConnecting == "y")
    {
        cout << "Please enter the name of a room you want to connect it to :" << endl;
        roomOk = false;
        flag1 = false;
        flag2 = false;
        while(roomOk == false)
        {
            flag1 = false;
            flag2 = false;
            getline(cin,room2);
            for(int i = 0; i < vertices.size(); i++)
            {
                if(room1 == room2)
                {
                    flag1 = true;
                }
                else if(vertices[i].name == room2)
                {
                    flag2 = true;
                }
            }
            if(flag1 == true)
            {
                cout << "You cannot connect the room to itself. Please try again" << endl;
            }
            else if(flag1 == false and flag2 == true)
            {
                roomOk = true;
            }
            else
            {
                cout << "Room not found. Please try again" << endl;
            }
        }
        cout << "Enter the chance of falling into a trap between these two rooms (0-50) : " << endl;
        cin >> risk;
        while(risk > 50 or risk < 0)
        {
            cout << "You entered an invalid value (Range is 0 to 50). Please try again." << endl;
            cin >> risk;
        }
        //Sends the room1 and 2 strings to appropriate functions to add to the maze.
        addVertex(room1);
        int newID = vertices.size()+1;
        for(int i = 0; i < vertices.size(); i++)
        {
            if(vertices[i].name == room1)
            {
                vertices[i].ID = newID;
            }
        }
        addEdge(room1,room2,risk,1);
        cout << "Add another connection? (y or n)" << endl;
        getline(cin,doneConnecting);
        getline(cin,doneConnecting);
    }

}

void maze::printRooms()
{
    cout << "- Room - " << " - Connected Room - " << " - Chance of falling into trap - " << endl;
    for(int i = 0; i < vertices.size(); i++)
        {
            for(int j = 0; j < vertices[i].adj.size(); j++)
            {
                cout << vertices[i].name << " -> " << vertices[i].adj[j].v->name << " : " << vertices[i].adj[j].risk << "%" << endl;
            }
        }
}

vector<vertex> maze::getVertices()
{
    return vertices;
}
