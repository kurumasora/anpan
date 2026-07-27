#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "controller.h"

static int parse_target_count(int argc, char *argv[], int *target_count)
{
    char *end;
    long value;

    if (argc == 1) {
        *target_count = 12;
        return 0;
    }
    if (argc != 2) {
        return -1;
    }

    errno = 0;
    end = NULL;
    value = strtol(argv[1], &end, 10);
    if (errno != 0 || end == argv[1] || *end != '\0' ||
        value < 0 || value > INT_MAX) {
        return -1;
    }
    *target_count = (int)value;
    return 0;
}

int main(int argc, char *argv[])
{
    int target_count;
    int produced;

    if (parse_target_countargc, argv(, &target_count) != 0) {
        fprintf(stderr, "Usage: %s [target-anpan-count]\n", argv[0]);
        fprintf(stderr, "  target > 0: produce exactly that many anpan\n");
        fprintf(stderr, "  target = 0: continuous operation\n");
        return 2;
    }

    printf("=== Anpan Manufacturing Application ===\n");
    produced = RunProduction(target_count);

    if (produced < 0) {
        printf("=== Production stopped because of an error ===\n");
        return 1;
    }

    printf("=== Production complete: %d anpan ===\n", produced);
    return 0;
}
