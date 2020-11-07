#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <time.h>
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

int is_running(void){
    int pid_file = open(LOCKFILE, O_CREAT | O_RDWR, LOCKMODE);
    int rc = flock(pid_file, LOCK_EX | LOCK_NB);
    if(rc) {
        if(EWOULDBLOCK == errno){
            syslog(LOG_ERR, "can't open %s: %s", LOCKFILE, strerror(errno));
            printf("Daemon already running\n");
            exit(1);
        }
    }
    else {
        return(0);
    }
}

int main()
{
    is_running();

    time_t now;
    struct tm backup;
    double seconds;
    time(&now);  /* get current time; same as: now = time(NULL)  */
    backup = *localtime(&now);
    backup.tm_hour = 18; 
    backup.tm_min = 29; 
    backup.tm_sec = 0;

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

            //set watch
            givePermissions();
            removeLivePermissions();

            int pid = fork();
            if (pid > 0) {
                // while(1) {
                //     // printf("loop\n");
                //     // sleep(1);
                //     sleep(1);
                //     time(&now);
                //     seconds = difftime(now,mktime(&backup));
                //     printf("%.f\n", seconds);
                //     if (seconds == 0) {
                //         daemonLoop();
                //     }
                // }
                exit(EXIT_SUCCESS);
            } else if (pid == 0){
                mqd_t mq;
                struct mq_attr queue_attributes;
                char buffer[1024 + 1];
                int terminate = 0;

                queue_attributes.mq_flags = 0;
                queue_attributes.mq_maxmsg = 10;
                queue_attributes.mq_msgsize = 1024;
                queue_attributes.mq_curmsgs = 0;

                mq = mq_open("/force_transfer", O_CREAT | O_RDONLY, 0644, &queue_attributes);

                do {
                    ssize_t bytes_read;

                    bytes_read = mq_receive(mq, buffer, 1024, NULL);
                    buffer[bytes_read] = "\0";

                    if (! strncmp(buffer, "transfer_complete", strlen("transfer_complete"))) {
                        printf("1 buffer: %s\n", buffer);
                        givePermissions();
                    } else if(! strncmp(buffer, "transfer_start", strlen("transfer_start"))){
                        printf("2 buffer: %s\n", buffer);
                        removePermissions();
                        transfer("/force_transfer");
                    }else{
                        printf("3 buffer: %s\n", buffer);
                        syslog(LOG_ERR, "Message Queue Error: %s", buffer);
                    }

                } while(!terminate);

                mq_close(mq);
                mq_unlink("/force_transfer");
                exit(EXIT_FAILURE);
                // mqd_t mq;
                // struct mq_attr queue_attributes;
                // char buffer[1024 + 1];
                // int terminate = 0;

                // queue_attributes.mq_flags = 0;
                // queue_attributes.mq_maxmsg = 10;
                // queue_attributes.mq_msgsize = 1024;
                // queue_attributes.mq_curmsgs = 0;

                // mq = mq_open("/force_transfer", O_CREAT | O_RDONLY, 0644, &queue_attributes);
                
                // do {
                //     printf("other loop\n");
                //     ssize_t bytes_read;

                //     bytes_read = mq_receive(mq, buffer, 1024, NULL); 
                //     buffer[bytes_read] = "\0";
                //     printf("buffer: %s\n", buffer);

                //     if (! strncmp(buffer, "transfer_complete", strlen("transfer_complete"))) {
                //         printf("2: %s\n", buffer);
                //         givePermissions();
                //     } else if (! strncmp(buffer, "transfer_start", strlen("transfer_start"))){
                //         printf("1: %s\n", buffer);
                //         removePermissions();
                //         transfer("/force_transfer");
                //     }
                //     else {
                //         printf("3: %s\n", buffer);
                //         syslog(LOG_ERR, "Message Queue Error: %s", buffer);
                //     }

                // } while(1);

                // mq_close(mq);
                // mq_unlink("/force_transfer");
                // return 0;
            }
       }
    }
 
    return 0;
}