#include <stdio.h>      
#include <stdlib.h>     

#include "../includes/printer.h"

typedef struct Printer {
    char* usage_bar_char;
} Printer;

static void visualise_cpu_usage(float percentage, const char* usage_bar_char);

void visualise_cpu_usage(float percentage, const char* usage_bar_char) {
    int progress = (int)(percentage / 10.0f);
    
    printf("[");

    for(int i = 0; i < progress; i++)
        printf("%s", usage_bar_char);

    for(int i = progress; i < 10; i++)
        printf(" ");

    printf("] %0.2f%%", (double)percentage);
}

Printer* printer_create(char* progress_marker) {
  Printer* printer = malloc(sizeof(*printer));
  if(printer == NULL) return NULL;

  *printer =(Printer){
    .usage_bar_char = progress_marker,
  };

  return printer;
}

void printer_print_proc_usage(Printer* printer, AnalysedProcStats* stats) {
    if(printer == NULL) return;

    if(stats == NULL) return;

    printf("\033[H\033[J");     
    printf("---------  CPU Tracker  ---------\n");
    
    for(uint8_t i = 0; i < stats->CPUs_number; i++) {
        printf("cpu %d: ", i);
        visualise_cpu_usage(stats->CPUs[i], printer->usage_bar_char);
        printf("\n");
    }
    
    printf("total usage: ");
    visualise_cpu_usage(stats->total, printer->usage_bar_char);
    printf("\n");

    printf("\n");
}

void printer_delete(Printer* printer) {
    if(printer == NULL) return;

    free(printer);
}