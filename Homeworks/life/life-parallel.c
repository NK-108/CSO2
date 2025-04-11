#include <pthread.h>
#include <stdlib.h>
#include "life.h"

typedef struct {
    int thread_id;
    int threads;
    int steps;
    LifeBoard *even;
    LifeBoard *odd;
    pthread_barrier_t *barrier;
} ThreadData;

void *life_worker(void *arg) {
    ThreadData *data = arg;
    int width = data->even->width;
    int height = data->even->height;
    int rows = height - 2;

    // divide rows among threads
    int chunk = rows / data->threads;
    int remainder = rows % data->threads;
    int start = 1 + data->thread_id * chunk + (data->thread_id < remainder ? data->thread_id : remainder);
    int end = start + chunk + (data->thread_id < remainder ? 1 : 0);

    for (int step = 0; step < data->steps; ++step) {
        if (step % 2) { //ODD
            for (int y = start; y < end; y++) {
                for (int x = 1; x < width - 1; x++) {
                    int live = 0;
                    for (int dy = -1; dy <= 1; dy++)
                        for (int dx = -1; dx <= 1; dx++)
                            live += LB_get(data->odd, x + dx, y + dy);
    
                    LB_set(data->even, x, y,
                        live == 3 || (live == 4 && LB_get(data->odd, x, y))
                    );
                }
            }
        }
        else { //EVEN
            for (int y = start; y < end; y++) {
                for (int x = 1; x < width - 1; x++) {
                    int live = 0;
                    for (int dy = -1; dy <= 1; dy++)
                        for (int dx = -1; dx <= 1; dx++)
                            live += LB_get(data->even, x + dx, y + dy);
    
                    LB_set(data->odd, x, y,
                        live == 3 || (live == 4 && LB_get(data->even, x, y))
                    );
                }
            }
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

    LifeBoard *odd = LB_new(state->width, state->height);

    for (int i = 0; i < threads; i++) {
        tds[i].thread_id = i;
        tds[i].threads = threads;
        tds[i].steps = steps;
        tds[i].even = state;
        tds[i].odd = odd;
        tds[i].barrier = &barrier;
        pthread_create(&tids[i], NULL, life_worker, &tds[i]);
    }

    for (int i = 0; i < threads; i++) {
        pthread_join(tids[i], NULL);
    }

    if (steps % 2) { // swap board to original allocation if steps are odd
        LB_swap(state, odd);
    }

    pthread_barrier_destroy(&barrier);
    LB_del(odd);
    free(tds);
    free(tids);
}
