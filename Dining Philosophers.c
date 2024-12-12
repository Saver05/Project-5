//
// Created by jfram on 11/12/2024.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#include "common.h"
#include "common_threads.h"
#include "zemaphore.h"

int N;
int algorithm;
int *fork_available;
pthread_mutex_t lock;
Zem_t *forks;
Zem_t table_limit;

pthread_t *philosophers;

static void think(int p) {
    printf("Philosopher %d is thinking\n", p);
    Spin(1);
}

static void eat() {
    Spin(1);
}

//*********************************************************************************
//Algorithm 1

void alg1_pickup_forks(int p) {
    int left = p;
    int right = (p+1) % N;

    Sem_wait(&forks[left]);

    Sem_wait(&forks[right]);
}

void alg1_putdown_forks(int p) {
    int left = p;
    int right = (p+1) % N;
    Sem_post(&forks[left]);
    Sem_post(&forks[right]);
}

//*********************************************************************************
//Algorithm 2

void alg2_pickup_forks(int p) {
    int left = p;
    int right = (p+1) % N;

    Mutex_lock(&lock);

    while (fork_available[left] ==0 && fork_available[right] ==0) {
        Mutex_unlock(&lock);
        sleep(1);
        Mutex_lock(&lock);
    }

    fork_available[left] = 0;
    fork_available[right] = 0;
    Mutex_unlock(&lock);
}

void alg2_putdown_forks(int p) {
    int left = p;
    int right = (p+1) % N;

    Mutex_lock(&lock);
    fork_available[left] = fork_available[right] = 1;
    Mutex_unlock(&lock);
}

//*********************************************************************************
//Algorithm 3

void alg3_pickup_forks(int p) {
    int left = p;
    int right = (p+1) % N;

    Sem_wait(&table_limit);

    Sem_wait(&forks[left]);
    Sem_wait(&forks[right]);
}

void alg3_putdown_forks(int p) {
    int left = p;
    int right = (p+1) % N;

    Sem_post(&forks[left]);
    Sem_post(&forks[right]);

    Sem_post(&table_limit);
}

//*********************************************************************************

void *philosopher(void *arg) {
    int p = (int)(long)arg;

    while (1) {
        think(p);
        printf("Philosopher %d is picking up their fork\n", p);
        switch (algorithm) {
            case 1:
                alg1_pickup_forks(p);
            break;
            case 2:
                alg2_pickup_forks(p);
            break;
            case 3:
                alg3_pickup_forks(p);
            break;
            default:
                fprintf(stderr, "Unknown algorithm!\n");
            exit(1);
        }

        printf("Philosopher %d is picking up eating\n", p);
        eat();
        printf("Philosopher %d is putting down their fork\n", p);
        switch (algorithm) {
            case 1:
                alg1_putdown_forks(p);
            break;
            case 2:
                alg2_putdown_forks(p);
            break;
            case 3:
                alg3_putdown_forks(p);
            break;
        }
        return NULL;
    }
}


int main (int argc, char *argv[]){
    if (argc != 3) {
        fprintf(stderr, "usage: %s N algorithm(1/2/3)\n", argv[0]);
        exit(1);
    }

    N = atoi(argv[1]);
    algorithm = atoi(argv[2]);

    if (N<3 || N>20) {
        fprintf(stderr, "N must be between 1 and 20\n");
        exit(1);
    }

    if (algorithm >3 || algorithm < 1) {
        fprintf(stderr, "Algorithm choice must be between 1 and 3\n");
        exit(1);
    }

    forks = malloc(sizeof(Zem_t) * N);
    philosophers = malloc(sizeof(pthread_t) * N);

    for (int i = 0; i < N; i++) {
        Sem_init(&forks[i], 1);
    }

    if (algorithm == 2) {
        Mutex_init(&lock);
        fork_available = malloc(sizeof(int) * N);
        for (int i = 0; i < N; i++) {
            fork_available[i] = 1;
        }
    }

    if (algorithm == 3) {
        Zem_init(&table_limit, N-1);
    }

    for (int i = 0; i < N; i++) {
        Pthread_create(&philosophers[i], NULL, philosopher, (void *)(long)i);
    }

    for (int i =0; i < N; i++) {
        Pthread_join(philosophers[i], NULL);
    }

    return 0;
}