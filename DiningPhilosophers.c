#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

// Temos N filosofos e N garfos. Cada garfo recebera uma numeraçao.
// Cada filosofo ira pegar, primeiro, o garfo de menor numero e depois o garfo de maior numero a sua disposiçao.
// Para os N-1 primeiros filosofos, o garfo de menor numero estara a direita.
// Para o filosofo que restou, o garfo de menor numero estara a esquerda.

#define N 5 // numero de filosofos

#define DIR (f_num) // expressao para o garfo da direita
#define ESQ ((f_num + 1)%N) // expressao para o garfo da esquerda

sem_t garfos[N]; // semaforos para controlar acesso a cada garfo

void *filosofo(void *num);
void pega_garfos(int);
void devolve_garfos(int);
int getRandomNumber(int, int);

int fil_num[N];

#define COLOR_GREEN "\x1b[32m"
#define COLOR_RESET "\x1b[0m"

int main() {
	
    int f_num;
    for (f_num = 0; f_num < N; f_num++)
        printf(COLOR_RESET "Filosofo %d precisa dos garfos %d e %d para comer\n", f_num + 1, DIR + 1, ESQ + 1);

    int i;
    pthread_t thread_id[N]; // cada thread simula o comportamento de um filosofo

    for (i = 0; i < N; i++) // inicializa um semaforo para cada garfo
        sem_init(&garfos[i], 0, 1);

    for (i = 0; i < N; i++) { // inicializa as threads de cada filosofo
        fil_num[i] = i;
        printf("Filosofo %d criado\n", fil_num[i] + 1);
        pthread_create(&thread_id[i], NULL, filosofo, &fil_num[i]);
    }

    for (i = 0; i < N; i++)
        pthread_join(thread_id[i], NULL);
}

void *filosofo(void *num) {
	
    int i = *((int*)num);
    
    while (1) {
        printf("Filosofo %d esta pensando\n", i + 1);
		    sleep(getRandomNumber(3, 1));
        pega_garfos(i);
        printf(COLOR_GREEN "Filosofo %d esta comendo\n" COLOR_RESET, i + 1);
        sleep(getRandomNumber(3, 1));
        devolve_garfos(i);
    }
}

void pega_garfos(int f_num) {

    printf("Filosofo %d esta com fome\n", f_num + 1);

    if (f_num + 1 == N) { // para esse filosofo o garfo de menor numero estara na esquerda

      // espera pelo garfo de menor numero
      printf("Filosofo %d espera pelo garfo %d\n", f_num + 1, ESQ + 1);
      sem_wait(&garfos[ESQ]);
      // pega garfo de menor numero
      printf("Filosofo %d pegou o garfo %d\n", f_num + 1, ESQ + 1);
      // espera pelo garfo de maior numero
      printf("Filosofo %d espera pelo garfo %d\n", f_num + 1, DIR + 1);
      sem_wait(&garfos[DIR]);
      // pega garfo de maior numero
      printf("Filosofo %d pegou os garfos %d e %d\n", f_num + 1, DIR + 1, ESQ + 1);
    }

    else { // para os outros filosofos o garfo de menor numero estara na direita

      // espera pelo garfo de menor numero
      printf("Filosofo %d espera pelo garfo %d\n", f_num + 1, DIR + 1);
      sem_wait(&garfos[DIR]);
      // pega garfo de menor numero
      printf("Filosofo %d pegou o garfo %d\n", f_num + 1, DIR + 1);
      // espera pelo garfo de maior numero
      printf("Filosofo %d espera pelo garfo %d\n", f_num + 1, ESQ + 1);
      sem_wait(&garfos[ESQ]);
      // pega garfo de maior numero
      printf("Filosofo %d pegou os garfos %d e %d\n", f_num + 1, DIR + 1, ESQ + 1);
    }
}

void devolve_garfos(int f_num) {
	
    printf("Filosofo %d devolveu os garfos %d e %d\n", f_num + 1, DIR + 1, ESQ + 1);
    // devolve garfo da esquerda
    sem_post(&garfos[ESQ]);
    // devolve garfo da direita
    sem_post(&garfos[DIR]);
}

int getRandomNumber(int max, int min) { // retorna um inteiro aleatorio no intervalo [min,max]

  int result = (rand() % (max + 1));
  if (result < min) result = min;
  return result;
}
