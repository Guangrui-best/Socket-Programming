# EE450 Socket Programming Project

## Personal Information

-Name: Guangrui Shen

-USCID: 2801221073

## What I have done in the project?

In this project, I build up a communication system between scheduler, hospitals A, B, C and client to generate customized resource allocation based on user quries. In details, hospitals will construct a graph given map.txt and get the shortest path between themselves and the client location. The scheduler will assign the best hospital through calculated score and distance to the client and meanwhile update the assigned hospital's information.

## Code files details

### scheduler.cpp
- Build up TCP connection with client and UDP connection with hospitals A, B and C.
- Receive client query and sent it to hospitals.
- Receive hospitals score and distance to determine which one should be assigned.
- Send the result to the client and update the assigned hospital information.

### hospitalA.cpp, hospitalB.cpp, hospitalC.cpp

- Reindex the map location and construct a graph given the map.txt file.
- Connect with the scheduler through UDP.
- Obtain the shortest distance between self location and received client location using Dijkstra algorithm.
- Calculate score through capacity, occupancy and shortest distance.
- Send distance and score to the scheduler.
- Receive the result and update hospital information if it is assigned.

### client.cpp

- Connect with scheduler through TCP.
- Send query (self location) to the scheduler server.
- Received the assigned hospital id as the result.

## The format of all the messages exchanged

- For scheduler
    Receive capacity and occupancy (string) from hospitals.
    Receive client location (string) from client and sent it (string) to hospitals.
    Receive score (string) and distance (string) from hospitals.
    Send the assigned hospital id (string) to client.

- For hospitals A, B, C
    Get hospital location, capacity, occupancy (string) from the command line.
    Send capacity and occupancy (string) to scheduler.
    Receive client location (string) from scheduler.
    Send score and distance (string) to scheduler.

- For client
    Get client location (string) from the command line.
    Send the location (string) to scheduler.
    Receive the assigned hospital id (string) from scheduler.

## Idiosyncrasy

The project does not fail in any conditions.

## Reused Code

I reused sample code about building up UDP and TCP connections from "Beej's Guide to Network Programming".