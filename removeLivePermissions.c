#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int removeLivePermissions() {

    int pid = fork();
 
    if (pid == 0) {
        char *argv[]={"chmod","555","-R","/var/www/html/live", NULL};
        execv("/bin/chmod",argv);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        return 0;
    }

	
}
