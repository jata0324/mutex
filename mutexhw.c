#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_exist = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_enter = PTHREAD_COND_INITIALIZER;

char text[20][10];
int val;
int pro_cnt=0, cons_cnt=0;
int count=0;
char buf[10];
char numtoc[4];
char buffer[30];

void *Provider(void* arg)
{
  for(pro_cnt=0; pro_cnt<100; pro_cnt++){
    pthread_mutex_lock(&mutex);

    if(count>=20){
      pthread_cond_signal(&cond_exist);
      pthread_cond_wait(&cond_empty, &mutex);
      
    }
  
    strcpy(buffer, "msg");
    sprintf(numtoc, "%d", pro_cnt%20);
    strcat(buffer, numtoc);
    printf("Pro %s\n", buffer);
    strcpy(text[pro_cnt%20], buffer);

    count++;

    

    pthread_mutex_unlock(&mutex);
    
    sleep(1);
  }
}

void *Cons1(void* arg)
{
  while(cons_cnt<100){
    pthread_mutex_lock(&mutex);
    
    if(count<=15){
      pthread_cond_signal(&cond_empty);
      pthread_cond_wait(&cond_exist, &mutex);      
    }
    printf("Cons1 %s(every 2 second)\n", text[cons_cnt%20]);
    cons_cnt++;
    count--;

   

    pthread_mutex_unlock(&mutex);
    sleep(2);
  }
}

void *Cons2(void* arg)
{
  while(cons_cnt<100){
  pthread_mutex_lock(&mutex);

  
  pthread_cond_wait(&cond_enter, &mutex);

  if(count==0)
    pthread_cond_wait(&cond_exist, &mutex);
 
  printf("Cons2 %s(enter key)\n", text[cons_cnt%20]);
  count--;
  cons_cnt++;

  if(count<=15)
    pthread_cond_signal(&cond_empty);

  pthread_mutex_unlock(&mutex);
  sleep(1);
  }
}

int main(void){
  pthread_t thread[3];
 
  /*pthread_attr_t attr;
  
  struct sched_param param;
  int policy;

  pthread_getschedparam(pthread_self(), &policy, &param);
  param.sched_priority = 1;
  policy = SCHED_RR;
  pthread_setschedparam(pthread_self(), policy, &param);
  pthread_attr_init(&attr);
  pthread_attr_setschedpolicy(&attr, SCHED_RR);
  */
  pthread_create(&thread[0], NULL, Provider, NULL);
  pthread_create(&thread[1], NULL, Cons1, NULL);
  pthread_create(&thread[2], NULL, Cons2, NULL);

  while(cons_cnt<100){
    fgets(buf, 10, stdin);
    
    if(!strcmp(buf, "\n")){
      pthread_cond_signal(&cond_enter);
    }
    
  }

  pthread_exit(0);
  return 0;
}
