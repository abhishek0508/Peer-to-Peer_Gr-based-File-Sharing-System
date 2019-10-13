#include"utils.h"

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

void *RequestThread(void *newsockfd1){
    
    // this char c is for receving file name from client
    char buff[BUFFER_SIZE], c[BUFFER_SIZE];
    int n;
    int newsockfd = *((int*)newsockfd1);
    
    int val = read(newsockfd, c, sizeof(c));
    
    // by opening the file name received from client we are checking if file is present
    // in server or not

    string fi=string(c);
    FILE *fp = fopen(fi.c_str(), "r+");

    if(fp == NULL){
        perror("file does not exist");
        pthread_exit(NULL);
    }

    printf("File opened, now sending file\n");
    
    // moving pointer of file to calculate file size
    fseek(fp, 0, SEEK_END);
    long long size = ftell(fp);
    rewind(fp);

    // sending file size to client
    send(newsockfd, &size, sizeof(size), 0);

    // sending file to client by reading in chunks
    while( (n = fread(buff, sizeof(char), BUFFER_SIZE, fp))> 0){
        printf("%d\n",n);
        send(newsockfd, buff, n, 0);
        memset(buff, '\0', BUFFER_SIZE);
    }
    fclose(fp);
    close(newsockfd);
}
bool isUpload(string request){
    vector<string> result;
    boost::split(result, request, boost::is_any_of(" "));
    if(result[0]=="upload_file")
        return true;
    else
        return false;  
}
string getFilePath(string request){
    vector<string> result;
    boost::split(result, request, boost::is_any_of(" "));
    if(result[0]=="upload_file")
        return result[1];
    else
        return NULL;  
}
string getSHA(string fpath){

}

void *ThreadServerProgram(void *arg){

    // parsing of argument containing the ip & port
    char *arr = (char*)arg;
    string argument_input(arr);
    vector<string> arguments;
    boost::split(arguments, argument_input, boost::is_any_of(" "));

    // variables used in program
    int sockfd, new_sockfd, portno, n;
    char buff[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    portno = atoi(arguments[1].c_str());
    bzero((char *) &serv_addr, sizeof(serv_addr));

    // socket creation at server side
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd<0){
        printf("socket opening failure");
        exit(1);
    }
    
    // assigning ip & port in structure
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(arguments[0].c_str());

    // socket binding
    if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){
        perror("Binding failed, incorrect ip or port");
    }

    // socket listening
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    pthread_t thread1;

    while(1){

        // socket accepts, once accepts create new socket for read & write
        printf("waiting for connections\n");
        int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if(newsockfd<0){
            perror("error on accept\n");
        }
        printf("connected....\n");

        // argument parsing of newsockfd
        int *arg = (int*)malloc(sizeof(*arg));
        *arg = newsockfd;

        // create new thread whenever a request for new connection comes in.
        pthread_create(&thread1, NULL, RequestThread, (void*)arg);

    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) 
{ 

    if(argc < 4){
        printf("wrong no of input\n");
        exit(1);
    }

    //argv[1] ip address in which its own server will run
    // argv[2] port address in which its own server will run
    // argv[3] it will tracker_info.txt which will contains tracker's details
    vector<string> ports, IP;
    ports  = getTrackerPort(argv[3]);
    IP = getTrackerIP(argv[3]);

    pthread_t thread3;

    string s = "";
    s+=argv[1];
    s+=" ";
    s+=argv[2];

    // create thread for running server in this port
    pthread_create(&thread3, NULL, ThreadServerProgram, (void*)s.c_str());


    // Client Program for Peer starts here
    // it will connect to tracker
    //variables used in program
    string fname;
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buff[BUFFER_SIZE];
    FILE *fp;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd<0){
        perror("error opening socket\n");
    }

    server = gethostbyname(IP[0].c_str());

    if(server == NULL){
        fprintf(stderr, "Error, no such host\n");
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    portno = atoi(ports[0].c_str());

    // assign serverip & port into the structure
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(IP[0].c_str());
    serv_addr.sin_port = htons(portno);


    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){
        perror("connection failed to server\n");
    }

    // client request_type or command is asked from user 
    while(true){

        // this is for connecting to tracker
        char request[BUFFER_SIZE]; 
        char response[BUFFER_SIZE];
        printf("Enter commands\n");
        getline(cin,fname);
        for(int i=0;i<fname.length();i++){
            request[i] = fname.at(i);
        }
        if(isUpload(request)){
            string filePath = getFilePath(request);
            string SHA1 = getSHA(filePath);
            
        }
        send(sockfd, request, sizeof(request), 0);
        bzero(request, BUFFER_SIZE);
        bzero(response, BUFFER_SIZE);
        read(sockfd, response, sizeof(response));
        printf("%s\n", response);

        // this is for connecting to peer
        // char request[BUFFER_SIZE];
        // printf("enter file name");
        // getline(cin, fname);
        // send(sockfd, fname.c_str(), sizeof(fname), 0);

        
        // fp = fopen("new_file.mkv", "w+");
        // if(fp == NULL){
        //     perror("Not able to create file on client side");
        // }

        // long long file_size;
        // recv(sockfd, &file_size, sizeof(file_size), 0);
        // cout<<"file_size:"<<file_size<<endl;
        // fflush(fp);
        // while((n = recv(sockfd, buff, BUFFER_SIZE, 0))>0){
        //     fwrite(buff, sizeof(char), n, fp);
        //     fflush(fp);
        //     memset(buff, '\0', BUFFER_SIZE);
        // }
        // fclose(fp);
    }

    close(sockfd); 
    return 0; 
} 