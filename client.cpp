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
using namespace std;

int main(int argc, char* argv[]){
    if(argc != 2){
        fprintf(stderr, "Input wrong!");
        exit(1);
    }

    int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);   // get TCP client socket descriptor
    if (sock < 0) {
        fprintf(stderr, "Failed to create TCP client socket");
        exit(1);
    }

    struct sockaddr_in scheduler_addr;
    memset(&scheduler_addr, 0, sizeof(scheduler_addr));
    scheduler_addr.sin_family = AF_INET;
    scheduler_addr.sin_port = htons(34073);
    scheduler_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int conn;
    if ((conn = connect(sock, (struct sockaddr*)&scheduler_addr, sizeof(scheduler_addr))) < 0){
        fprintf(stderr, "Connecting failed!");
        exit(1);
    }
    cout << "The client is up and running" << endl;

    string message = argv[1];
    char cli_buff[256];
    memset(cli_buff, 0, sizeof(cli_buff));
    strcpy(cli_buff, message.c_str());
    if(send(sock, cli_buff, sizeof(cli_buff), 0) < 0){
        fprintf(stderr, "Sending failed!");
        exit(1);
    }
    cout << "The client has sent query to Scheduler using TCP: client location " << message << endl;
    
    char buffer[256];
    if(recv(sock, buffer, sizeof(buffer), 0) < 0){
        fprintf(stderr, "Receiving failed!");
        exit(1);
    }

    if(strcmp(buffer, "None") == 0){
        cout << "Score = None, No assignment" << endl;
    }
    else if(strcmp(buffer, "Not Found") == 0){
        cout << "Location " << message << " not found" << endl;
    }
    else{
        cout << "The client has received results from the Scheduler: assigned to Hospital "
             << buffer << endl;
    }
    close(sock);

    return 0;
}