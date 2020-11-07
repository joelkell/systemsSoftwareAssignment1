#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int startAuditWatch() {

    int pid = fork();
 
    if (pid == 0) {
        system("sudo auditctl -w /var/www/html/intranet/ -p wrxa");
        exit(EXIT_SUCCESS);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        return 0;
    }

	
}
