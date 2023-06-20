#include <stdio.h>      

// Inside libraries includes
#include "../includes/printer.h"

static void visualise_cpu_usage(float percentage);

void visualise_cpu_usage(float percentage) {
    int progress = (int)(percentage / 10.0);
    
    printf("[");

    for(int i = 0; i < progress; i++)
        printf("#");

    for(int i = progress; i < 10; i++)
        printf(" ");

    printf("] %0.2f%%", percentage);
}

void print_proc_usage(AnalysedProcStats* stats) {
    if(stats == NULL) return;

    printf("\033[H\033[J");

    printf("cpu:  ");
    visualise_cpu_usage(stats->total);
    printf("\n");
    
    for(uint8_t i = 0; i < stats->CPUs_number; i++) {
        printf("cpu %d: ", i);
        visualise_cpu_usage(stats->CPUs[i]);
        printf("\n");
    }

    printf("\n");
}