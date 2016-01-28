#include "server.h"

int main() {
    key_t mkey = ftok(".", 'c');
    int retval;
    struct msqid_ds msq_status;
    
    signal(SIGINT, cleanup);
    
    /*---- Get message queue identifier ------*/
    if ((msq_id = msgget (mkey, IPC_CREAT|0660)) < 0) {
        perror ("msgget failed!");
        exit(2);
    }
    
    /*--- get status info -----------------*/
    if (msgctl (msq_id, IPC_STAT, &msq_status) < 0) {
        perror ("msgctl (get status)failed!");
        exit(3);
    }
    
    if((retval = read_message(msq_id, 1, &msg)) == 1) {
        perror("read_message failed");
        exit(4);
    }
    fprintf(stderr, "%s", msg.msg_data);
    
    //fork and respond
    
    
    // Cleanup:
    kill(0, SIGINT);
    
    return 0;
}

int cleanup(int signo) {
    // Remove the message queue
    if(msgctl (msq_id, IPC_RMID, 0) < 0) {
        perror ("msgctl (remove queue) failed!");
        exit(3);
    }
    exit(0);
}

//cleanup function