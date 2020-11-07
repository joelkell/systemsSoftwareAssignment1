#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* for fork */
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h> /* for wait */

int writeAuditLogs() {

    int pid = fork();
 
    if (pid == 0) {

        printf("Write audit logs Child start\n");

        char *argv[]={"echo","Write audit logs is running now....",NULL};
        //argv[0] is the command to run: echo
        //argv[1] text to be echoed
        //argv[2] null, needed!!
        execv("/bin/echo",argv);
    } else if (pid > 0) {

        printf("Write audit logs before wait\n");

        int status;
        waitpid(pid, &status, 0);
        printf("Write audit logs Finished Waiting\n");
        return 0;
    }

	
}
