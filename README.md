# MKS65C-FinalProject: Chatrooms
Chatroom application with multiple chatrooms and clients!
# Features
* Server-client communication
* Usernames
* Broadcast messages to all clients
* Distinguish between different chatrooms
* Switch between different chatrooms
# Instructions
1. Clone this repository.
```
git clone https://github.com/alexliu4/MKS65C-FinalProject.git
```
2. Go inside this repository.
```
cd MKS65C-FinalProject
```
3. Run make.
```
make
```
4. Run the server executable.
```
./server
```
5. Run the client executable, followed by the server that the server executable is running in.
```
./client <server_name>
```
6. Enter your username as prompted.
7. Join a chatroom as prompted.
8. To change chatrooms, type the following:
```
~join <valid_chatroom_number>
```
# Bugs
* Chatroom storage is limited (may eventually break).
  * If time allows, use a circular array resolve this bug.
* After entering their username, the client must type something to receive further instruction.
