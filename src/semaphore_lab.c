#include "semaphore_lab.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static sem_t output_lock;

void *worker(void *arg) {
    char token = *(char *)arg;
    int index;

    //fprintf(stderr, "TODO: semaphore critical section not implemented\n");
    (void)token;
    (void)index;
    return NULL;

    for (index = 0; index < 10; index++) {
        /* TODO: enter the critical section with sem_wait(). */
        if (sem_wait(&output_lock) == -1) {
            perror("sem_wait");
            return NULL;
        }

        printf("[%c", token);
        fflush(stdout);
        usleep(1000);
        printf("]");
        fflush(stdout);

        /* TODO: leave the critical section with sem_post(). */
        if (sem_post(&output_lock) == -1) {
            perror("sem_post");
            return NULL;
        }
    }

    return NULL;
}

int main(void) {
    pthread_t first_thread;
    pthread_t second_thread;
    char first_token = 'A';
    char second_token = 'B';

    if (sem_init(&output_lock, 0, 1) == -1) {
        perror("sem_init");
        return 1;
    }

    if (pthread_create(&first_thread, NULL, worker, &first_token) != 0) {
        perror("pthread_create");
        return 1;
    }
    if (pthread_create(&second_thread, NULL, worker, &second_token) != 0) {
        perror("pthread_create");
        return 1;
    }

    if (pthread_join(first_thread, NULL) != 0) {
        perror("pthread_join");
        return 1;
    }
    if (pthread_join(second_thread, NULL) != 0) {
        perror("pthread_join");
        return 1;
    }

    if (sem_destroy(&output_lock) == -1) {
        perror("sem_destroy");
        return 1;
    }

    printf("\ndone\n");
    return 0;
}