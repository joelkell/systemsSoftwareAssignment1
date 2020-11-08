#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int transfer(char* queueName) {

    int pid = fork();
 
    if (pid == 0) {
        char *argv[]={"rsync","-avuqp","--chmod=555","/var/www/html/intranet/","/var/www/html/live/",NULL};
        execv("/usr/bin/rsync",argv);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);

        mqd_t mq;
        char buffer[1024];	

        mq = mq_open(queueName, O_WRONLY);
        mq_send(mq, "transfer_complete", 1024, 0);
        mq_close(mq);
        
        return 0;
    }

	
}
