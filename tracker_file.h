#include<iostream>
#include<pthread.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h> 
#include<string.h>
#include<sys/types.h> 
#include<sys/socket.h> 
#include<netinet/in.h> 
#include<arpa/inet.h> 
#include<sys/fcntl.h>
#include<netdb.h>
#include<vector>
#include<boost/algorithm/string.hpp>
#include<map>
#include <openssl/sha.h>
using namespace std;
#define BUFFER_SIZE 512*1024

vector<string> getTrackerPort(string tracker_info){

    FILE *fp = fopen(tracker_info.c_str(), "r+");
    if(fp == NULL){
        exit(1);
    }

    vector<string> portno;
    char c[1000];
    fscanf(fp, "%[^\n]", c);
    string tracker(c);
    boost::split(portno, tracker, boost::is_any_of(" "));
    fclose(fp);
    return portno;
}
vector<string> getTrackerIP(string tracker_info){
    FILE *fp = fopen(tracker_info.c_str(), "r+");
    if(fp == NULL){
        exit(1);
    }

    vector<string> IP;
    char c[1000];
    char c1[1000];

    fscanf(fp, "%[^\n]", c);
    fscanf(fp, "%c", c);
    fscanf(fp, "%[^\n]", c1);

    string tracker(c1);
    boost::split(IP, tracker, boost::is_any_of(" "));
    fclose(fp);
    return IP;
}