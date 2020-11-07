#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int writeAuditLogs() {

    int pid = fork();
 
    if (pid == 0) {
        int fd = open("/var/www/logs/auditlogs.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

        dup2(fd, 1);
        dup2(fd, 2);

        close(fd);

        system("ausearch -f /var/www/html/intranet/ | aureport -f -i");
        exit(EXIT_SUCCESS);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        return 0;
    }

	
}
