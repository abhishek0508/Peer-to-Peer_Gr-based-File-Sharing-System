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

vector<string> getSHA(string fpath){

    vector<string> result;
    FILE *fp = fopen(fpath.c_str(),"r+");
    if(fp==NULL){
        exit(1);
    }

    fseek(fp,0, SEEK_END);
    int file_size = ftell(fp);
    rewind(fp);

    unsigned char buffer[BUFFER_SIZE];
    unsigned char hash1[20];
    char partial[40];
    string total_chunk_string="";
    int n;
    while((n=fread(buffer,1,sizeof(buffer),fp))>0 && file_size>0){
        file_size = file_size-n;
        SHA1(buffer, n, hash1);
        bzero(buffer, BUFFER_SIZE); 
        for(int i=0;i<5;i++){
            sprintf(partial+2*i,"%02x",hash1[i]);
        }

        total_chunk_string += partial;
    }
    result.push_back(total_chunk_string);

    //whole file sha1
    unsigned char final_hash[20];
    char shortHash[40];
    SHA1((unsigned char *)total_chunk_string.c_str(), total_chunk_string.size(), final_hash);
    for(int i=0;i<20;i++){
        sprintf(shortHash+i*2,"%02x",final_hash[i]);
    }
    result.push_back(string(shortHash));
    return result;
}