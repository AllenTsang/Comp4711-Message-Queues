#ifndef SERVER_H
#define SERVER_H

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "msg.h"

int msq_id;

int main();
void handle_client();
void final_packet(int clientpid);
void cleanup(int signo);
void fatal(char* s);
#endif