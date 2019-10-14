#include"utils.h"
#include"tracker_file.h"

using namespace std;

#define BUFFER_SIZE 512*1024
map<string, string> user_info;// contains user_name & password
map<string, vector<string> > group_user; // contains all group info & all users in it
map<string, string> group_owner; // user associated with which group
map<string, vector<string>> gr_file; // groupid-key containg files(file_name)
map<string, vector<string> > file_port; //file_name is key ip & port ehich contains these files.
map<string, vector<string> > user_group; 
map<string, string> file_full_sha;
map<string, string> file_partial_sha;
pthread_mutex_t mutex1  = PTHREAD_MUTEX_INITIALIZER; 


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
    cout<<"request_length--"<<sizeof(request_type)<<endl;
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
                return "group_created_with current user as owner";
            }
            else if(strcmp(command[0].c_str(), "join_group")==0){

                pthread_mutex_lock(&mutex1);
                group_user[command[1]].push_back(curr_user);
                user_group[curr_user].push_back(command[1]);
                pthread_mutex_unlock(&mutex1);
                return "Congrats! you have joined a group";
            
            }
            else if(strcmp(command[0].c_str(), "leave_group")==0){

                pthread_mutex_lock(&mutex1);

                map<string, vector<string>> new_group_user;
                for(map<string, vector<string> >::iterator it=group_user.begin(); it!=group_user.end();it++){
                    if(it->first == command[1]){
                        for(vector<string>::iterator sec=it->second.begin(); sec!=it->second.end();sec++){
                            if(*sec == curr_user)
                                continue;
                            else
                                new_group_user[it->first].push_back(*sec);   
                        }
                    }                    
                }
                group_user.clear();
                group_user.insert(new_group_user.begin(),new_group_user.end());
                new_group_user.clear();

                pthread_mutex_unlock(&mutex1);
                
                return "your entry from group ommited";
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

                pthread_mutex_lock(&mutex1);
                    gr_file[command[2]].push_back(fname);
                    file_port[fname].push_back(command[5]);
                    file_full_sha[fname] = command[4];
                    file_partial_sha[fname] = command[3];
                pthread_mutex_unlock(&mutex1);
                return "file_uploaded";
            
            }
            else if(strcmp(command[0].c_str(), "list_files")==0){
                string files="";
                vector<string> file_name = gr_file[command[1]];
                pthread_mutex_lock(&mutex1);
                for(string s: file_name){
                    files.append(s);
                    files.append("\n");
                }
                pthread_mutex_unlock(&mutex1);
                return files;
            }
            else if(strcmp(command[0].c_str(), "download_file")==0){
                string final_ports_sha="";
                vector<string> ports = file_port[command[2]];
                pthread_mutex_lock(&mutex1);
                for(string s: ports){
                    final_ports_sha.append(s);
                    final_ports_sha.append(" ");
                }
                final_ports_sha.append(file_full_sha[command[2]]);
                final_ports_sha.append(" ");
                final_ports_sha.append(file_partial_sha[command[2]]);
                pthread_mutex_unlock(&mutex1);
                return final_ports_sha;
            }
            else if(strcmp(command[0].c_str(), "show_downloads")==0){

            }
            else if(strcmp(command[0].c_str(), "stop_sharing")==0){

            }
            else if(strcmp(command[0].c_str(), "logout")==0){

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

        // function to handle type of request from peers in tracker
        string response = request_handler(request_type, login_flag, curr_user);

        write(newsockfd, response.c_str(), strlen(response.c_str()));
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