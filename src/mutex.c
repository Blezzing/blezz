#include"mutex.h"

#include<stdio.h>
#include<sys/file.h>
#include<errno.h>

#define LOCK_PATH "/var/run/blezz/lock"

int pid_file;
int rc;

void applicationLock(){
    pid_file = open("/var/run/blezz/lock", O_CREAT | O_RDWR, 0666);
    if (pid_file == -1){
        printf("Failed creating the application lock, make sure the directory /var/run/blezz/ exist, and is executable by your user.\n");
    }
    rc = flock(pid_file, LOCK_EX | LOCK_NB);
}

void applicationRelease(){
    flock(pid_file, LOCK_UN);
}

int applicationFirstInstance(){
    if(rc) { //we could not get a lock
        if(EWOULDBLOCK == errno){
            printf("Application lock is already taken, refuses to start a new instance.\n");
            return 0;
        }
        else{
            return 1;
        }
    }
    else { //we got a lock
        return 1;
    }
}