#include "msg.h"

int send_message(int msg_qid, struct msgbuf *qbuf ) { 
    int result, length; 
    /* The length is essentially the size of the structure minus sizeof(mtype) */ 
    length = sizeof (struct msgbuf) - sizeof(long);         
    if((result = msgsnd(msg_qid, qbuf, length, 0)) == -1) { 
        return -1; 
    } 
    return (result);
} 

int read_message(int qid, long type, struct msgbuf *qbuf ) { 
	int result, length; 
    // The length is essentially the size of the structure minus sizeof(mtype)  
    length = sizeof(struct msgbuf) - sizeof(long);         
    if ((result = msgrcv(qid, qbuf, length, type,  0)) == -1) { 
        return -1; 
    } 
    return (result); 
} 


void set_message(int pid, char* buffer) {
    msg.msg_type = pid;
    sprintf(msg.msg_data, buffer);
    msg.msg_len = strlen(msg.msg_data);
}