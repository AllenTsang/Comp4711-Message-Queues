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
            handle_client();
        }
    }
    //Cleanup (parent only):
    //kill(0, SIGINT);
    //return 0;
}

void handle_client() {
    char filename[LINESIZE] = "";
    int clientpid = 0, priority = 0;
    FILE *fp;
    
    signal(SIGINT, SIG_DFL);
    //full server read process here - open file to read, packetize and send messages
    fprintf(stderr, "child process %d\n", getpid());
    
    
    
    sscanf(msg.msg_data, "%d %d %s", &clientpid, &priority, filename);
    
    fprintf(stderr, "clientpid: %d\npriority: %d\nfilename: %s\n", clientpid, priority, filename);
    
    //open file
    if((fp = fopen(filename,"r")) == 0) {
        perror("fopen failed");
        exit(1);
    }
    
    //packetize
    
    
    //create message
    msg.msg_type = clientpid;
    sprintf(msg.msg_data, "this is a test message");
    msg.msg_len = strlen(msg.msg_data);
    
    //send message
    if((send_message(msq_id, &msg)) == -1) { 
        perror("send_message failed"); 
        exit(1); 
	} 
    
    //close file
    if(fclose(fp) != 0) {
        perror("fclose failed");
        exit(1);
    }
    
    exit(0);
}


void cleanup(int signo) {
    signal(SIGINT, SIG_DFL);
    //Remove the message queue
    if(msgctl(msq_id, IPC_RMID, 0) < 0) {
        perror("msgctl(remove queue) failed");
        //exit(3);
    }
    kill(0, SIGINT);
}


