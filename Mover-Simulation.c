#include <pthread.h> // for pthread_create(), pthread_join(), etc.
#include <stdio.h> // for scanf(), printf(), etc.
#include <stdlib.h> // for malloc()
#include <unistd.h> // for sleep()

#define NUMBER_OF_BOXES_PER_DWELLER 5
#define ROOM_IN_TRUCK 10
#define MIN_BOX_WEIGHT 5
#define MAX_BOX_WEIGHT 50
#define MAX_TIME_FOR_HOUSE_DWELLER 7
#define MIN_TIME_FOR_HOUSE_DWELLER 1
#define MAX_TIME_FOR_MOVER 3
#define MIN_TIME_FOR_MOVER 1
#define MIN_TIME_FOR_TRUCKER 1
#define MAX_TIME_FOR_TRUCKER 3
#define MIN_TRIP_TIME 5
#define MAX_TRIP_TIME 10




#define RANDOM_WITHIN_RANGE(a,b,seed) (a+rand_r(&seed)%(b-a))

// For pipes
#define READ_END 0
#define WRITE_END 1

// Pipe between house dwellers and movers
int houseFloor[2];
// Pipe between movers and truckers
int nextToTrucks[2];

/*
ARGS struct
*/

typedef struct thread_args{

  int index;
  unsigned int seed;
  int * fd1;
  int * fd2;

} Args;


/*
HOUSE DWELLER THREAD FUNCTION
*/
void* dweller(void* arg){

  Args* argg = (Args*) arg;
  printf("Hello I am house dweller %d\n", argg->index);

  int j;
  for(j = 0; j < NUMBER_OF_BOXES_PER_DWELLER; j++){
    int interval = RANDOM_WITHIN_RANGE (MIN_TIME_FOR_HOUSE_DWELLER ,MAX_TIME_FOR_HOUSE_DWELLER, argg->seed);
    sleep(interval);
    int weight = RANDOM_WITHIN_RANGE(MIN_BOX_WEIGHT,MAX_BOX_WEIGHT,argg->seed);
    printf("House dweller %d created a box that weighs %d in %d units of time\n", argg->index,weight,interval);
    write(houseFloor[WRITE_END],&weight,sizeof(int));

  }
  printf("House dweller %d is done packing\n",argg->index );

  return NULL;
}

/*
MOVER THREAD FUNCTION
*/

void* mover(void* arg){

  Args* argg = (Args*) arg;
  int weight;

  printf("Hello I am house mover %d\n", argg->index);

  while (read(houseFloor[READ_END],&weight,sizeof(int)) != 0){

    int interval = RANDOM_WITHIN_RANGE(MIN_TIME_FOR_MOVER,MAX_TIME_FOR_MOVER,argg->seed);
    sleep(interval);
    printf("Mover %d brought down a box that weighs %d in %d units of time\n",argg->index,weight,interval);
    write(nextToTrucks[WRITE_END], &weight, sizeof(int));
  }
  printf("Mover %d is done moving boxes downstairs\n",argg->index);

  return NULL;

}

/*
TRUCK DRIVER THREAD FUNCTION
*/

void* driver(void* arg){

  Args* argg = (Args*) arg;

  printf("Hello I am house driver %d\n", argg->index);

  int availableSpace = ROOM_IN_TRUCK;
  int weight;
  int totalWeight = 0;

  while (read(nextToTrucks[READ_END],&weight,sizeof(int)) != 0){

    int interval = RANDOM_WITHIN_RANGE(MIN_TIME_FOR_TRUCKER,MAX_TIME_FOR_TRUCKER,argg->seed);
    sleep(interval);
    totalWeight += weight;
    availableSpace--;
    printf("Trucker %d  loaded up a box that weighs %d to the truck, took %d units of time, room left:%d\n",argg->index,weight,interval,availableSpace);

    if(availableSpace == 0){
      int interval = RANDOM_WITHIN_RANGE(MIN_TRIP_TIME,MAX_TRIP_TIME,argg->seed);
      printf("Full truck %d with load of %d units of mass departed, round-trip will take %d\n",argg->index,totalWeight,interval);
      availableSpace = 10;
      totalWeight = 0;
      sleep(interval);
    }
  }

    int interval = RANDOM_WITHIN_RANGE(MIN_TRIP_TIME,MAX_TRIP_TIME,argg->seed);
    printf("Not Full truck with load of %d units of mass departed, one way trip will take %d\n",totalWeight,interval);
    sleep(interval);
    printf("Trucker %d is finished.\n",argg->index);
  return NULL;

}



int main(int argc, char** argv){

  /*
  SETUP & INPUT
  */

    srand(time(NULL));

    pipe(houseFloor);
    pipe(nextToTrucks);

    int dwellers,movers,drivers;
    printf("Please input number of people living in the house, number of movers and number of truck drivers\n");
    scanf("%d %d %d", &dwellers,&movers,&drivers);

    pthread_t dwellerThread[dwellers];
    pthread_t moverThread[movers];
    pthread_t driverThread[drivers];

  /*
  CREATING 3 TYPES OF THREADS (DWERLLER,MOVER,DRIVER)
  */

    int i;

    for(i = 0; i < dwellers; i++){
      Args* argg = (Args*)malloc(sizeof(Args));
      argg->fd1 = houseFloor;
      argg->fd2 = nextToTrucks;
      argg->index = i;
      argg->seed = rand();
      pthread_create(&(dwellerThread[i]),NULL,&dweller,(void*)argg);

    }



    for(i = 0; i < movers; i++){
      Args* argg = (Args*)malloc(sizeof(Args));
      argg->fd1 = houseFloor;
      argg->fd2 = nextToTrucks;
      argg->index = i;
      argg->seed = rand();

      pthread_create(&(moverThread[i]),NULL,&mover,(void*)argg);
    }



    for(i = 0; i < drivers; i++){
      Args* argg = (Args*)malloc(sizeof(Args));
      argg->fd1 = houseFloor;
      argg->fd2 = nextToTrucks;
      argg->index = i;
      argg->seed = rand();

      pthread_create(&(driverThread[i]),NULL,&driver,(void*)argg);
    }

  /*
  --- WAITING FOR THREADS ---
  --- CLOSING PIPES ---
  */

    for(i = 0; i < dwellers; i++)
      pthread_join(dwellerThread[i],NULL);

    close(houseFloor[WRITE_END]);

    for(i = 0; i < movers; i++)
      pthread_join(moverThread[i],NULL);

    close(houseFloor[READ_END]);
    close(nextToTrucks[WRITE_END]);

    for(i = 0; i < drivers; i++)
      pthread_join(driverThread[i],NULL);

    close(nextToTrucks[READ_END]);
    printf("Moving is finished!\n");


    return 0;
}
