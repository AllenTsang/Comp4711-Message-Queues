#include "client.h"

int main(int argc, char** argv) {
    key_t mkey = ftok(".", 'c');
    int msq_id, retval;
    struct msqid_ds msq_status;
    
    /*if (argc != 2) {
        fprintf (stderr, "Usage: %s keyval\n", argv[0]);
        exit(1);
    }*/
    
    //get filename from standard input
    
    //client to server = mtype 1
    
    /*---- Get message queue identifier ------*/
    //mkey = (key_t) atoi(argv[1]);
    if ((msq_id = msgget (mkey, IPC_CREAT|0660)) < 0) {
        perror ("msgget failed!");
        exit(2);
    }
    
    /*--- get status info -----------------*/
    if (msgctl (msq_id, IPC_STAT, &msq_status) < 0) {
        perror ("msgctl (get status)failed!");
        exit(3);
    }
    
    msg.msg_type = 1;
    sprintf(msg.msg_data, "test message");
    msg.msg_len = sizeof(msg.msg_data);
    
    fprintf(stderr, "%s\n", msg.msg_data);
    
    //send
	if((send_message( msq_id, &msg )) == -1) { 
        perror("send_message failed"); 
        exit(1); 
	} 
    
    //get reply and print
    
    
    return 0;
}