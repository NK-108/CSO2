#include "life.h"
#include <pthread.h>

typedef struct {
    int y1;
    int y2;
    int steps;
    pthread_barrier_t *barrier_ptr;
    LifeBoard *odd_state_ptr;
    LifeBoard *even_state_ptr;
} task_description;

void *thread_life(void *args) {
    task_description *task = (task_description *)args;
    int steps = task->steps;
    int y1 = task->y1;
    int y2 = task->y2;
    pthread_barrier_t *barrier = task->barrier_ptr;
    LifeBoard *odd_state = task->odd_state_ptr;
    LifeBoard *even_state = task->even_state_ptr;

    for (int step = 0; step < steps; step += 1) {   
        if (step % 2) { //ODD
            for (int y = y1; y < y2; y += 1) {
                for (int x = 1; x < odd_state->width - 1; x += 1) {
                    /* For each cell, examine a 3x3 "window" of cells around it,
                    * and count the number of live (true) cells in the window. */
                    int live_in_window = 0;
                    for (int y_offset = -1; y_offset <= 1; y_offset += 1)
                        for (int x_offset = -1; x_offset <= 1; x_offset += 1)
                            if (LB_get(odd_state, x + x_offset, y + y_offset))
                                live_in_window += 1;
    
                    /* Cells with 3 live neighbors remain or become live.
                       Live cells with 2 live neighbors remain live. */
                    LB_set(even_state, x, y,
                        live_in_window == 3 /* dead cell with 3 neighbors or live cell with 2 */ ||
                        (live_in_window == 4 && LB_get(odd_state, x, y)) /* live cell with 3 neighbors */
                    );
                }
            }
        }
        else { //EVEN
            for (int y = y1; y < y2; y += 1) {
                for (int x = 1; x < even_state->width - 1; x += 1) {
                    /* For each cell, examine a 3x3 "window" of cells around it,
                    * and count the number of live (true) cells in the window. */
                    int live_in_window = 0;
                    for (int y_offset = -1; y_offset <= 1; y_offset += 1)
                        for (int x_offset = -1; x_offset <= 1; x_offset += 1)
                            if (LB_get(even_state, x + x_offset, y + y_offset))
                                live_in_window += 1;

                    /* Cells with 3 live neighbors remain or become live.
                       Live cells with 2 live neighbors remain live. */
                    LB_set(odd_state, x, y,
                        live_in_window == 3 /* dead cell with 3 neighbors or live cell with 2 */ ||
                        (live_in_window == 4 && LB_get(even_state, x, y)) /* live cell with 3 neighbors */
                    );
                }
            }
        }
        pthread_barrier_wait(barrier);
    }
    return NULL;
}

void simulate_life_parallel(int threads, LifeBoard *state, int steps) {
    /* YOUR CODE HERE */
    LifeBoard *odd_state = LB_new(state->width, state->height);
    LifeBoard *even_state = LB_clone(state);
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, threads);

    int max = state->height;
    int step = max / threads;

    pthread_t id[threads];
    task_description tasks[threads];

    for (int i = 0; i < threads; i++) {
        tasks[i].y1 = i * step;
        tasks[i].y2 = (i + 1) * step;
        tasks[i].steps = steps;
        tasks[i].barrier_ptr = &barrier;
        tasks[i].odd_state_ptr = odd_state;
        tasks[i].even_state_ptr = even_state;
        pthread_create(id+i, NULL, thread_life, tasks + i);
    }

    for(int i = 0; i < threads; i += 1) {
        pthread_join(id[i], NULL);
    }

    if (steps % 2) {
        LB_swap(state, odd_state);
        LB_del(even_state);
        LB_del(odd_state);
    } else {
        LB_swap(state, even_state);
        LB_del(even_state);
        LB_del(odd_state);
    }
    
}