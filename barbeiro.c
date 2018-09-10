#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <queue>

// using namespace std;

#define LUGARES 5
#define CLIENTES 10

sem_t clientes[CLIENTES];
sem_t barberReady;
sem_t accessWRSearts;
sem_t custReady;

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int c_cortes[CLIENTES];

std::queue<int> filaDeEspera;

int numberOfFreeWRSeats = LUGARES;

void *barber(void *) {
  while(1) {
    sem_wait(&custReady);
    sem_wait(&accessWRSearts);
    sem_post(&barberReady);
    int i = filaDeEspera.front();
    sem_post(&clientes[i]);
    filaDeEspera.pop();
    numberOfFreeWRSeats += 1;
    sem_post(&accessWRSearts);
    //printf("%d lugares livres\n", numberOfFreeWRSeats);
    printf("Cara %d esta cortando o cabelo. \n", i);
    sleep(1);
    printf(ANSI_COLOR_GREEN "cliente %d, teve cabelo cortado.\n" ANSI_COLOR_RESET, i);
    c_cortes[i] += 1;
    //printf("Corte o cabelo aqui.\n");
  }
}

void *customer(void *i) {
  int id = *((int *)i);
  while(1) {
    sem_wait(&accessWRSearts);
    if(numberOfFreeWRSeats > 0) {
      numberOfFreeWRSeats -= 1;
      filaDeEspera.push(id-1);
      printf("O clinte %d entrou na fila de espera.\n", id-1);
      sem_post(&custReady);
      sem_post(&accessWRSearts);
      // sleep(2);
      sem_wait(&clientes[id-1]);
      sem_wait(&barberReady);
      //printf("Cortou o cabelo.\n");
      printf(ANSI_COLOR_RED "thread %d cortou o cabelo %d vezes.\n" ANSI_COLOR_RESET, id-1, c_cortes[id-1]);
      sleep(1);
    } else {
      sem_post(&accessWRSearts);
      printf("%d Sai sem cortar o cabelo.\n",id-1);
      sleep(1);
    }
  }
}

int main() {
  sem_init(&barberReady, 0, 0);
	sem_init(&accessWRSearts, 0, 1);
	sem_init(&custReady, 0, 0);

  for(int i = 0; i < CLIENTES; i++) {
    sem_init(&clientes[i], 0, 0);
  }

  int nums[CLIENTES+1];
  for(int i = 0; i <= CLIENTES; i++) {
    nums[i] = i;
    c_cortes[i] = 0;
  }

  pthread_t threads[CLIENTES+1];
  pthread_create(&threads[0], NULL, barber, NULL);
  for(int i = 1; i <= CLIENTES; i++) {
    pthread_create(&threads[i], NULL, customer, &nums[i]);
  }

	for(int i = 0; i <= CLIENTES; i++) {
		pthread_join(threads[i], NULL);
	}

  return 0;
}
