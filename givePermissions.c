#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h>

int givePermissions() {

    int pid = fork();
 
    if (pid == 0) {
        char *argv[]={"chmod","777","-R","/var/www/html/intranet", NULL};
        execv("/bin/chmod",argv);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        return 0;
    }	
}
