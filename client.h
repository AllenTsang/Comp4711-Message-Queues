#ifndef CLIENT_H
#define CLIENT_H

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "msg.h"

int main();
void* prompt_thread(void* params);
void fatal(char* s);
#endif