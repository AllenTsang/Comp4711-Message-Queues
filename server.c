#include "server.h"

int main() {
    key_t mkey = ftok(".", 'c');
    int retval;
    struct msqid_ds msq_status;
    
    signal(SIGINT, cleanup);
    
    //get message queue identifier
    if((msq_id = msgget(mkey, IPC_CREAT|0660)) < 0) {
        perror("msgget failed");
        exit(2);
    }
    
    //get status info
    if(msgctl(msq_id, IPC_STAT, &msq_status) < 0) {
        perror("msgctl(get status) failed");
        exit(3);
    }
    
    //forever loop read, runs until ^C
    for(;;) {
        if((retval = read_message(msq_id, 1, &msg)) == -1) {
            perror("read_message failed");
            exit(4);
        }
        fprintf(stderr, "Message was: %s\n", msg.msg_data);
        
        //fork, child responds by pid
        if(fork() == 0) {
            handle_client();
        }
    }
}


void handle_client() {
    char filename[LINESIZE] = "";
    int clientpid = 0, priority = 0;
    FILE *fp;
    
    signal(SIGINT, SIG_DFL);
    //full server read process here - open file to read, packetize and send messages
    
    sscanf(msg.msg_data, "%d %d %s", &clientpid, &priority, filename);
    
    //fprintf(stderr, "clientpid: %d\npriority: %d\nfilename: %s\n", clientpid, priority, filename);
    
    //open file
    if((fp = fopen(filename,"r")) == 0) {
        perror("fopen failed");
        //send no such file message
        set_message(clientpid, "No such file.");
        if((send_message(msq_id, &msg)) == -1) { 
            perror("send_message failed on no such file"); 
            exit(1); 
        } 
        exit(1);
    }
    
    //packetize
    /*
    size_t current = 0, size = 0;
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    */
    
    char buffer[MAXMSGDATA];
    while(!feof(fp)) {
        fread(buffer, sizeof(buffer), 1, fp);
        //create message
        set_message(clientpid, buffer);
        
        //send message
        if((send_message(msq_id, &msg)) == -1) { 
            perror("send_message failed"); 
            exit(1); 
        } 
        memset(buffer, '\0', MAXMSGDATA);
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


