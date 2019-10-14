#include"utils.h"

using namespace std;

#define BUFFER_SIZE 512*1024
map<string, string> user_info;// contains user_name & password
map<string, vector<string> > group_user; // contains all group info & all files in it
map<string, string> group_owner; // user associated with which group
map<string, string> gr_file; // group containg file_name
map<string, string> file_ip_port; //file_name & ip & port
pthread_mutex_t mutex1  = PTHREAD_MUTEX_INITIALIZER; 

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
string getFileName(string fname){
    vector<string> fcomponent;
    boost::split(fcomponent, fname, boost::is_any_of("/"));
    int len = fcomponent.size();
    return fcomponent[len-1];
}

// function to handle all type request from peers
string request_handler(char *request_type, int &login_flag, string &curr_user){
    string input(request_type);

    cout<<"type: "<<request_type<<endl;
    vector<string> command;
    boost::split(command, input, boost::is_any_of(" "));

    // create_user
    if(strcmp(command[0].c_str(), "create_user")==0){

        pthread_mutex_lock(&mutex1);
        user_info[command[1]] = command[2];
        pthread_mutex_unlock(&mutex1);

        return "user_created";
    }
    //login user
    else if(strcmp(command[0].c_str(), "login") == 0){
        if(user_info.find(command[1]) == user_info.end()){
            return "user not registered";
        }
        else{
            if(user_info[command[1]] == command[2]){

                pthread_mutex_lock(&mutex1);
                curr_user = command[1];
                login_flag = 1;
                pthread_mutex_unlock(&mutex1);

                return "login done! now you can operate";
            }
            return "wrong password";
        }
    }
    else {
        if(login_flag==1){
            if(strcmp(command[0].c_str(),"create_group")==0){

                pthread_mutex_lock(&mutex1);
                group_user[command[1]].push_back(curr_user);
                group_owner[command[1]] = curr_user;
                pthread_mutex_unlock(&mutex1);
            }
            else if(strcmp(command[0].c_str(), "join_group")==0){

                pthread_mutex_lock(&mutex1);
                group_user[command[1]].push_back(curr_user);
                pthread_mutex_unlock(&mutex1);
            
            }
            else if(strcmp(command[0].c_str(), "leave_group")==0){

                pthread_mutex_lock(&mutex1);

                map<string, vector<string>> new_group_user;
                for(map<string, vector<string> >::iterator it=group_user.begin(); it!=group_user.end();it++){
                    if(it->first == command[1]){
                        for(vector<string>::iterator sec=it->second.begin(); sec!=it->second.end();sec++){
                            if(*sec == curr_user){
                                continue;
                            }else{

                                new_group_user[it->first].push_back(*sec);
                                
                            }
                        }
                    }                    
                }
                group_user.clear();
                group_user.insert(new_group_user.begin(),new_group_user.end());
                new_group_user.clear();

                pthread_mutex_unlock(&mutex1);
            }
            else if(strcmp(command[0].c_str(), "list_groups")==0){
                string str = "";
                for(map<string, vector<string> >::iterator it=group_user.begin(); it!=group_user.end();it++){
                    str+=it->first;
                    str+=" ";                    
                }
                return str;
            }
            else if(strcmp(command[0].c_str(), "upload_file")==0){
                
                string fname;
                fname = getFileName(command[1]);
                return fname;
            }
            else{
                return "false";
            }
        }
        else{
            return "You're not logged in";
        }
    }
}

void *RequestThread(void *newsockfd1){
    
    // information that will be maintained for this particular thread
    int login_flag = 0;
    string curr_user;
    // request_type from client
    char request_type[BUFFER_SIZE];
    int newsockfd = *((int*)newsockfd1);

    while(true){
        memset(request_type, '\0', sizeof(request_type));
        read(newsockfd, request_type, sizeof(request_type));
        cout<<"read only once"<<endl;
        // function to handle type of request from peers in tracker
        string response = request_handler(request_type, login_flag, curr_user);

        write(newsockfd, response.c_str(), sizeof(response));
    }
}

int main(int argc, char *argv[])
{ 
    if(argc < 2){
        printf("tracker info not provided");
        exit(1);
    }

    vector<string> ports, IP;
    ports  = getTrackerPort(argv[1]);
    IP = getTrackerIP(argv[1]);

    int sockfd, new_sockfd, portno, n;
    char buff[BUFFER_SIZE];

    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    // socket creation at server side
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd<0){
        printf("socket opening failure");
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(ports[0].c_str());

    serv_addr.sin_port = htons(portno);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(IP[0].c_str());

    // socket binding
    if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){
        perror("Binding failed, incorrect port or ip");
    }

    // socket listening
    listen(sockfd, 10);
    clilen = sizeof(cli_addr);
    
    while(1){

        pthread_t thread1;
        // socket accepts, once accepts create new socket for read & write
        printf("waiting for connections\n");
        int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if(newsockfd<0){
            perror("error on accept");
        }
        printf("connected....\n");

        int *arg = (int*)malloc(sizeof(*arg));
        *arg = newsockfd;

        // create new thread whenever a request for new connection comes in.
        pthread_create(&thread1, NULL, RequestThread, (void*)arg);
        // pthread_join(thread1, NULL);
    }

    return 0; 
} 