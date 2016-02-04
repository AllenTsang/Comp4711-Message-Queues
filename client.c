/*
SOURCE FILE:    client.c
PROGRAM:        Comp4981 Assignment 2 - Message Queue Client/Server
FUNCTIONS:      int main()
                void fatal(char* s)
DATE:           Jan 25, 2016
REVISIONS:      v1
DESIGNER:       Allen Tsang
PROGRAMMER:     Allen Tsang
NOTES:
    The client program for a message queue client/server.  It prompts
    the user for a filename to request, as well as a priority level.
    It sends this request to the server, and is either sent the file,
    or notified that the server could not find it.  In either case,
    the client exits.
*/
#include "client.h"

/*
FUNCTION:       main
DATE:           Jan 25, 2016
REVISIONS:      Jan 29, 2016
                    Added functionality.
DESIGNER:       Allen Tsang
PROGRAMMER:     Allen Tsang
INTERFACE:      int main()
RETURNS:        0 if the program exits normally.
                Other values may be returned otherwise.
NOTES:
    The main function runs the client side of the program.  It prompts
    the user for a filename to request, as well as a priority level.
    Once it has these, it looks for the message queue set up by the
    server and retrieves the relevant information before sending the
    file request message.  It then enters a read loop until the server
    sends an EOF packet signaling that the entire file requested has
    been sent.  Alternatively, the server can notify the client that
    the file could not be found, then send the EOF packet.  Once this
    is done, the client exits.
*/
int main() {
    key_t mkey = ftok(".", 'c');
    int msq_id, retval, priority = 0;
    struct msqid_ds msq_status;
    char filename[LINESIZE], buffer[MAXMSGDATA];
    char* prompt = "Enter your filename and priority level: ";
    pthread_t thread;
    
    pthread_create(&thread, NULL, prompt_thread, prompt);
    pthread_join(thread, NULL);
    
    scanf("%s %d", filename, &priority);
    
    if((msq_id = msgget(mkey, IPC_CREAT|0660)) < 0) {
        fatal("Failed to get message queue identifier.");
    }
    
    if(msgctl(msq_id, IPC_STAT, &msq_status) < 0) {
        fatal("Failed to get status info.");
    }
    
    sprintf(buffer, "%d %d %s", getpid(), priority, filename);
    set_message(1, buffer);
    
	if((send_message(msq_id, &msg)) == -1) { 
        fatal("Failed to send message");
	} 
    
    signal(SIGINT, SIG_IGN);
    
    //read loop, until server sends the EOF final packet
    while((retval = read_message(msq_id, getpid(), &msg)) != -1) {
        if(msg.msg_data[0] == EOF) {
            fprintf(stdout, "\n");
            fflush(stdout);
            break;
        }
        fprintf(stdout, "%s", msg.msg_data);
        fflush(stdout);
    } 
    
    if(retval == -1) {
        fatal("Failed to read message.");
    }
    
    return 0;
}

/*
FUNCTION:       prompt_thread
DATE:           Feb 3, 2016
REVISIONS:      v1
DESIGNER:       Allen Tsang
PROGRAMMER:     Allen Tsang
INTERFACE:      void* prompt_thread(void* params)
                    void* params: char* to prompt with
RETURNS:        void
NOTES:
    Run by a thread created by the client.  Prompts the user
    for input, then exits.
*/
void* prompt_thread(void* params) {
    fprintf(stderr, (char*)params);
    pthread_exit(0);
    return 0;
}

/*
FUNCTION:       fatal
DATE:           Jan 13, 2016
REVISIONS:      v1
DESIGNER:       Allen Tsang
PROGRAMMER:     Allen Tsang
INTERFACE:      void fatal(char* s)
                    char* s: description of the error to print
RETURNS:        void
NOTES:
    Prints the description of the error and exits the process.
*/
void fatal(char* s) {
    perror(s);
    exit(1);
}