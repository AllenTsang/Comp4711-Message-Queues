#ifndef MSG_H
#define MSG_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MAXMSGDATA (4096-16)
/* don't want sizeof(Msg) > 4096 */
#define MSGHDRSIZE (sizeof(msg) - MAXMSGDATA)

/* length of msg_len and msg_type */
struct msgbuf{
    long msg_type; /* message type */
    int msg_len; /* # bytes in msg_data */
    char msg_data [MAXMSGDATA];
} msg;

int send_message(int msg_qid, struct msgbuf *qbuf);
int read_message(int qid, long type, struct msgbuf *qbuf);
#endif