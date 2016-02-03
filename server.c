/*
SOURCE FILE:    server.c
PROGRAM:        Comp4981 Assignment 2 - Message Queue Client/Server
FUNCTIONS:      int main()
                void handle_client()
                void final_packet(int clientpid)
                void cleanup(int signo)
                void fatal(char* s)
DATE:           Jan 25, 2016
REVISIONS:      v1
DESIGNER:       Allen Tsang
PROGRAMMER:     Allen Tsang
NOTES:
    The server program for a message queue client/server.  It sets up
    a message queue and listens for client requests on it.  Requests
    are handled by forking a child process, then continuing to listen
    for new requests until the user stops the program by hitting ^C.
*/
#include "server.h"

/*
FUNCTION:       main
DATE:           Jan 25, 2016
REVISIONS:      Jan 29, 2016
                    Signal handling.
DESIGNER:       Allen Tsang
PROGRAMMER:     Allen Tsang
INTERFACE:      int main()
RETURNS:        0 if the program exits normally.
                Other values may be returned otherwise.
NOTES:
    The main function prepares the server side of the program.
    It changes SIGINT to call a cleanup function for when the server
    needs to shutdown, then creates a message queue and gets all
    relevant info.  It then enters a forever loop, reading messages
    addressed to the server through mtype being 1.  Whenever it
    receives a request, it will spawn a new process to handle the
    client and return to listening for new clients.  The loop is
    only exited when the server is stopped by a SIGINT from the user.
*/
int main() {
    key_t mkey = ftok(".", 'c');
    int retval;
    struct msqid_ds msq_status;
    
    signal(SIGINT, cleanup);
    
    if((msq_id = msgget(mkey, IPC_CREAT|0660)) < 0) {
        fatal("Failed to get message queue identifier.");
    }
    
    if(msgctl(msq_id, IPC_STAT, &msq_status) < 0) {
        fatal("Failed to get status info.");
    }
    
    //forever loop read, runs until ^C
    for(;;) {
        if((retval = read_message(msq_id, 1, &msg)) == -1) {
            fatal("Failed to read message.");
        }
        
        if(fork() == 0) {
            handle_client();
        }
    }
}

/*
FUNCTION:       handle_client
DATE:           Jan 25, 2016
REVISIONS:      Jan 29, 2016
                    Added functionality.
                Feb 2, 2016
                    Added EOF loop condition.
DESIGNER:       Allen Tsang
PROGRAMMER:     Allen Tsang
INTERFACE:      void handle_client()
RETURNS:        void
NOTES:
    Function to handle a client request.  To be called by a newly forked
    child process.  Resets SIGINT to its default behavior, reads the client
    request message for its pid, priority and requested filename.  If the
    requested file exists, it is sent through the message queue, then the
    file is closed and the process exits.  The amount of file data per message
    is determined by the requested priority.
*/
void handle_client() {
    char filename[LINESIZE], buffer[MAXMSGDATA];
    int clientpid = 0, priority = 0;
    FILE *fp;
    
    signal(SIGINT, SIG_DFL);
    
    sscanf(msg.msg_data, "%d %d %s", &clientpid, &priority, filename);
    
    if(priority == 0) {
        priority = 1;
    }
    
    if((fp = fopen(filename,"r")) == 0) {
        sprintf(buffer, "No such file.");
        set_message(clientpid, buffer);
        if((send_message(msq_id, &msg)) == -1) {
            fatal("Failed to send 'no such file' message.");
        } 
        final_packet(clientpid);
        fatal("Failed to open file.");
    }
    
    //file reading loop, until EOF is read
    while(!feof(fp)) {
        fread(buffer, sizeof(buffer)/priority, 1, fp);
        set_message(clientpid, buffer);
        if((send_message(msq_id, &msg)) == -1) { 
            fatal("send_message failed");
        } 
        memset(buffer, '\0', MAXMSGDATA);
    }
    final_packet(clientpid);
    
    if(fclose(fp) != 0) {
        fatal("file close failed");
    }
    exit(0);
}

/*
FUNCTION:       final_packet
DATE:           Feb 2, 2016
REVISIONS:      v1
DESIGNER:       Allen Tsang
PROGRAMMER:     Allen Tsang
INTERFACE:      void final_packet(int clientpid)
                    int clientpid: pid of client process to tell to close
RETURNS:        void
NOTES:
    Sends a final packet to a client process after reading an entire file,
    telling that process to close.
*/
void final_packet(int clientpid) {
    char buffer[LINESIZE];
    buffer[0] = EOF;
    set_message(clientpid, buffer);
    if((send_message(msq_id, &msg)) == -1) { 
        fatal("send_message failed");
    }
}

/*
FUNCTION:       cleanup
DATE:           Jan 28, 2016
REVISIONS:      v1
DESIGNER:       Allen Tsang
PROGRAMMER:     Allen Tsang
INTERFACE:      void cleanup(int signo)
                    int signo: number of signal caught
RETURNS:        void
NOTES:
    Signal handling function for caught signals.  Only catches SIGINT.
    Restores default SIGINT functionality, removes the message queue,
    and then re-sends SIGINT to all processes in the group to close
    them all.  Only caught from the main parent process of the server.
*/
void cleanup(int signo) {
    signal(SIGINT, SIG_DFL);
    if(msgctl(msq_id, IPC_RMID, 0) < 0) {
        perror("msgctl(remove queue) failed");
        fatal("Failed to remove queue.");
    }
    kill(0, SIGINT);
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
    Reused wrapper function from a previous assignment.
*/
void fatal(char* s) {
    perror(s);
    exit(1);
}
