#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

sem_t esperandoagente;
sem_t tabaco;
sem_t papel;
sem_t fosforo;
sem_t sempusher;


sem_t tabaco_f; //for smoker with tobacco
sem_t papel_f; //for smoker with paper
sem_t fosforo_f; //for smoker with match

int e_tabaco = 0;
int e_papel = 0;
int e_fosforo = 0;

int c_tabaco = 0;
int c_papel = 0;
int c_fosforo = 0;

void *agent_a();
void *agent_b();
void *agent_c();
void *pusher_t();
void *pusher_p();
void *pusher_f();
void *smoker_t();
void *smoker_p();
void *smoker_f();

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"

int main()
{
	int i;
	sem_init(&esperandoagente, 0, 1);
	sem_init(&tabaco, 0, 0);
	sem_init(&papel, 0, 0);
	sem_init(&fosforo, 0, 0);
	sem_init(&tabaco_f, 0, 0);
	sem_init(&papel_f, 0, 0);
	sem_init(&fosforo_f, 0, 0);
  sem_init(&sempusher, 0, 1);
	pthread_t threads[9];
	pthread_create(&threads[0], NULL, smoker_t, NULL);
	pthread_create(&threads[1], NULL, smoker_p, NULL);
	pthread_create(&threads[2], NULL, smoker_f, NULL);
	pthread_create(&threads[3], NULL, pusher_t, NULL);
	pthread_create(&threads[4], NULL, pusher_p, NULL);
	pthread_create(&threads[5], NULL, pusher_f, NULL);
	pthread_create(&threads[6], NULL, agent_a, NULL);
	pthread_create(&threads[7], NULL, agent_b, NULL);
	pthread_create(&threads[8], NULL, agent_c, NULL);

	for(i = 0; i < 9; i++) {
		pthread_join(threads[i], NULL);
	}
	return 0;
}

void *agent_a()
{
	while(1) {
		sem_wait(&esperandoagente);
		printf("agentA: papel e tabaco na mesa.\n");
		//fflush(stdout);
		sem_post(&tabaco);
		sem_post(&papel);
	}
}

void *agent_b()
{
	while(1) {
		sem_wait(&esperandoagente);
		printf("agentB: papel e fosforo na mesa.\n");
		//fflush(stdout);
		sem_post(&papel);
		sem_post(&fosforo);
	}
}

void *agent_c()
{
	while(1) {
		sem_wait(&esperandoagente);
		printf("agentC: tabaco e fosforo na mesa.\n");
		//fflush(stdout);
		sem_post(&tabaco);
		sem_post(&fosforo);
	}
}

void *pusher_t()
{
	while(1) {
		sem_wait(&tabaco);
    sem_wait(&sempusher);
		//printf("pusher_t: ativado.\n");
		if(e_papel) {
			e_papel -= 1;
			sem_post(&fosforo_f);
		}
		else if(e_fosforo) {
			e_fosforo -= 1;
			sem_post(&papel_f);
		}
		else {
			e_tabaco += 1;
		}
		sem_post(&sempusher);
	}
}

void *pusher_p()
{
	while(1) {
		sem_wait(&papel);
		sem_wait(&sempusher);
		//printf("pusher_p: ativado.\n");
		if(e_fosforo) {
			e_fosforo -= 1;
			sem_post(&tabaco_f);
		}
		else if(e_tabaco) {
			e_tabaco -= 1;
			sem_post(&fosforo_f);
		}
		else {
			e_papel += 1;
		}
		sem_post(&sempusher);
	}
}

void *pusher_f()
{
	while(1) {
		sem_wait(&fosforo);
		sem_wait(&sempusher);
		//printf("pusher_f: ativado.\n");
		if(e_papel) {
			e_papel -= 1;
			sem_post(&tabaco_f);
		}
		else if(e_tabaco) {
			e_tabaco -= 1;
			sem_post(&papel_f);
		}
		else {
			e_fosforo += 1;
		}
		sem_post(&sempusher);
	}
}

void *smoker_t()
{
	while(1) {
		sem_wait(&tabaco_f);
		printf(ANSI_COLOR_RED "Smoker T: fuma. %d cigarro\n" ANSI_COLOR_RESET,++c_tabaco);
    sem_post(&esperandoagente);
		sleep(2);
	}
}

void *smoker_p()
{
	while(1) {
		sem_wait(&papel_f);
		printf(ANSI_COLOR_RED "Smoker P: fuma. %d cigarro\n" ANSI_COLOR_RESET,++c_papel);
		sem_post(&esperandoagente);
		sleep(2);
	}
}

void *smoker_f()
{
	while(1) {
		sem_wait(&fosforo_f);
		printf(ANSI_COLOR_RED "Smoker F: fuma. %d cigarro\n" ANSI_COLOR_RESET, ++c_fosforo);
		sem_post(&esperandoagente);
		sleep(2);
	}
}
