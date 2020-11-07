#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include <stdlib.h>

int main() {
	mqd_t mq;
	char buffer[1024];	

	mq = mq_open("/force_transfer_queue", O_WRONLY);

	printf("File to transfer: \n");
    printf(">> ");
    fgets(buffer, 1024, stdin);
    mq_send(mq, buffer, 1024, 0);

	mq_close(mq);
	return 0;
}
