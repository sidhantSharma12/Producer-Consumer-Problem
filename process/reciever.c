#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <math.h>
#include <time.h>

int main(int argc, const char * argv[]){

	mqd_t qdes;
	mqd_t counter;

	int consumed;
	int counted;

	int id= atoi(argv[1]);

	int numbProd = atoi(argv[2]);
    int arraySize = atoi(argv[3]);

	char *qname = "/mailbox";
	char *counterName = "/counter";

	mode_t mode = S_IRUSR | S_IWUSR;
	struct mq_attr attr;


	attr.mq_maxmsg  = arraySize; /* Max. # of messages on queue */
	attr.mq_msgsize = sizeof(int);/* Max. message size (bytes) */
	attr.mq_flags   = 0;		/* a blocking queue  */

	qdes  = mq_open(qname, O_RDWR | O_CREAT, mode, &attr);

	attr.mq_maxmsg  = 1; // This is for the counter
	attr.mq_msgsize = sizeof(int);
	attr.mq_flags   = 0;		

	counter  = mq_open(counterName, O_RDWR | O_CREAT, mode, &attr);


	while(1){
	
		mq_receive(counter, (char *)&counted, sizeof(int), 0);

		if (counted==numbProd){
			mq_send(counter, (char *)&counted, sizeof(int), 0);//send if uve consumed everything
			break;
		}

		else{
			counted+=1;
			mq_send(counter, (char *)&counted, sizeof(int), 0);
		}


		mq_receive(qdes, (char *)&consumed, sizeof(int), 0);

        int root =(int) sqrt(consumed);

        if(root*root==consumed){
          printf("%d %d %d\n", id, consumed, root);
        }

	}

	return 0;
}