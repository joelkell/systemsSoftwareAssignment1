#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include <stdlib.h>

int main() {
	mqd_t mq;
	char buffer[1024];	

	mq = mq_open("/force_transfer", O_WRONLY);
	mq_send(mq, "transfer_start", 1024, 0);
	mq_close(mq);
	
	return 0;
}
