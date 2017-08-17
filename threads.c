#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/time.h>

int numbProd;
int arraySize;
int producer;
int consumer;
int cRead=0;

struct circqueue* num_q;

pthread_mutex_t mutex;
sem_t spaces;
sem_t items;
 

////////////////////////////Circular queue Implementation Online//////////////////////
struct circqueue {
   int front,rear;
   int capacity;
   int *array;
};


struct circqueue *q(int size) {
   struct circqueue *q=malloc(sizeof(struct circqueue));
   if(!q)return NULL;
   q->capacity=size;
   q->front=-1;
   q->rear=-1;
   q->array=malloc(q->capacity*sizeof(int));
   if(!q->array)return NULL;
  return q;
}

int isemptyqueue(struct circqueue *q) {
   return(q->front==-1);
}

int isfullqueue(struct circqueue *q) {
   return((q->rear+1)%q->capacity==q->rear);
}

int queuesize(struct circqueue *q) {
   return(q->capacity-q->rear+q->front+1)%q->capacity;
}


void enqueue(struct circqueue *q,int x) {

   if(isfullqueue(q))
      printf("queue overflow\n");
   else{
      q->rear=(q->rear+1)%q->capacity;
      q->array[q->rear]=x;
      if(q->front==-1) {
         q->front=q->rear;
      }
   }
}

int dequeue(struct circqueue *q) {
   int data=0;

   if(isemptyqueue(q)) {
      printf("queue underflow");
      return 0;
   }
   else {
      data=q->array[q->front];
      if(q->front==q->rear)
         q->front=q->rear=-1;
      else
         q->front=(q->front+1)%q->capacity;
   }

   return data;
}
////////////////////////////Circular queue Implementation Online//////////////////////

void* produce(void * index){
   int * id = (int *) index;

   while(*id < numbProd){

      sem_wait(&spaces);
      pthread_mutex_lock(&mutex);
      enqueue(num_q, *id);
      *id+=producer;
      pthread_mutex_unlock(&mutex);
      sem_post(&items);
   }

   free(index);

   pthread_exit(0);

}

void* consume(void* index){
   int * id = (int *) index;

   while(1){

      pthread_mutex_lock(&mutex);//use the same mutex
      if(cRead==numbProd){
         free(id);//free the id passed to it
         pthread_mutex_unlock(&mutex);
         pthread_exit(0);
      }
      cRead++;
      pthread_mutex_unlock(&mutex);

      sem_wait(&items);
      pthread_mutex_lock(&mutex);
      int val= dequeue(num_q);
      int root =(int) sqrt(val);

      if(root*root==val){
         printf("%d %d %d\n", *id, val, root);
      }

      pthread_mutex_unlock(&mutex);
      sem_post(&spaces);

   }

   pthread_exit(0);

}

int main(int argc, const char * argv[]){

   if(argc!=5){
      return -1;
   }

   struct timeval tv;
   double t1;
   double t2;

   gettimeofday(&tv, NULL);
   t1 = tv.tv_sec + tv.tv_usec/1000000.0;

   numbProd = atoi(argv[1]);
   arraySize = atoi(argv[2]);
   producer = atoi(argv[3]);
   consumer = atoi(argv[4]);

   num_q = q(arraySize);

   pthread_t pthreadc[consumer];
   pthread_t pthreadp[producer];

   pthread_mutex_init(&mutex, NULL);
   sem_init(&spaces, 0 , arraySize);
   sem_init(&items, 0 , 0);


   int i;

   for(i=0; i<producer; i++){//create consumers
         int * indexp = malloc(sizeof(int));
         *indexp=i;

         pthread_create(&pthreadp[i], NULL, produce, indexp);
   }

   for(i=0; i<consumer; i++){//create producers
         int * indexc = malloc(sizeof(int));
         *indexc=i;
        
         pthread_create(&pthreadc[i], NULL, consume, indexc);
   }

   for(i=0; i<producer; i++){
      pthread_join(pthreadp[i], NULL);
   }

   for(i=0; i<consumer; i++){
      pthread_join(pthreadc[i], NULL);
   }

   gettimeofday(&tv, NULL);
   t2 = tv.tv_sec + tv.tv_usec/1000000.0;
   printf("System execution time: %.6lf seconds\n", t2-t1);

   free( num_q->array );
   free( num_q );

   pthread_mutex_destroy(&mutex);
   sem_destroy(&spaces);
   sem_destroy(&items);

   return 0;

}



























