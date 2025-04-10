#include "life.h"
#include <pthread.h>

typedef struct {
    int y1;
    int y2;
    int steps;
    pthread_barrier_t *barrier_ptr;
    LifeBoard *state_ptr;
    LifeBoard *next_state_ptr;
} task_description;

void *thread_life(void *args) {
    task_description *task = (task_description *)args;
    int steps = task->steps;
    int y1 = task->y1;
    int y2 = task->y2;
    pthread_barrier_t *barrier = task->barrier_ptr;
    LifeBoard *state = task->state_ptr;
    LifeBoard *next_state = task->next_state_ptr;

    for (int step = 0; step < steps; step += 1) {   
        for (int y = y1; y < y2; y += 1) {
            for (int x = 1; x < state->width - 1; x += 1) {
                /* For each cell, examine a 3x3 "window" of cells around it,
                * and count the number of live (true) cells in the window. */
                int live_in_window = 0;
                for (int y_offset = -1; y_offset <= 1; y_offset += 1)
                    for (int x_offset = -1; x_offset <= 1; x_offset += 1)
                        if (LB_get(state, x + x_offset, y + y_offset))
                            live_in_window += 1;
                /* Cells with 3 live neighbors remain or become live.
                   Live cells with 2 live neighbors remain live. */
                LB_set(next_state, x, y,
                    live_in_window == 3 /* dead cell with 3 neighbors or live cell with 2 */ ||
                    (live_in_window == 4 && LB_get(state, x, y)) /* live cell with 3 neighbors */
                );
            }
        }
        pthread_barrier_wait(barrier);
        LB_swap(state, next_state);
        pthread_barrier_wait(barrier);
    }
    return NULL;
}

void simulate_life_parallel(int threads, LifeBoard *state, int steps) {
    /* YOUR CODE HERE */
    LifeBoard *next_state = LB_new(state->width, state->height);
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, threads);

    int max = state->height;
    int step = max / threads;

    pthread_t id[threads];
    task_description tasks[threads];

    for (int i = 0; i < threads; i++) {
        (i == 0) ? (tasks[i].y1 = (i * step) + 1) : (tasks[i].y1 = i * step);
        (i == threads - 1) ? (tasks[i].y2 = (i * step) - 1) : (tasks[i].y2 = i * step);
        tasks[i].steps = steps;
        tasks[i].barrier_ptr = &barrier;
        tasks[i].state_ptr = state;
        tasks[i].next_state_ptr = next_state;
        pthread_create(id+i, NULL, thread_life, tasks + i);
    }

    for(int i = 0; i < threads; i += 1) {
        pthread_join(id[i], NULL);
    }

    LB_del(next_state);
    
}