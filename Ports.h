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

vector<int> getPorts(char *response){
    vector<int> ports;
    vector<string> tokens;
    string resp(response);

    boost::split(tokens, resp, boost::is_any_of(" "));

    for(int i=0;i<tokens.size()-2;i++){
        ports.push_back(atoi(tokens[i].c_str()));
    }

    return ports;
}
vector<string> getSHAFrom(char *response){
    vector<string> tokens;
    string resp(response);
    vector<string> shas;
    boost::split(tokens, resp, boost::is_any_of(" "));
    int token_size = tokens.size();

    for(int i=token_size-2;i<token_size;i++){
        shas.push_back(tokens[i]);
    }
    
    return shas;
} 
