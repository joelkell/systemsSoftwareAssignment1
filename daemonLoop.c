#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include <stdlib.h>
#include <syslog.h>
#include "backup.h"
#include "transfer.h"
#include "givePermissions.h"
#include "removePermissions.h"
#include "writeAuditLogsToFile.h"

int daemonLoop() {
	mqd_t mq;
	struct mq_attr queue_attributes;
	char buffer[1024 + 1];
    int terminate = 0;

	queue_attributes.mq_flags = 0;
	queue_attributes.mq_maxmsg = 10;
    queue_attributes.mq_msgsize = 1024;
	queue_attributes.mq_curmsgs = 0;

	mq = mq_open("/status_queue", O_CREAT | O_RDONLY, 0644, &queue_attributes);

	removePermissions();
	
    backup();

	do {
		ssize_t bytes_read;

		bytes_read = mq_receive(mq, buffer, 1024, NULL);
		buffer[bytes_read] = "\0";

		if (! strncmp(buffer, "backup_complete", strlen("backup_complete"))) {
            transfer("/status_queue");
		} else if(! strncmp(buffer, "transfer_complete", strlen("transfer_complete"))){
			givePermissions();
            writeAuditLogs();
            terminate = 1;
		}else{
            syslog(LOG_ERR, "Message Queue Error: %s", buffer);
        }

	} while(!terminate);

	mq_close(mq);
	mq_unlink("/status_queue");
	return 0;
}