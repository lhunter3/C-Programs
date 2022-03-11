#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>



typedef struct node{

  char username[100];
  char processName;
  int arrival;
  int duration;
  struct node * next;

} Node;


/**
enqueue function @param Node** head, char username[100], char processName, int arrival, int duration
Adds to back of list
**/
void enqueue(Node** head, char username[100], char processName, int arrival, int duration){

  Node * newNode = (Node*) malloc(sizeof(Node));
  Node * current = *head;

  strcpy(newNode->username, username);
  newNode->processName = processName;
  newNode->arrival = arrival;
  newNode->duration = duration;
  newNode->next = NULL;

  if(*head == NULL){
    *head = newNode;
  }
  else{
    while(current->next != NULL){
      current = current->next;
    }
    current->next = newNode;
  }
}

/**
printAll function @param Node* head
Prints queue
**/
void printAll(Node * head){

  Node *current = head;
  printf("\nSummary\n");
  while (current != NULL){
    printf("%s\t%d\n", current->username,current->duration-1);
    current = current->next;
  }
}

/**
delete function @param Node** head, char processName
Used to remove processes from scheduling Queue.
**/
void delete(Node** head, char processName){

  Node * current = *head;
  Node * previous = NULL;

//Check if the value we want to delete is the head
  if(current != NULL && current->processName == processName){
    *head = current->next;
    free(current);

  }
  else{
    //look for value we want to delete, keep track of previous node
    while(current != NULL && current->processName != processName){
      previous = current;
      current = current->next;
    }
      //remove node & free memory
      if(current != NULL){
        previous->next = current->next;
        free(current);
      }

  }

}

/**
contains function @param Node* head, char * username
Used to check if user already when adding to summary queue.
**/
int contains(Node* head, char * username){

  Node * current = head;

  while(current != NULL){
    if(strcmp(current->username,username) == 0)
      return 1;

    current = current->next;
  }
  return 0;
}

/**
update function @param Node* head, char * username, int time
Updates queue times.
**/
void update(Node* head, char * username, int time){

  Node * current = head;
  //loop through list, replace with newInput if found, if oldInput is not found do nothing
  while(current != NULL){
    if(strcmp(current->username, username) == 0){
      current->duration = time;
      return;
    }
  current = current->next;
  }
}


/**
helper function @param Node ** schedulingQueue, int timeStamp
finds the shortest process that has arrived. returns char (processName)
**/
char shortestRemainingTimeFirstHelper(Node ** schedulingQueue, int timeStamp){

  int shortestTime = 999;
  char id = ' ';
  Node * current = *schedulingQueue;

  while(current != NULL){

    if(current->duration < shortestTime && current->arrival <= timeStamp){
      shortestTime = current->duration;
      id = current->processName;
    }
    current = current->next;
  }

  return id;
}

/**
shortestRemainingTimeFirst function @param (Node ** schedulingQueue
Does the simulation. Returns the summary Queue.
**/
Node *  shortestRemainingTimeFirst(Node ** schedulingQueue){

Node * summaryQ = NULL;
int time = 1;
char scheduledProcess;
printf("Time\tJob\n");

  while(*schedulingQueue != NULL){

    Node * temp = *schedulingQueue;
    while(temp != NULL){
      if(temp->duration == 0){
        if(contains(summaryQ,temp->username) == 0)
          enqueue(&summaryQ,temp->username,'a',0,time);
        else
          update(summaryQ,temp->username,time);
        delete(schedulingQueue,temp->processName);
      }
      temp = temp->next;
    }

    scheduledProcess = shortestRemainingTimeFirstHelper(schedulingQueue,time);

    if(scheduledProcess == ' ')
      printf("%d\t-\n",time);
    else{
      Node * current = *schedulingQueue;
      while(current != NULL){

        if(current->processName == scheduledProcess){
          printf("%d\t%c\n",time,current->processName);
          update(*schedulingQueue,current->username,current->duration-1);
          break;
        }
        current = current->next;
      }

    }

    sleep(1);
    time++;
  }

return summaryQ;


}


int main(int argc, char** argv){

    Node * schedulingQueue = NULL;

    char buf[100];
    char buf2[100];
    char buf3[100];
    char buf4[100];

    char username[100];
    char processName;
    int arrival;
    int duration;

    scanf("%s %s %s %s \n",buf,buf2,buf3,buf4);
    while (EOF!=scanf("%s %c %d %d",username,&processName,&arrival,&duration)){
        enqueue(&schedulingQueue, username,processName,arrival,duration);
    }

    Node * summary = shortestRemainingTimeFirst(&schedulingQueue);
    printAll(summary);
    return 0;
}
