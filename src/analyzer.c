#include <stdlib.h>     
#include <stdbool.h>    

// Inside libraries includes
#include "../includes/analyzer.h"

typedef struct Analyzer {
  uint64_t proc_total_prev;
  uint64_t proc_idle_prev;
  uint64_t* CPUs_total_prev;
  uint64_t* CPUs_idle_prev;
  bool prev_initialized;
} Analyzer;

static float single_analyse(CpuStatistics* stats, uint64_t* total_prev, uint64_t* idle_prev);


Analyzer* analyzer_create() {
    Analyzer* analyzer = malloc(sizeof(*analyzer));
    if(analyzer == NULL) { return NULL; }

    analyzer->prev_initialized = false;
    analyzer->CPUs_total_prev = NULL;
    analyzer->CPUs_idle_prev = NULL;

    return analyzer;
}

static float single_analyse(CpuStatistics* stats, uint64_t* total_prev, uint64_t* idle_prev) {
  uint64_t idle = stats->idle + stats->iowait;
  uint64_t non_idle = stats->user + stats->nice + stats->system + stats->irq + stats->sortirq + stats->steal;
  uint64_t total = idle + non_idle - *total_prev;
  uint64_t idled = idle - *idle_prev;

  float percentage = 0.0;
  if(total != 0.0)
    percentage = (float)(total - idled) / (float)total * 100.0;

  *total_prev = idle + non_idle;
  *idle_prev = idle;

  return percentage;
}

Result_enum analyzer_analyse_stats(Analyzer* analyzer, ProcStatistics* row_stats, AnalysedProcStats* analysed_stats) {
  if(analyzer == NULL) return NULL_TARGET_ERROR;
    
  if(row_stats == NULL) return NULL_TARGET_ERROR;

  if(analysed_stats == NULL) return NULL_TARGET_ERROR;


  if(!analyzer->prev_initialized) {
    analyzer->CPUs_total_prev = malloc(sizeof(uint64_t) * row_stats->CPUs_number);
    if(analyzer->CPUs_total_prev == NULL) return ALLOCATION_ERROR; 
    
    analyzer->CPUs_idle_prev = malloc(sizeof(uint64_t) * row_stats->CPUs_number);
    if(analyzer->CPUs_idle_prev == NULL) {
      free(analyzer->CPUs_total_prev);

      return ALLOCATION_ERROR;
    }
    
    uint64_t idle = row_stats->total.idle + row_stats->total.iowait;
    uint64_t non_idle = row_stats->total.user + row_stats->total.nice + row_stats->total.system + row_stats->total.irq + row_stats->total.sortirq + row_stats->total.steal;
    
    analyzer->proc_total_prev = idle + non_idle;
    analyzer->proc_idle_prev = idle;

    for(uint64_t i = 0; i < row_stats->CPUs_number; i++) {
        idle = row_stats->CPUs[i].idle + row_stats->CPUs[i].iowait;
        non_idle = row_stats->CPUs[i].user + row_stats->CPUs[i].nice + row_stats->CPUs[i].system + row_stats->CPUs[i].irq +row_stats->CPUs[i].sortirq + row_stats->CPUs[i].steal;
    
        analyzer->CPUs_total_prev[i] = idle + non_idle;
        analyzer->CPUs_idle_prev[i] = idle;
    }

    analyzer->prev_initialized = true;

    analysed_stats->CPUs = NULL;
    analysed_stats->CPUs_number = row_stats->CPUs_number;

    return INITIALISATION_SUCCESS;
  }

  analysed_stats->CPUs = malloc(sizeof(float) * row_stats->CPUs_number);
  if(analysed_stats->CPUs == NULL)return ALLOCATION_ERROR;

  analysed_stats->CPUs_number = row_stats->CPUs_number;
  analysed_stats->total = single_analyse(&row_stats->total, &analyzer->proc_total_prev, &analyzer->proc_idle_prev);
  
  for(uint8_t i = 0; i < row_stats->CPUs_number; i++) {
    analysed_stats->CPUs[i] = single_analyse(&(row_stats->CPUs[i]), &(analyzer->CPUs_total_prev[i]), &(analyzer->CPUs_idle_prev[i]));
  }

  return SUCCESS;
}

void analyzer_delete(Analyzer* analyzer) {
  if(analyzer == NULL) return;

  if(analyzer->CPUs_total_prev != NULL)
    free(analyzer->CPUs_total_prev);

  if(analyzer->CPUs_idle_prev != NULL)
    free(analyzer->CPUs_idle_prev);

  free(analyzer);
}