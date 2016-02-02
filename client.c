#include "client.h"

int main() {
    key_t mkey = ftok(".", 'c');
    int msq_id, retval, priority = 0;
    struct msqid_ds msq_status;
    char filename[LINESIZE] = "";
    
    //use thread somewhere
    
    
    fprintf(stderr, "Enter your filename and priority level: ");
    scanf("%79s %d", filename, &priority);
    
    fprintf(stderr, "Filename: %s\nPriority: %d\n", filename, priority);
    
    
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
    
    //fill in message to server
    msg.msg_type = 1;
    //send pid, priority, filename in message
    sprintf(msg.msg_data, "%d %d %s", getpid(), priority, filename);
    
    
    msg.msg_len = strlen(msg.msg_data);
    
    fprintf(stderr, "Message sent: %s\n", msg.msg_data);
    
    //send message
	if((send_message(msq_id, &msg)) == -1) { 
        perror("send_message failed"); 
        exit(1); 
	} 
    
    //get reply and print
    
    
    if((retval = read_message(msq_id, getpid(), &msg)) == 1) {
        perror("read_message failed");
        exit(4);
    }
    
    fprintf(stderr, "%s", msg.msg_data);
    
    
    return 0;
}