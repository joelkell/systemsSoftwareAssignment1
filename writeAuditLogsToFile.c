#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int writeAuditLogs() {

    int pid = fork();
 
    if (pid == 0) {
        char *argv[]={"echo","Write audit logs is running now....",NULL};
        execv("/bin/echo",argv);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        return 0;
    }

	
}
