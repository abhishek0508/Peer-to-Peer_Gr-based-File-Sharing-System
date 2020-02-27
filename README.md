# Peer-to-Peer-Group-based-File-Sharing-System

# Architecture overview:
# 1. Synchronized trackers(​ 2 tracker system)​ :
  a. Maintain information of clients with their files(shared by client) to assist the clients
for the communication between peers 
  b. Trackers should be synchronized i.e all the trackers if online should be in sync
with each other
# 2. Clients:
  a. User should create an account and register with tracker <br/>
  b. Login using the user credentials<br/>
  c. Create Group and hence will become owner of that group<br/>
  d. Fetch list of all Groups in server<br/>
  e. Request to Join Group<br/>
  f. Leave Group<br/>
  g. Accept Group join requests (if owner)<br/>
  h. Share file across group: Share the filename and SHA1 hash of the complete file<br/>
  as well as piecewise SHA1 with the tracker<br/>
  i. Fetch list of all sharable files in a Group<br/>
  j. Download file<br/>
  
  k.Retrieve peer information from tracker for the file<br/>
 
  # Core Part:​ Download file from multiple peers (different pieces of file from different peers - ​ piece selection algorithm​ ) simultaneously and all the files which client downloads will be shareable to other users in the same group.
  Ensure file integrity from SHA1 comparison
  k. Show downloads<br/>
  l. Stop sharing file<br/>
  m. Stop sharing all files(Logout)<br/>
  n. Whenever client logins, all previously shared files before logout should<br/>
  automatically be on sharing mode<br/>

## Working

Working​ :
1. At Least one tracker will always be online.<br/>
2. Client needs to create an account (userid and password) in order to be part of the
network.
3. Client can create any number of groups(groupid should be different) and hence
will be owner of those groups<br/>
4. Client needs to be part of the group from which it wants to download the file<br/>
5. Client will send join request to join a group<br/>
6. Owner Client Will Accept/Reject the request<br/>
7. After joining group ,client can see list of all the shareable files in the group<br/>
8. Client can share file in any group (​ note: ​ file will not get uploaded to tracker but
only the ​ <ip>:<port>​ of the client for that file)<br/>
9. Client can send the download command to tracker with the group name and
filename and tracker will send the details of the group members which are
currently sharing that particular file<br/>
10. After fetching the peer info from the tracker, client will communicate with peers
about the portions of the file they contain and hence accordingly decide which
part of data to take from which peer (You need to design your own Piece
Selection Algorithm)<br/>
11. As soon as a piece of file gets downloaded it should be available for sharing<br/>
12. After logout, the client should temporarily stop sharing the currently shared files
till the next login<br/>
13. All trackers need to be in sync with each other<br/>
  
## Commands​ :
## 1. Tracker​ :
  a. Run Tracker:<br/>
  ./tracker​ tracker_info.txt ​ tracker_no<br/>
  tracker_info.txt - Contains ip, port details of all the trackers<br/>
  b. Close Tracker:<br/>
quit
## 2. Client​ :<br/>
  a. Run Client:<br/>
  ./client​ <IP>:<PORT> tracker_info.txt<br/>
  tracker_info.txt - Contains ip, port details of all the trackers<br/>
  b. Create User Account:<br/>
  create_user​ <user_id> <passwd><br/>
  c. Login:<br/>
  login ​ <user_id> <passwd><br/>
  d. Create Group:<br/>
  create_group​ <group_id><br/>
  e. Join Group:<br/>
  join_group​ <group_id><br/>
  f. Leave Group:<br/>
  leave_group​ <group_id><br/>
  g. List pending join requests<br/>
  list_requests ​ <group_id><br/>
  h. Accept Group Joining Request:<br/>
  accept_request​ <group_id> <user_id><br/>
  i. List All Group In Network:<br/>
  list_groups<br/>
  j. List All sharable Files In Group:<br/>
  list_files​ <group_id><br/>
  k. Upload File:<br/>
  upload_file​ <file_path> <group_id><br/>
  l. Download File:<br/>
  download_file​ <group_id> <file_name> <destination_path><br/>
  m. Logout:<br/>
  logout<br/>
  n. Show_downloads<br/>
  Show_downloads<br/>
  Output format:<br/>
  [D] [grp_id] filename<br/>
  [C] [grp_id] filename<br/>
  D(Downloading), C(Complete)<br/>
  o. Stop sharing<br/>
  stop_share ​ <group_id> <file_name><br/>



