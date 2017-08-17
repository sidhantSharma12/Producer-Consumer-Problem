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


int numbProd;
int arraySize;
int producer;
int consumer;
int child_status;
int consumed = 0;

char *qname = "/mailbox";
char *counterName = "/counter";

int main(int argc, const char * argv[]){

  if(argc!=5){
      return -1;
  }

  struct mq_attr attr;

  attr.mq_maxmsg  = 1; /* Max. # of messages on queue */
  attr.mq_msgsize = sizeof(int);/* Max. message size (bytes) */
  attr.mq_flags   = 0;    /* a blocking queue  */

   struct timeval tv;
   double t1;
   double t2;

   mode_t mode = S_IRUSR | S_IWUSR;

   gettimeofday(&tv, NULL);
   t1 = tv.tv_sec + tv.tv_usec/1000000.0;

   numbProd = atoi(argv[1]);
   arraySize = atoi(argv[2]);
   producer = atoi(argv[3]);
   consumer = atoi(argv[4]);

   int i;
   mqd_t counter;

   counter  = mq_open(counterName, O_RDWR | O_CREAT, mode, &attr);

   if (counter == -1 ) {
    perror("mq_open() failed");
    exit(1);
  }

   if(mq_send(counter, (char *)&consumed, sizeof(int), 0) == -1) {
      perror("mq_send() failed");
    }
    

  for(i=0; i<producer; i++){

    char str[15];
    sprintf(str, "%d", i);

    char * arg_list [] ={"sender.out", str, argv[1], argv[2], argv[3], NULL};
    pid_t id= fork();
      
    if(id < 0){
      return -1;
    }

    if(id==0){
      execvp("./sender.out", arg_list);
    }
  }

    for(i=0; i<consumer; i++){

      char str[15];
      sprintf(str, "%d", i);

      char * arg_list [] ={"reciever.out", str, argv[1], argv[2], NULL};
      pid_t id= fork();
      
      if(id < 0){
        return -1;
      }

      if(id ==0){
        execvp("./reciever.out", arg_list);
      }
    }

  for (i = 0; i < producer + consumer; i++) {//wait for both processes to finish
     wait(&child_status);
  }

  gettimeofday(&tv, NULL);
  t2 = tv.tv_sec + tv.tv_usec/1000000.0;
  printf("System execution time: %.6lf seconds\n", t2-t1);

  if (mq_unlink(qname) != 0) { //unlink both queues
    perror("mq_unlink() failed");
    exit(3);
  }

  if (mq_unlink(counterName) != 0) {//unlink both queues
    perror("mq_unlink() failed");
    exit(3);
  }

  return 0;
}
