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


vector<string> request_type_command(string request){
    vector<string> result;
    boost::split(result, request, boost::is_any_of(" "));
    return result;  
}
string getFilePath(string request){
    vector<string> result;
    boost::split(result, request, boost::is_any_of(" "));
    if(result[0]=="upload_file")
        return result[1];
    else
        return NULL;  
}