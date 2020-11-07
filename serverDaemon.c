#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <errno.h>
#include "daemonLoop.h"
#include "transfer.h"
#include "givePermissions.h"
#include "removePermissions.h"
#include "removeLivePermissions.h"

#define LOCKFILE "/var/run/serverDaemon.pid"
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
#define LOCK_SH 1

int is_running(void){
    int fd;
    char buf[16];

    fd = open(LOCKFILE, O_RDWR|O_CREAT, LOCKMODE);

    if(fd < 0){
        syslog(LOG_ERR, "can't open %s: %s", LOCKFILE, strerror(errno));
        printf("Daemon already running\n");
        exit(1);
    }
    if(flock(fd, LOCK_SH) < 0) {
        if(errno == EACCES || errno == EAGAIN) {
            close(fd);
            return(1);
        }
        syslog(LOG_ERR, "Can't lock %s: %s", LOCKFILE, strerror(errno));
        exit(1);
    }
    ftruncate(fd,0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf) + 1);
    return(0);
}

int main()
{
    is_running();

    // Create a child process      
    int pid = fork();
 
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    } else if (pid == 0) {
       // Step 1: Create the orphan process
       
       // Step 2: Elevate the orphan process to session leader, to loose controlling TTY
       // This command runs the process in a new session
       if (setsid() < 0) { exit(EXIT_FAILURE); }

       // We could fork here again , just to guarantee that the process is not a session leader
       int pid = fork();
       if (pid > 0) {
            exit(EXIT_SUCCESS);
       } else {
       
            // Step 3: call umask() to set the file mode creation mask to 0
            // This will allow the daemon to read and write files 
            // with the permissions/access required 
            umask(0);

            // Step 4: Change the current working dir to root.
            // This will eliminate any issues of running on a mounted drive, 
            // that potentially could be removed etc..
            if (chdir("/") < 0 ) { exit(EXIT_FAILURE); }

            // int fd;
            // fd = open("dev/null", O_RDWR);
            // dup2(fd, STDIN_FILENO);
            // dup2(fd, STDOUT_FILENO);
            // dup2(fd, STDERR_FILENO);

            // close(STDIN_FILENO);
            // close(STDOUT_FILENO);
            // close(STDERR_FILENO);

            //set watch
            givePermissions();
            removeLivePermissions();

            int pid = fork();
            if (pid > 0) {
                while(1) {
                    printf("\nCountdown = 0\n");
                    printf("daemonLoop\n");
                    daemonLoop();
                    printf("daemonLoop finished\n");
                    sleep(30);
                }
            } else if (pid == 0){
                mqd_t mq;
                struct mq_attr queue_attributes;
                char buffer[1024 + 1];

                queue_attributes.mq_flags = 0;
                queue_attributes.mq_maxmsg = 10;
                queue_attributes.mq_msgsize = 1024;
                queue_attributes.mq_curmsgs = 0;

                mq = mq_open("/force_transfer_queue", O_CREAT | O_RDONLY, 0644, &queue_attributes);
                
                do {
                    ssize_t bytes_read;

                    bytes_read = mq_receive(mq, buffer, 1024, NULL); 
                    buffer[bytes_read] = "\0";

                    //CHANGE THIS TO BE if not transfer_complete block - transfer, else - set

                    if (! strncmp(buffer, "transfer_complete", strlen("transfer_complete"))) {
                        givePermissions();
                    } else {
                        printf("File: %s\n", buffer);
                        removePermissions();
                        transfer("/force_transfer_queue");
                    }

                } while(1);

                mq_close(mq);
                mq_unlink("/force_transfer_queue");
                return 0;
            }else{
                printf("Fork Failed");
            }
            
       }
    }
 
    return 0;
}