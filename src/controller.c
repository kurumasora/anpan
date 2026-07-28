#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>

#include "breadstocker.h"
#include "controller.h"
#include "conveyor.h"
#include "dough.h"
#include "oven1.h"
#include "oven2.h"

#define POS_DOUGH_STOCKER  3.0
#define POS_OVEN1          2.0
#define POS_OVEN2          1.0
#define POS_BREAD_STOCKER  0.5
#define MOVE_STEP          1.0
#define POSITION_TOLERANCE 0.01
#define TRANSFER_RETRIES   100
#define OVEN_COUNT         2

typedef enum {//コンベアの積載状態
    CONVEYOR_EMPTY = 0, 
    CONVEYOR_DOUGH,
    CONVEYOR_BREAD
} ConveyorLoad;

/*
#define CONVEYOR_EMPTY 0
#define CONVEYOR_DOUGH 1
#define CONVEYOR_BREAD 2
*/


typedef struct {
    int number;
    int active;
    int done;
    int result;
    struct timespec finished_at;
    pthread_t thread;
} OvenJob;

static ConveyorLoad conveyor_load = CONVEYOR_EMPTY;
static double commanded_position = 0.0;
static pthread_mutex_t oven_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t oven_finished = PTHREAD_COND_INITIALIZER;

static int start_bake(int oven, int weak_heat)
{
    return oven == 1 ? StartBake1(weak_heat) : StartBake2(weak_heat);
}

static int wait_bake(int oven)
{
    return oven == 1 ? WaitBake1() : WaitBake2();
}

static int set_dough(int oven)
{
    return oven == 1 ? SetDough1() : SetDough2();
}

static int set_bread(int oven)
{
    return oven == 1 ? SetBread1() : SetBread2();
}

static double oven_position(int oven)
{
    return oven == 1 ? POS_OVEN1 : POS_OVEN2;
}

static int move_conveyor(double target)
{
    double current = commanded_position;

    while (fabs(target - current) > POSITION_TOLERANCE) {
        double remaining = target - current;//現在地から目標値の差
        double next = target;//移動距離

        if (remaining > MOVE_STEP) {//差が1より大きい
            next = current + MOVE_STEP;
        } else if (remaining < -MOVE_STEP) {//差が-1より小さい
            next = current - MOVE_STEP;
        }

        if (next < 0.0 || next >= 3.5) {
            fprintf(stderr, "[SAFETY] Unsafe conveyor target %.3f rejected\n", next);
            return -1;
        }
        if (StartConveyor(next) < 0.0) {
            fprintf(stderr, "[ERROR] Conveyor failed near %.3f\n", next);
            return -1;
        }
        /*
         * StartConveyor() blocks until the commanded position is reached.
         * Keep the controller's commanded position instead of feeding the
         * simulator's intentionally noisy GetPosConveyor() value back into
         * the motion loop.
         */
        commanded_position = next;
        current = next;
    }
    return 0;
}

static int should_report_retry(int attempt)
{
    return attempt == 0 || (attempt + 1) % 10 == 0 ||
           attempt + 1 == TRANSFER_RETRIES;
}

static int retry_positioned_operation(double position, int (*operation)(void))
{
    int attempt;
    int result = 666;

    for (attempt = 0; attempt < TRANSFER_RETRIES; ++attempt) {
        result = operation();
        if (result == 0) {
            return 0;
        }
        if (should_report_retry(attempt)) {
            fprintf(stderr,
                    "[RETRY] Transfer returned %d at target %.2f, "
                    "measured %.3f (%d/%d)\n",
                    result, position, GetPosConveyor(), attempt + 1,
                    TRANSFER_RETRIES);
        }
    }
    return -1;
}

static int fetch_dough(void)
{
    if (conveyor_load != CONVEYOR_EMPTY) {
        fprintf(stderr, "[SAFETY] Cannot load dough: conveyor is not empty\n");
        return -1;
    }
    if (move_conveyor(POS_DOUGH_STOCKER) != 0 ||
        retry_positioned_operation(POS_DOUGH_STOCKER, GetOneDough) != 0) {
        fprintf(stderr, "[ERROR] Could not obtain dough\n");
        return -1;
    }
    conveyor_load = CONVEYOR_DOUGH;
    return 0;
}

static int load_oven(int oven)
{
    double position = oven_position(oven);
    int attempt;
    int last_result = 666;

    if (conveyor_load != CONVEYOR_DOUGH) {
        fprintf(stderr, "[SAFETY] Oven %d requires dough on the conveyor\n", oven);
        return -1;
    }
    if (move_conveyor(position) != 0) {
        return -1;
    }
    for (attempt = 0; attempt < TRANSFER_RETRIES; ++attempt) {
        last_result = set_dough(oven);
        if (last_result == 0) {
            conveyor_load = CONVEYOR_EMPTY;
            if (start_bake(oven, 0) == 0) {
                return 0;
            }
            fprintf(stderr, "[ERROR] Oven %d could not start\n", oven);
            return -1;
        }
        if (should_report_retry(attempt)) {
            fprintf(stderr,
                    "[RETRY] SetDough%d returned %d at position %.3f "
                    "(%d/%d)\n",
                    oven, last_result, GetPosConveyor(), attempt + 1,
                    TRANSFER_RETRIES);
        }
    }
    fprintf(stderr,
            "[ERROR] Could not load oven %d: result=%d position=%.3f\n",
            oven, last_result, GetPosConveyor());
    return -1;
}

static int store_bread_from_oven(int oven)
{
    double position = oven_position(oven);
    int attempt;
    int last_result = 666;

    if (conveyor_load != CONVEYOR_EMPTY) {
        fprintf(stderr, "[SAFETY] Cannot unload oven: conveyor is not empty\n");
        return -1;
    }
    if (move_conveyor(position) != 0) {
        return -1;
    }
    for (attempt = 0; attempt < TRANSFER_RETRIES; ++attempt) {
        last_result = set_bread(oven);
        if (last_result == 0) {
            conveyor_load = CONVEYOR_BREAD;
            break;
        }
        if (should_report_retry(attempt)) {
            fprintf(stderr,
                    "[RETRY] SetBread%d returned %d at position %.3f "
                    "(%d/%d)\n",
                    oven, last_result, GetPosConveyor(), attempt + 1,
                    TRANSFER_RETRIES);
        }
    }
    if (conveyor_load != CONVEYOR_BREAD) {
        fprintf(stderr,
                "[ERROR] Could not unload oven %d: result=%d position=%.3f\n",
                oven, last_result, GetPosConveyor());
        return -1;
    }
    if (move_conveyor(POS_BREAD_STOCKER) != 0 ||
        retry_positioned_operation(POS_BREAD_STOCKER, ThrowBread) != 0) {
        fprintf(stderr, "[ERROR] Could not store bread from oven %d\n", oven);
        return -1;
    }
    conveyor_load = CONVEYOR_EMPTY;
    return 0;
}

static void *oven_worker(void *argument)
{
    OvenJob *job = argument;
    int result = wait_bake(job->number);

    while (result == 10) {
        printf("Oven %d: underbaked; reheating with low heat\n", job->number);
        result = start_bake(job->number, 1);
    }

    pthread_mutex_lock(&oven_mutex);
    job->result = result;
    clock_gettime(CLOCK_MONOTONIC, &job->finished_at);
    job->done = 1;
    pthread_cond_signal(&oven_finished);
    pthread_mutex_unlock(&oven_mutex);
    return NULL;
}

static int start_oven_job(OvenJob *job)
{
    job->done = 0;
    job->result = 666;
    job->active = 1;

    if (pthread_create(&job->thread, NULL, oven_worker, job) != 0) {
        job->active = 0;
        fprintf(stderr, "[ERROR] Could not create worker for oven %d\n", job->number);
        return -1;
    }
    return 0;
}

static int any_active(const OvenJob jobs[OVEN_COUNT])
{
    return jobs[0].active || jobs[1].active;
}

static int completion_is_earlier(const OvenJob *left, const OvenJob *right)
{
    if (left->finished_at.tv_sec != right->finished_at.tv_sec) {
        return left->finished_at.tv_sec < right->finished_at.tv_sec;
    }
    if (left->finished_at.tv_nsec != right->finished_at.tv_nsec) {
        return left->finished_at.tv_nsec < right->finished_at.tv_nsec;
    }
    return left->number < right->number;
}

static int select_finished_oven(OvenJob jobs[OVEN_COUNT])
{
    int selected = -1;
    int i;

    pthread_mutex_lock(&oven_mutex);
    while (!jobs[0].done && !jobs[1].done) {
        pthread_cond_wait(&oven_finished, &oven_mutex);
    }
    for (i = 0; i < OVEN_COUNT; ++i) {
        if (jobs[i].done &&
            (selected < 0 || completion_is_earlier(&jobs[i], &jobs[selected]))) {
            selected = i;
        }
    }
    jobs[selected].done = 0;
    pthread_mutex_unlock(&oven_mutex);
    return selected;
}

static void join_active_jobs(OvenJob jobs[OVEN_COUNT])
{
    int i;

    for (i = 0; i < OVEN_COUNT; ++i) {
        if (jobs[i].active) {
            pthread_join(jobs[i].thread, NULL);
            jobs[i].active = 0;
        }
    }
}

static int produce_batch(int total)
{
    OvenJob jobs[OVEN_COUNT] = {
        { .number = 1, .active = 0, .done = 0, .result = 0 },
        { .number = 2, .active = 0, .done = 0, .result = 0 }
    };
    int loaded = 0;
    int produced = 0;
    int i;

    for (i = 0; i < OVEN_COUNT && loaded < total; ++i) {
        if (fetch_dough() != 0 || load_oven(jobs[i].number) != 0 ||
            start_oven_job(&jobs[i]) != 0) {
            goto fail;
        }
        ++loaded;
        printf("Oven %d: baking item %d/%d\n", jobs[i].number, loaded, total);
    }

    while (any_active(jobs)) {
        int selected = select_finished_oven(jobs);
        OvenJob *job = &jobs[selected];

        pthread_join(job->thread, NULL);
        job->active = 0;

        if (job->result != 0) {
            fprintf(stderr, "[ERROR] Oven %d failed with status %d\n",
                    job->number, job->result);
            goto fail;
        }
        if (store_bread_from_oven(job->number) != 0) {
            goto fail;
        }
        ++produced;
        printf("Oven %d: stored anpan %d/%d\n",
               job->number, produced, total);

        /* Restart the oven just serviced before handling the next ready oven. */
        if (loaded < total) {
            if (fetch_dough() != 0 || load_oven(job->number) != 0 ||
                start_oven_job(job) != 0) {
                goto fail;
            }
            ++loaded;
            printf("Oven %d: baking item %d/%d\n", job->number, loaded, total);
        }
    }

    return produced;

fail:
    join_active_jobs(jobs);
    return -1;
}

int RunProduction(int target_count)
{
    int batch = 0;
    int total_produced = 0;

    if (InitConveyor() != 0) {
        fprintf(stderr, "[ERROR] Conveyor initialization failed\n");
        return -1;
    }
    commanded_position = 0.0;

    while (target_count == 0 || total_produced < target_count) {
        int dough_count;
        int batch_target;
        int batch_produced;

        ++batch;
        printf("\n--- AGV refill batch %d ---\n", batch);
        if (InitDough() != 0) {
            fprintf(stderr, "[ERROR] Dough stocker initialization failed\n");
            return -1;
        }
        dough_count = GetDoughCount();
        printf("Dough supplied: %d\n", dough_count);

        if (dough_count <= 0) {
            printf("AGV supplied no dough; requesting another refill\n");
            continue;
        }

        batch_target = dough_count;
        if (target_count > 0 &&
            batch_target > target_count - total_produced) {
            batch_target = target_count - total_produced;
        }

        conveyor_load = CONVEYOR_EMPTY;
        batch_produced = produce_batch(batch_target);
        if (batch_produced < 0) {
            return -1;
        }
        total_produced += batch_produced;
        printf("Batch %d complete: %d stored (total %d",
               batch, batch_produced, total_produced);
        if (target_count > 0) {
            printf("/%d", target_count);
        }
        printf(")\n");
    }

    return total_produced;
}
