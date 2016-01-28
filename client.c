#include "client.h"

int main(int argc, char** argv) {
    key_t mkey = ftok(".", 'c');
    int msq_id, retval;
    struct msqid_ds msq_status;
    
    /*if (argc != 3) {
        fprintf (stderr, "Usage: %s <priority number> <filename>\n", argv[0]);
        exit(1);
    }*/
    
    //from filename from stdin?
    
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
    
    //message to server
    msg.msg_type = 1;
    sprintf(msg.msg_data, "test message: pid %d", getpid());
    msg.msg_len = strlen(msg.msg_data);
    
    fprintf(stderr, "%s\n", msg.msg_data);
    
    //send
	if((send_message( msq_id, &msg )) == -1) { 
        perror("send_message failed"); 
        exit(1); 
	} 
    
    //get reply and print
    
    
    return 0;
}