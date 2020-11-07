#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int backup() {

    int pid = fork();
 
    if (pid == 0) {

        time_t rawtime;
        struct tm * timeinfo;
        char directory[100];
        char buf[20];

        time ( &rawtime );
        timeinfo = localtime ( &rawtime );

        strcpy(directory,"/var/www/backup/");
        snprintf(buf, 12, "%d", (timeinfo->tm_year + 1900));
        strcat(directory,buf);
        snprintf(buf, 12, "%02d", timeinfo->tm_mon + 1);
        strcat(directory,buf);
        snprintf(buf, 12, "%02d", timeinfo->tm_mday);
        strcat(directory,buf);
        snprintf(buf, 12, "%02d", timeinfo->tm_hour);
        strcat(directory,buf);
        snprintf(buf, 12, "%02d", timeinfo->tm_min);
        strcat(directory,buf);
        snprintf(buf, 12, "%02d", timeinfo->tm_sec);
        strcat(directory,buf);
        strcat(directory,"/");

        char *argv[]={"rsync","-avuqp","--chmod=777","/var/www/html/intranet/",directory,NULL};
        execv("/bin/rsync",argv);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);

        mqd_t mq;
        char buffer[1024];	

        mq = mq_open("/status_queue", O_WRONLY);
        mq_send(mq, "backup_complete", 1024, 0);
        mq_close(mq);

        return 0;
    }

	
}
