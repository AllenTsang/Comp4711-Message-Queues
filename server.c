#include "server.h"

int main() {
    key_t mkey = ftok(".", 'c');
    int retval;
    struct msqid_ds msq_status;
    
    signal(SIGINT, cleanup);
    
    /*---- Get message queue identifier ------*/
    if ((msq_id = msgget(mkey, IPC_CREAT|0660)) < 0) {
        perror("msgget failed");
        exit(2);
    }
    
    /*--- get status info -----------------*/
    if (msgctl(msq_id, IPC_STAT, &msq_status) < 0) {
        perror("msgctl(get status) failed");
        exit(3);
    }
    
    //forever loop read, runs until ^C
    for(;;) {
        if((retval = read_message(msq_id, 1, &msg)) == 1) {
            perror("read_message failed");
            exit(4);
        }
        fprintf(stderr, "Message was: %s\n", msg.msg_data);
        
        //fork, child responds by pid
        if(fork() == 0) {
            signal(SIGINT, SIG_DFL);
            fprintf(stderr, "child process %d\n", getpid());
            exit(0);
        }
        
    }
    
    //Cleanup (parent only):
    //kill(0, SIGINT);
    //return 0;
}



void cleanup(int signo) {
    signal(SIGINT, SIG_DFL);
    // Remove the message queue
    if(msgctl(msq_id, IPC_RMID, 0) < 0) {
        perror("msgctl(remove queue) failed");
        //exit(3);
    }
    kill(0, SIGINT);
}


