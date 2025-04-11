#include <pthread.h>
#include <stdlib.h>
#include "life.h"

typedef struct {
    int thread_id;
    int threads;
    int steps;
    LifeBoard *current;
    LifeBoard *next;
    pthread_barrier_t *barrier;
} ThreadData;

void *life_worker(void *arg) {
    ThreadData *data = arg;
    int width = data->current->width;
    int height = data->current->height;
    int rows = height - 2;

    // divide rows among threads
    int chunk = rows / data->threads;
    int remainder = rows % data->threads;
    int start = 1 + data->thread_id * chunk + (data->thread_id < remainder ? data->thread_id : remainder);
    int end = start + chunk + (data->thread_id < remainder ? 1 : 0);

    for (int step = 0; step < data->steps; ++step) {
        for (int y = start; y < end; y++) {
            for (int x = 1; x < width - 1; x++) {
                int live = 0;
                for (int dy = -1; dy <= 1; dy++)
                    for (int dx = -1; dx <= 1; dx++)
                        live += LB_get(data->current, x + dx, y + dy);

                LB_set(data->next, x, y,
                    live == 3 || (live == 4 && LB_get(data->current, x, y))
                );
            }
        }

        pthread_barrier_wait(data->barrier);

        if (data->thread_id == 0) {
            LB_swap(data->current, data->next);
        }

        pthread_barrier_wait(data->barrier);
    }

    return NULL;
}

void simulate_life_parallel(int threads, LifeBoard *state, int steps) {
    if (threads < 1) threads = 1;

    pthread_t *tids = malloc(sizeof(pthread_t) * threads);
    ThreadData *tds = malloc(sizeof(ThreadData) * threads);
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, threads);

    LifeBoard *next = LB_new(state->width, state->height);

    for (int i = 0; i < threads; i++) {
        tds[i].thread_id = i;
        tds[i].threads = threads;
        tds[i].steps = steps;
        tds[i].current = state;
        tds[i].next = next;
        tds[i].barrier = &barrier;
        pthread_create(&tids[i], NULL, life_worker, &tds[i]);
    }

    for (int i = 0; i < threads; i++) {
        pthread_join(tids[i], NULL);
    }

    pthread_barrier_destroy(&barrier);
    LB_del(next);
    free(tds);
    free(tids);
}
