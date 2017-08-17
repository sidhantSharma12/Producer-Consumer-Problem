#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>


int main(int argc, const char * argv[]){

	mqd_t qdes;

	int id=  atoi(argv[1]);
	int numbProd = atoi(argv[2]);
    int arraySize = atoi(argv[3]);
    int producer = atoi(argv[4]);
	
	char *qname = "/mailbox";

	mode_t mode = S_IRUSR | S_IWUSR;

	struct mq_attr attr;

	attr.mq_maxmsg  = arraySize; /* Max. # of messages on queue */
	attr.mq_msgsize = sizeof(int);/* Max. message size (bytes) */
	attr.mq_flags   = 0;		/* a blocking queue  */

	qdes  = mq_open(qname, O_RDWR | O_CREAT, mode, &attr);
	
	
	if (qdes == -1 ) {
		perror("mq_open() failed");
		exit(1);
	}

	while(id < numbProd){

		if (mq_send(qdes, (char *)&id, sizeof(int), 0) == -1) { 
				perror("mq_send() failed");
		}

		id+=producer;
	}

	if (mq_close(qdes) == -1) { 
		perror("mq_close() failed");
		exit(2);
	}

	return 0;
}