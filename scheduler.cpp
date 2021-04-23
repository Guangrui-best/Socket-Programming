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

using namespace std;

class Hospitals{
public:
    int location;
    int capacity;
    int occupancy;
    float distance = 0;
    float score = -2.0;
    struct sockaddr_in addr;
};
void recv_from_hosp(int sock_udp);

Hospitals hospitalA;
Hospitals hospitalB;
Hospitals hospitalC;


int main(){

    int sock_udp = socket(PF_INET, SOCK_DGRAM, 0);  
    if (sock_udp < 0){
        fprintf(stderr, "Failed to create UDP scheduler socket!");
        exit(1);
    }
    struct sockaddr_in scheduler_addr_udp;
    memset(&scheduler_addr_udp, 0, sizeof(scheduler_addr_udp));
    scheduler_addr_udp.sin_family = AF_INET;
    scheduler_addr_udp.sin_port = htons(33073);
    scheduler_addr_udp.sin_addr.s_addr = inet_addr("127.0.0.1");
    if(bind(sock_udp, (struct sockaddr*)& scheduler_addr_udp, sizeof(scheduler_addr_udp)) < 0){
        fprintf(stderr, "Binding UDP socket failed!");
        exit(1);
    }
    cout << "The Scheduler is up and running." << endl;
    recv_from_hosp(sock_udp); //receive information from hospitals

    //create TCP socket
    int sock_tcp;
    sock_tcp = socket(PF_INET, SOCK_STREAM, 0);
    if(sock_tcp < 0){
        fprintf(stderr, "Failed to create TCP scheduler socket!");
        exit(1);
    }
    struct sockaddr_in scheduler_addr_tcp;
    memset(&scheduler_addr_tcp, 0, sizeof(scheduler_addr_tcp));
    scheduler_addr_tcp.sin_family = AF_INET;
    scheduler_addr_tcp.sin_port = htons(34073);
    scheduler_addr_tcp.sin_addr.s_addr = inet_addr("127.0.0.1");
    if(bind(sock_tcp, (struct sockaddr*)& scheduler_addr_tcp, sizeof(scheduler_addr_tcp)) < 0){
        fprintf(stderr, "Binding TCP socket failed!");
        exit(1);
    }
    
    //listen client
    if(listen(sock_tcp, SOMAXCONN) < 0){
        fprintf(stderr, "Listening failed!");
        exit(1);
    }
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    while (1){
        int child_sock = accept(sock_tcp, (struct sockaddr*)&client_addr, &client_addr_len);
        if(child_sock < 0){
            fprintf(stderr, "Connecting failed!");
            exit(1);
        }
        pid_t pid = fork();
        if(pid == -1) {
            fprintf(stderr, "Child process failed!");
        }
        else if (pid == 0){
            close(sock_tcp);   //child doesn't need listener
            char cli_buff[256];
            memset(&cli_buff, 0, sizeof(cli_buff));
            if(recv(child_sock, cli_buff, sizeof(cli_buff), 0) < 0){
                fprintf(stderr, "Receiving failed!");
                exit(1);
            }
            cout << "The Scheduler has received client at location " << cli_buff 
                 << " from the client using TCP over port 34073"<< endl;
        
            // send the client location to hospitals
            char hosp_buff[256];
            memset(&hosp_buff, 0, sizeof(hosp_buff));
            if (hospitalA.capacity > hospitalA.occupancy){
                socklen_t hospA_len = sizeof(hospitalA.addr); 
                sendto(sock_udp, cli_buff, sizeof(cli_buff), 0, (struct sockaddr*)&hospitalA.addr, sizeof(hospitalA.addr));
                cout << "The Scheduler has sent client location to Hospital A using UDP over port 33073." << endl;
                recvfrom(sock_udp, hosp_buff, sizeof(hosp_buff), 0, (struct sockaddr*)&hospitalA.addr, &hospA_len);
                char* message;
                message = strtok(hosp_buff, " ");
                hospitalA.score = atof(message);
                message = strtok(NULL, " ");
                hospitalA.distance = atof(message);
                if (hospitalA.distance > 0){
                    cout << "The Scheduler has received map information from Hospital A, the score = "
                        << hospitalA.score << " and the distance = " << hospitalA.distance << endl;
                }else{
                    cout << "The Scheduler has received map information from Hospital A, the score = None"
                        << " and the distance = None" << endl;
                }
            }
            memset(&hosp_buff, 0, sizeof(hosp_buff));
            if (hospitalB.capacity > hospitalB.occupancy){
                socklen_t hospB_len = sizeof(hospitalB.addr); 
                sendto(sock_udp, cli_buff, sizeof(cli_buff), 0, (struct sockaddr*)&hospitalB.addr, sizeof(hospitalB.addr));
                cout << "The Scheduler has sent client location to Hospital B using UDP over port 33073." << endl;
                recvfrom(sock_udp, hosp_buff, sizeof(hosp_buff), 0, (struct sockaddr*)&hospitalB.addr, &hospB_len);
                char* message;
                message = strtok(hosp_buff, " ");
                hospitalB.score = atof(message);
                message = strtok(NULL, " ");
                hospitalB.distance = atof(message);
                if (hospitalB.distance > 0){
                    cout << "The Scheduler has received map information from Hospital B, the score = "
                        << hospitalB.score << " and the distance = " << hospitalB.distance << endl;
                }else{
                    cout << "The Scheduler has received map information from Hospital B, the score = None"
                        << " and the distance = None" << endl;
                }
            }
            memset(&hosp_buff, 0, sizeof(hosp_buff));
            if (hospitalC.capacity > hospitalC.occupancy){
                socklen_t hospC_len = sizeof(hospitalC.addr); 
                sendto(sock_udp, cli_buff, sizeof(cli_buff), 0, (struct sockaddr*)&hospitalC.addr, sizeof(hospitalC.addr));
                cout << "The Scheduler has sent client location to Hospital C using UDP over port 33073." << endl;
                recvfrom(sock_udp, hosp_buff, sizeof(hosp_buff), 0, (struct sockaddr*)&hospitalC.addr, &hospC_len);
                char* message;
                message = strtok(hosp_buff, " ");
                hospitalC.score = atof(message);
                message = strtok(NULL, " ");
                hospitalC.distance = atof(message);
                if (hospitalC.distance > 0){
                    cout << "The Scheduler has received map information from Hospital C, the score = "
                        << hospitalC.score << " and the distance = " << hospitalC.distance << endl;
                }else{
                    cout << "The Scheduler has received map information from Hospital C, the score = None"
                        << " and the distance = None" << endl;
                }
            }

            string assigned = "None";  // initialization
            if(hospitalA.distance >= 0 && hospitalB.distance >= 0 && hospitalC.distance >= 0){
                float max_score = -1;
                float min_distance = FLT_MAX;
                if(hospitalA.score > max_score) {
                    max_score = hospitalA.score;
                    min_distance = hospitalA.distance;
                    assigned = "A";
                }
                if(hospitalB.score > max_score) {
                    max_score = hospitalB.score;
                    assigned = "B";
                }
                else if(hospitalB.score == max_score && hospitalB.distance < min_distance){
                    min_distance = hospitalB.distance;
                    assigned = "B";
                }
                if(hospitalC.score > max_score) {
                    max_score = hospitalC.score;
                    assigned = "C";
                }
                else if(hospitalC.score == max_score && hospitalC.distance < min_distance){
                    min_distance = hospitalC.distance;
                    assigned = "C";
                }
            }
            else if(hospitalA.distance == -2 || hospitalB.distance == -2 || hospitalC.distance == -2)
                assigned = "Not Found"; // not find the client location in the map
            else
                assigned = "None"; // client is in the one of the hospitals or they are all unavailable
        
            memset(&cli_buff, 0, sizeof(cli_buff));
            memset(&hosp_buff, 0, sizeof(hosp_buff));

            strcpy(cli_buff, assigned.c_str());
            strcpy(hosp_buff, assigned.c_str());
            send(child_sock, cli_buff, sizeof(cli_buff), 0);
            cout << "The Scheduler has sent the result to client using TCP over port 34074." << endl;
        
            if(assigned == "A") {
                hospitalA.occupancy++;
                sendto(sock_udp, hosp_buff, sizeof(hosp_buff), 0, (struct sockaddr*)&hospitalA.addr, sizeof(hospitalA.addr));
                cout << "The Scheduler has sent the result to Hospital " << assigned << " using UDP over port 33073" << endl;
            }
            else if(assigned == "B"){
                hospitalB.occupancy++;
                sendto(sock_udp, hosp_buff, sizeof(hosp_buff), 0, (struct sockaddr*)&hospitalB.addr, sizeof(hospitalB.addr));
                cout << "The Scheduler has sent the result to Hospital " << assigned << " using UDP over port 33073" << endl;
            }
            else if(assigned == "C"){
                hospitalC.occupancy++;
                sendto(sock_udp, hosp_buff, sizeof(hosp_buff), 0, (struct sockaddr*)&hospitalC.addr, sizeof(hospitalC.addr));
                cout << "The Scheduler has sent the result to Hospital " << assigned << " using UDP over port 33073" << endl;
            }
            
            close(child_sock);
            close(sock_tcp);
            close(sock_udp);
            exit(0);
        }
        else close(child_sock);
    }
    return 0;
}

void recv_from_hosp(int sock_udp){
    int counter = 0;
    while (1){
        struct sockaddr_in hospital_addr;
        socklen_t hospital_addr_len = sizeof(hospital_addr);
        char buffer[256];
        if(recvfrom(sock_udp, buffer, sizeof(buffer), 0, (struct sockaddr*)&hospital_addr, &hospital_addr_len) < 0){
            continue;
        }
        char* message = strtok(buffer, " ");
        int capacity = atoi(message);
        message = strtok(NULL, " ");
        int occupancy = atoi(message);
        if (ntohs(hospital_addr.sin_port) == 30073){
            cout << "The Scheduler has received information from Hospital A: total capacity is "
                 << capacity << " and initial occupancy is " << occupancy << endl;
            counter++;
            hospitalA.capacity = capacity;
            hospitalA.occupancy = occupancy;
            hospitalA.addr = hospital_addr;
        }
        else if(ntohs(hospital_addr.sin_port) == 31073){
            cout << "The Scheduler has received information from Hospital B: total capacity is "
                 << capacity << " and initial occupancy is " << occupancy << endl;
            counter++;
            hospitalB.capacity = capacity;
            hospitalB.occupancy = occupancy;
            hospitalB.addr = hospital_addr;
        }
        else if(ntohs(hospital_addr.sin_port) == 32073){
            cout << "The Scheduler has received information from Hospital C: total capacity is "
                 << capacity << " and initial occupancy is " << occupancy << endl;
            counter++;
            hospitalC.capacity = capacity;
            hospitalC.occupancy = occupancy;
            hospitalC.addr = hospital_addr;
        }
        if (counter == 3) return;
    }
}