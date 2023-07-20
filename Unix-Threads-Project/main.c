//Course: CSE4034 (2022) - Advanced Unix Programming
//Project 2 (Thread)
//Ali Yetim 150119803
//Cem Anaral 150119761


#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER 9999999

// Struct to represent a car
struct Car {
  pthread_mutex_t lock;
  int id;
  int chassisOn;
  int tiresOn;
  int seatsOn;
  int engineOn;
  int topCoverOn;
  int paintOn;
  int isDone;
};
struct Car cars[BUFFER];

int numOfTypeA, numOfTypeB, numOfTypeC, numOfTypeD, maxSimulationDay,
    currentSimulationDay;
int chassisLimit, paintLimit, tireLimit, seatLimit, engineLimit, topCoverLimit;

sem_t sem_chassis, sem_top_cover, sem_paint, sem_tires, sem_seats, sem_engine;



int main() {

  FILE *input_file = fopen("input.txt", "r");
  if (input_file == NULL) {
    perror("ERROR");
    return 1;
  }
  fscanf(input_file, "%d %d %d %d %d", &numOfTypeA, &numOfTypeB, &numOfTypeC,
         &numOfTypeD, &maxSimulationDay);
  fscanf(input_file, "%d %d %d %d %d %d", &chassisLimit, &paintLimit,
         &tireLimit, &seatLimit, &engineLimit, &topCoverLimit);
  fclose(input_file);

  sem_init(&sem_chassis, 0, chassisLimit);
  sem_init(&sem_tires, 0, tireLimit);
  sem_init(&sem_seats, 0, seatLimit);
  sem_init(&sem_engine, 0, engineLimit);
  sem_init(&sem_top_cover, 0, topCoverLimit);
  sem_init(&sem_paint, 0, paintLimit);

  printf("<Technician_ID Car_dID Operation Simulation_Day>\n\n");
  
  void *typeACallFunc(void *arg);
  void *typeBCallFunc(void *arg);
  void *typeCCallFunc(void *arg);
  void *typeDCallFunc(void *arg);

  for (int i = 0; i < BUFFER; i++) {
    pthread_mutex_init(&cars[i].lock, NULL);
  }

  pthread_t typeATh[numOfTypeA], typeBTh[numOfTypeB], typeCTh[numOfTypeC],
      typeDTh[numOfTypeD];
  int typeAIds[numOfTypeA], typeBIds[numOfTypeB], typeCIds[numOfTypeC],
      typeDIds[numOfTypeD];
  for (int i = 0; i < numOfTypeA; i++) {
    typeAIds[i] = i;
    pthread_create(&typeATh[i], NULL, typeACallFunc, &typeAIds[i]);
  }

  for (int i = 0; i < numOfTypeB; i++) {
    typeBIds[i] = i;
    pthread_create(&typeBTh[i], NULL, typeBCallFunc, &typeBIds[i]);
  }

  for (int i = 0; i < numOfTypeC; i++) {
    typeCIds[i] = i;
    pthread_create(&typeCTh[i], NULL, typeCCallFunc, &typeCIds[i]);
  }

  for (int i = 0; i < numOfTypeD; i++) {
    typeDIds[i] = i;
    pthread_create(&typeDTh[i], NULL, typeDCallFunc, &typeDIds[i]);
  }

  for (int i = 1; i < maxSimulationDay + 1; i++) {
    currentSimulationDay = i;
    printf("DAY %d\n", currentSimulationDay);

    for (int j = 0; j < chassisLimit; j++) {
      sem_post(&sem_chassis);
    }
    for (int j = 0; j < seatLimit; j++) {
      sem_post(&sem_seats);
    }
    for (int j = 0; j < engineLimit; j++) {
      sem_post(&sem_engine);
    }
    for (int j = 0; j < tireLimit; j++) {
      sem_post(&sem_tires);
    }
    for (int j = 0; j < topCoverLimit; j++) {
      sem_post(&sem_top_cover);
    }
    for (int j = 0; j < paintLimit; j++) {
      sem_post(&sem_paint);
    }
    sleep(3);
  }

  int num_cars = 0;
  for (int i = 0; i < BUFFER; i++) {
    if (cars[i].paintOn) {
      num_cars++;
    }
  }
  printf("Number of created cars: %d\n", num_cars);

  sem_destroy(&sem_chassis);
  sem_destroy(&sem_tires);
  sem_destroy(&sem_seats);
  sem_destroy(&sem_engine);
  sem_destroy(&sem_top_cover);
  for (int i = 0; i < BUFFER; i++) {
    pthread_mutex_destroy(&cars[i].lock);
  }

  return 0;
}


void *typeACallFunc(void *arg) {

  int id = *((int *)arg);
  int carID;

  while (1) {

    for (carID = 0; carID < BUFFER; carID++) {
      if (pthread_mutex_trylock(&cars[carID].lock) == 0) {
        if (cars[carID].chassisOn && !cars[carID].tiresOn) {

          sem_wait(&sem_tires);
          usleep(300);

          cars[carID].tiresOn = 1;
          printf("Type A - %d		%d		tires		%d\n",
                 id, carID + 1, currentSimulationDay);
        }
      }

      if (cars[carID].topCoverOn && !cars[carID].paintOn) {

        sem_wait(&sem_paint);
        usleep(300);
        cars[carID].paintOn = 1;
        cars[carID].isDone = 1;

        printf("Type A - %d		%d		paint		%d\n",
               id, carID + 1, currentSimulationDay);
      }
      pthread_mutex_unlock(&cars[carID].lock);
    }
    if (carID == BUFFER) {
      continue;
    }
  }
}


void *typeBCallFunc(void *arg) {

  int id = *((int *)arg);
  int carID;

  while (1) {

    for (carID = 0; carID < BUFFER; carID++) {
      if (pthread_mutex_trylock(&cars[carID].lock) == 0) {
        if (!cars[carID].chassisOn &&
            currentSimulationDay != maxSimulationDay) {

          sem_wait(&sem_chassis);
          usleep(300);
          cars[carID].chassisOn = 1;
          printf("Type B - %d		%d		chassis		%d\n",
                 id, carID + 1, currentSimulationDay);
        }
        pthread_mutex_unlock(&cars[carID].lock);
      }
    }
    if (carID == BUFFER) {
      continue;
    }
  }
}


void *typeCCallFunc(void *arg) {

  int id = *((int *)arg);
  int carID;

  while (1) {

    for (carID = 0; carID < BUFFER; carID++) {
      if (pthread_mutex_trylock(&cars[carID].lock) == 0) {
        if (cars[carID].chassisOn && !cars[carID].seatsOn) {

          sem_wait(&sem_seats);
          usleep(300);
          cars[carID].seatsOn = 1;
          printf("Type C - %d		%d		seats		%d\n",
                 id, carID + 1, currentSimulationDay);
        }
        pthread_mutex_unlock(&cars[carID].lock);
      }
    }
    if (carID == BUFFER) {

      continue;
    }
  }
}


void *typeDCallFunc(void *arg) {

  int id = *((int *)arg);
  int carID;

  while (1) {

    for (carID = 0; carID < BUFFER; carID++) {
      if (pthread_mutex_trylock(&cars[carID].lock) == 0) {
        if (cars[carID].chassisOn && !cars[carID].engineOn) {

          sem_wait(&sem_engine);
          usleep(300);
          cars[carID].engineOn = 1;
          printf("Type D - %d		%d		engine		%d\n",
                 id, carID + 1, currentSimulationDay);
        }
      }

      if (cars[carID].tiresOn && cars[carID].seatsOn && cars[carID].engineOn &&
          !cars[carID].topCoverOn) {

        sem_wait(&sem_top_cover);
        usleep(300);
        cars[carID].topCoverOn = 1;
        printf("Type D - %d		%d		top cover	%d\n",
               id, carID + 1, currentSimulationDay);
      }
      pthread_mutex_unlock(&cars[carID].lock);
    }
    if (carID == BUFFER) {
      continue;
    }
  }
}
