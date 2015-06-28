#ifndef CHAT
#define CHAT

#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <vector>
#include <string.h>
#include <iostream>
#include <sstream>
#include <arpa/inet.h>

using namespace std;

#define MAX_FRIENDS 20
#define BUFF_SIZE 256
#define PORTNO 50131

char buffer_read[BUFF_SIZE];			// Temporary buffer to hold the received messages
char buffer_write[BUFF_SIZE];			// Temporary buffer to hold the messages to be sent

/* Struct containing data for each host connection*/
typedef struct s_friend {

	int socket;						// The socket that manages the connection between the two hosts
	string name;					// Name given to the contact
	string IP;						// contact IP				
	vector<string> messages;		// Vector that storages the received messages from the host
} s_friend;

/* Struct containing local data */
typedef struct local_host {
	
	int socket_init_pt;				// Welcome socket to receive new connection requests
	int port_number;				// Port number of the local application
	int active_window;				// Determine wether a friend's messages should or not be printed
	int number_friends;				// Number of friends in the local host's contact list
	vector<s_friend> contacts;		// Vector to store data regarding each conversation 

} local_host; 

local_host my_contact;

int group[MAX_FRIENDS];

int currentContact;

pthread_mutex_t mutex;

/* Function responsible for sending messages. */
void send_message();

/* Function responsible for sending group messages. */
void group_message();

/* Function responsible for receiving messages and storing them. --Implemented as a thread-- */
void receive_message();

/* Creates the server side of the application. (Creates the welcome socket) */
void create_server();

/* Accepts new income connection requests to the server. Creates a new TCP socket to handle the connection. --Implemented as a thread-- */
void accept_connection();

/* Creates the client side of the application. (Will be changed to a function responsible for logging the host into the network) */
void create_client();

/* Removes a host with index chosen by user */
void remove_friend();

/* Removes a host with specific index */
void remove_client(int index);

/* Attributes name to a contact */
void name_contact();

/* Print buffer containing user messages */
void print_buffer();

/* Print a list of all contacts in the format: index/name/IP */
void print_contacts();

/* Verify if the connection was closed from one of the sides */
bool isclosed(int sock);

/* Main function. Starts the threads that control the application */
int main(int argc, char * argv[]);

#endif
