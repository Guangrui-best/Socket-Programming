#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <float.h>
#include <map>
#include <fstream>
#include <sstream>
#include <set>
#include <algorithm>

using namespace std;

class Hospitals{
public:
    int location;
    int capacity;
    int occupancy;
    float distance;
    float score;
    struct sockaddr_in addr;
};
void construct_reindex_map();
void construct_graph();
void Dijkstra_shortest_path(int reidx_own_loc);

map<int, map<int, float>> graph;
map<int, int> idx_map;
map<int, float> shortest_path;
Hospitals hospital;

int main(int argc, char *argv[]){
    if(argc != 4){
        fprintf(stderr, "Wrong input!");
        exit(1);
    }

    hospital.location = atoi(argv[1]);
    hospital.capacity = atoi(argv[2]);
    hospital.occupancy = atoi(argv[3]);

    construct_reindex_map();
    construct_graph();
    
    // calculate shortest path
    float reidx_own_loc = idx_map[hospital.location];
    Dijkstra_shortest_path(reidx_own_loc);
    
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock < 0){
        fprintf(stderr, "Failed to create UDP socket!");
        exit(1);
    }
    struct sockaddr_in hosp_addr;
    memset(&hosp_addr, 0, sizeof(hosp_addr));
    hosp_addr.sin_family = AF_INET;
    hosp_addr.sin_port = htons(30073);
    hosp_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(bind(sock, (struct sockaddr*)& hosp_addr, sizeof(hosp_addr)) < 0){
        fprintf(stderr, "Binding failed!");
        exit(1);
    }
    cout << "Hospital A is up and running using UDP on port 30073." << endl;

    struct sockaddr_in scheduler_addr;
    memset(&scheduler_addr, 0, sizeof(scheduler_addr));
    scheduler_addr.sin_family = AF_INET;
    scheduler_addr.sin_port = htons(33073);
    scheduler_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    char buffer[256];
    memset(&buffer, 0, sizeof(buffer));
    stringstream input;
    input << hospital.capacity << " " << hospital.occupancy;
    string tmp = input.str();
    strcpy(buffer, tmp.c_str());
    sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&scheduler_addr, sizeof(scheduler_addr));
    cout << "Hospital A has total capacity " << hospital.capacity << " and initial occupancy " << hospital.occupancy << endl;

    char recvbuff[256];
    memset(&recvbuff, 0, sizeof(recvbuff));
    socklen_t scheduler_addr_len = sizeof(scheduler_addr);
    while (1){
        if (recvfrom(sock, recvbuff, sizeof(recvbuff), 0, (struct sockaddr*)&scheduler_addr, &scheduler_addr_len) < 0){
            continue;
        }
        if (strcmp(recvbuff, "A") == 0){
            hospital.occupancy++;
            float availablity = (float)(hospital.capacity - hospital.occupancy) / (float)hospital.capacity;
            cout << "Hospital A has been assigned to a client, occupation is updated to "
                 << hospital.occupancy << ", availabilty is updated to " << availablity << endl;
            continue;
        }
        int location = atoi(recvbuff);
        cout << "Hospital A has received input from client at location " << location << endl;
        
        memset(&buffer, 0, sizeof(buffer));

        if (idx_map.find(location) == idx_map.end()){
            // if the given location does not find in the map
            cout << "Hospital A does not have the location " << location << " in map" << endl;
            cout << "Hospital A has sent \"location not found\" to the Scheduler" << endl;
            strcpy(buffer, "-2 -2");
            sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&scheduler_addr, scheduler_addr_len);
        }
        else if(location == hospital.location){
            //the given location is hospital A
            strcpy(buffer, "-1 -1");
            sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&scheduler_addr, scheduler_addr_len);
        }
        else{
            int reidx_loc;
            reidx_loc = idx_map[location];
            float availabilty = (float)(hospital.capacity - hospital.occupancy) / (float)hospital.capacity;
            float distance = shortest_path[reidx_loc];
            float score = 1.0 / (distance * (1.1 - availabilty));
            cout << "Hospital A has capacity " << hospital.capacity << ", occupation = " 
                 << hospital.occupancy << ", availbility = " << availabilty << endl;
            cout << "Hospital A has found the shortest path to client, distance = " << distance << endl;
            cout << "Hospital A has the score = " << score << endl;
            
            stringstream output;
            output << score << " " << distance;
            string output_tmp = output.str();
            strcpy(buffer, output_tmp.c_str());
            sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&scheduler_addr, scheduler_addr_len);
            cout << "Hospital A has sent score = " << score << " and distance = " << distance
                 << " to the Scheduler" << endl;
        }
    }
    close(sock);
    
    return 0;
}
void construct_reindex_map(){
    ifstream infile;
    infile.open("map.txt", ios::in);
    if(!infile.is_open()){
        fprintf(stderr, "Error opening file!");
        exit(1);
    }
    string line;
    int counter = 0;
    while(getline(infile, line)){
        stringstream ss;
        ss.str(line);
        int row, col;
        float distance;
        ss >> row >> col >> distance;
        if(idx_map.find(row) == idx_map.end()){
            idx_map[row] = counter;
            counter++;
        }
        if(idx_map.find(col) == idx_map.end()){
            idx_map[col] = counter;
            counter++;
        }
    }
    infile.close();
}
void construct_graph(){
    ifstream infile;
    infile.open("map.txt", ios::in);
    if(!infile.is_open()){
        fprintf(stderr, "Error opening file!");
        exit(1);
    }
    string line;
    while(getline(infile, line)){
        stringstream ss;
        ss.str(line);
        int row, col;
        float distance;
        ss >> row >> col >> distance;
        graph[idx_map[row]][idx_map[col]] = distance;
        graph[idx_map[col]][idx_map[row]] = distance;
    }
    infile.close();
}

void Dijkstra_shortest_path(int loc){
    set<int> IsVisited = {loc};
    // initialize;
    for (int i = 0; i < graph.size(); i++){
        if(graph[loc][i] == 0){
            if (loc == i) 
                continue;
            shortest_path[i] = -1;
            continue;
        }
        shortest_path[i] = graph[loc][i];
    }
    while(1){
        float pre_min = -1;
        int visited = -1;
        for(int i = 0; i < graph.size(); i++){
            if(IsVisited.find(i) == IsVisited.end() && shortest_path[i] != -1){
                if(pre_min == -1 || pre_min > shortest_path[i]){
                    pre_min = shortest_path[i];
                    visited = i;
                }
            }
        }
        if (visited == -1) break;
        IsVisited.insert(visited);
        for(int i = 0; i < graph.size(); i++){
            if(IsVisited.find(i) == IsVisited.end() && graph[visited][i] != 0){
                if (shortest_path[i] == -1)
                    shortest_path[i] = pre_min + graph[visited][i];
                else
                    shortest_path[i] = min(shortest_path[i], pre_min + graph[visited][i]);
            }
        }
    }
    
}