#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "common_threads.h"
#include "zemaphore.h"
#include "common.h"

#define MAX_CUSTOMERS 100
#define WAITING_CHAIRS 15
#define NUM_BARBERS 5

typedef struct {
    int customer_id;
    int barber_id;
} CustomerInfo;

int waiting_customers = 0;
Zem_t mutex;
Zem_t *customer_ready;
Zem_t *barber_ready;
Zem_t waiting_room_seats;
int current_customer[NUM_BARBERS];

void cut_hair(int barber_id, int customer_id) {
    printf("Barber %d is cutting customer %d's hair\n", barber_id, customer_id);
    Spin(1);
}

void get_haircut(int customer_id) {
    printf("Customer %d is getting haircut\n", customer_id);
    Spin(1);
}

void *barber(void *arg) {
    int barber_id = *(int*)arg;

    while (1) {
        Zem_wait(&customer_ready[barber_id]);

        Zem_wait(&mutex);
        waiting_customers--;
        Zem_post(&mutex);

        cut_hair(barber_id, current_customer[barber_id]);

        Zem_post(&barber_ready[barber_id]);

        Zem_post(&waiting_room_seats);
    }
    return NULL;
}

void *customer(void *arg) {
    CustomerInfo *info = (CustomerInfo*)arg;
    int customer_id = info->customer_id;
    int assigned_barber = -1;

    Zem_wait(&waiting_room_seats);

    Zem_wait(&mutex);
    waiting_customers++;

    for (int i = 0; i < NUM_BARBERS; i++) {
        if (barber_ready[i].value > 0) {
            Zem_wait(&barber_ready[i]);
            info->barber_id = i;
            assigned_barber = i;
            current_customer[i] = customer_id;
            break;
        }
    }

    if (assigned_barber == -1) {
        waiting_customers--;
        Zem_post(&waiting_room_seats);
        Zem_post(&mutex);
        printf("Customer %d leaves: no barbers available\n", customer_id);
        free(info);
        return NULL;
    }

    Zem_post(&customer_ready[assigned_barber]);
    Zem_post(&mutex);

    Zem_wait(&barber_ready[assigned_barber]);

    get_haircut(customer_id);

    free(info);
    return NULL;
}

int main() {
    pthread_t barber_threads[NUM_BARBERS];
    pthread_t customer_threads[MAX_CUSTOMERS];
    int barber_ids[NUM_BARBERS];

    Zem_init(&mutex, 1);
    Zem_init(&waiting_room_seats, WAITING_CHAIRS);

    customer_ready = malloc(NUM_BARBERS * sizeof(Zem_t));
    barber_ready = malloc(NUM_BARBERS * sizeof(Zem_t));

    for (int i = 0; i < NUM_BARBERS; i++) {
        Zem_init(&customer_ready[i], 0);
        Zem_init(&barber_ready[i], 1);
        current_customer[i] = -1;

        barber_ids[i] = i;
        Pthread_create(&barber_threads[i], NULL, barber, &barber_ids[i]);
    }

    for (int i = 0; i < MAX_CUSTOMERS; i++) {
        CustomerInfo *info = malloc(sizeof(CustomerInfo));
        info->customer_id = i;
        Pthread_create(&customer_threads[i], NULL, customer, info);

        usleep(rand() % 1000000);
    }

    for (int i = 0; i < MAX_CUSTOMERS; i++) {
        Pthread_join(customer_threads[i], NULL);
    }

    free(customer_ready);
    free(barber_ready);

    return 0;
}