#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define N 5

sem_t barberReady;
sem_t accessWRSeats;
sem_t custReady;

int numberOfFreeWRSeats = N;

void *barber() {
  while(1) {
    sem_wait(&custReady);
    sem_wait(&accessWRSeats);
    numberOfFreeWRSeats += 1;
    // sleep(2);
    printf("%d lugares livres\n", numberOfFreeWRSeats);
    sem_post(&barberReady);
    sem_post(&accessWRSeats);
    printf("cliente %d, teve cabelo cortado.\n", N - numberOfFreeWRSeats);
    printf("Corte o cabelo aqui.\n");
  }
}

void *customer() {
  while(1) {
    sem_wait(&accessWRSeats);
    if(numberOfFreeWRSeats > 0) {
      numberOfFreeWRSeats -= 1;
      sem_post(&custReady);
      sem_post(&accessWRSeats);
      // sleep(2);
      sem_wait(&barberReady);
      printf("Cortou o cabelo.\n");
      sleep(2);
    } else {
      sleep(1);
      sem_post(&accessWRSeats);
      printf("Sai sem cortar o cabelo.\n");
    }
  }
}

int main() {
  sem_init(&barberReady, 0, 0);
	sem_init(&accessWRSeats, 0, 1);
	sem_init(&custReady, 0, 0);

	pthread_t threads[N +2];
  pthread_create(&threads[0], NULL, barber, NULL);
  for(int i = 1; i <= N+1; i++) {
    pthread_create(&threads[i], NULL, customer, NULL);
  }

	for(int i = 0; i <= N+1; i++) {
		pthread_join(threads[i], NULL);
	}

  return 0;
}
