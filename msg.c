/*
SOURCE FILE:    msg.c
PROGRAM:        Comp4981 Assignment 2 - Message Queue Client/Server
FUNCTIONS:      int send_message(int qid, struct msgbuf *qbuf)
                int read_message(int qid, long type, struct msgbuf *qbuf)
                void set_message(long mtype, char buffer[])
DATE:           Jan 25, 2016
REVISIONS:      v1
DESIGNER:       Allen Tsang
PROGRAMMER:     Allen Tsang
NOTES:
    The message handling component of the programs.  Contains the
    message structure, as well as functions for sending it, reading
    from it, and setting its contents.
*/
#include "msg.h"

/*
FUNCTION:       send_message
DATE:           Jan 25, 2016
REVISIONS:      v1
DESIGNER:       Allen Tsang
PROGRAMMER:     Allen Tsang
INTERFACE:      int send_message(int qid, struct msgbuf *qbuf)
                    int qid: message queue id
                    struct msgbuf *qbuf: message to send
RETURNS:        -1 if sending fails, number of bytes sent otherwise.
NOTES:
    Sends a message to the message queue.
*/
int send_message(int qid, struct msgbuf *qbuf) { 
    int result, length; 
    /* The length is essentially the size of the structure minus sizeof(mtype) */ 
    length = sizeof (struct msgbuf) - sizeof(long);
    if((result = msgsnd(qid, qbuf, length, 0)) == -1) { 
        return -1; 
    } 
    return (result);
} 

/*
FUNCTION:       read_message
DATE:           Jan 25, 2016
REVISIONS:      v1
DESIGNER:       Allen Tsang
PROGRAMMER:     Allen Tsang
INTERFACE:      int read_message(int qid, long type, struct msgbuf *qbuf)
                    int qid: message queue id
                    long type: message type to read
                    struct msgbuf *qbuf: structure to read message into
RETURNS:        -1 if reading fails, number of bytes read otherwise.
NOTES:
    Reads a message from the message queue.
*/
int read_message(int qid, long type, struct msgbuf *qbuf) { 
	int result, length; 
    // The length is essentially the size of the structure minus sizeof(mtype)  
    length = sizeof(struct msgbuf) - sizeof(long);
    if ((result = msgrcv(qid, qbuf, length, type, 0)) == -1) { 
        return -1; 
    } 
    return (result); 
} 

/*
FUNCTION:       set_message
DATE:           Jan 29, 2016
REVISIONS:      v1
DESIGNER:       Allen Tsang
PROGRAMMER:     Allen Tsang
INTERFACE:      int set_message(int mtype, char buffer[])
                    long mtype: message type to set
                    char buffer[]: message data to set
RETURNS:        void
NOTES:
    Sets the fields of a message structure.
*/
void set_message(long mtype, char buffer[]) {
    msg.msg_type = mtype;
    sprintf(msg.msg_data, buffer);
    buffer[MAXMSGDATA] = '\0';
    msg.msg_len = strlen(msg.msg_data);
}


void remove_messages(int qid, int pid) {
    while(1) {
        int length = sizeof(struct msgbuf) - sizeof(long);
        msgrcv(qid, 0, length, pid, 0);
        read_message(qid, pid, 0);
        if(errno == ENOMSG) {
            break;
        }
    }
}
