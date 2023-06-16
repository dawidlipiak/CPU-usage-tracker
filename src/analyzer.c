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

static float single_analyse(CpuStatistics* statistics, uint64_t* total_prev, uint64_t* idle_prev);


Analyzer* analyzer_new() {
    Analyzer* analyzer = malloc(sizeof(*analyzer));
    if(analyzer == NULL) { return NULL; }

    analyzer->prev_initialized = false;

    return analyzer;
}

static float single_analyse(CpuStatistics* statistics, uint64_t* total_prev, uint64_t* idle_prev) {
  uint64_t idle = statistics->idle + statistics->iowait;
  uint64_t non_idle = statistics->user + statistics->nice + statistics->system + statistics->irq + statistics->sortirq + statistics->steal;
  uint64_t total = idle + non_idle - *total_prev;
  uint64_t idled = idle - *idle_prev;

  float percentage = 0.0;
  if(total != 0.0)
    percentage = (float)(total - idled) / (float)total * 100.0;

  *total_prev = idle + non_idle;
  *idle_prev = idle;

  return percentage;
}

Result_enum analyzer_analyse_statistics(Analyzer* analyzer, ProcStatistics* row_statistics, AnalysedProcStats* analysed_statistics) {
  if(analyzer == NULL)
    return NULL_TARGET_ERROR;
    
  if(analyzer->CPUs_total_prev == NULL)
    return NULL_TARGET_ERROR;

  if(analyzer->CPUs_idle_prev == NULL)
    return NULL_TARGET_ERROR;
  
  if(row_statistics == NULL)
    return NULL_TARGET_ERROR;

  if(analysed_statistics == NULL)
    return NULL_TARGET_ERROR;

  if(!analyzer->prev_initialized) {
    analyzer->CPUs_total_prev = malloc(sizeof(uint64_t) * row_statistics->CPUs_number);
    if(analyzer->CPUs_total_prev == NULL) { return ALLOCATION_ERROR; }
    
    analyzer->CPUs_idle_prev = malloc(sizeof(uint64_t) * row_statistics->CPUs_number);
    if(analyzer->CPUs_idle_prev == NULL) {
      free(analyzer->CPUs_total_prev);

      return ALLOCATION_ERROR;
    }
    
    uint64_t idle = row_statistics->total.idle + row_statistics->total.iowait;
    uint64_t non_idle = row_statistics->total.user + row_statistics->total.nice + row_statistics->total.system + row_statistics->total.irq + row_statistics->total.sortirq + row_statistics->total.steal;
    
    analyzer->proc_total_prev = idle + non_idle;
    analyzer->proc_idle_prev = idle;

    for(uint8_t i = 0; i < row_statistics->CPUs_number; i++) {
        idle = row_statistics->CPUs[i].idle + row_statistics->CPUs[i].iowait;
        non_idle = row_statistics->CPUs[i].user + row_statistics->CPUs[i].nice + row_statistics->CPUs[i].system + row_statistics->CPUs[i].irq +row_statistics->CPUs[i].sortirq + row_statistics->CPUs[i].steal;
    
        analyzer->CPUs_total_prev[i] = idle + non_idle;
        analyzer->CPUs_idle_prev[i] = idle;
    }
  }

  analysed_statistics->CPUs_number = row_statistics->CPUs_number;
  analysed_statistics->total = single_analyse(&row_statistics->total, &analyzer->proc_total_prev, &analyzer->proc_idle_prev);
  
  for(uint8_t i = 0; i < row_statistics->CPUs_number; i++) {
    analysed_statistics->CPUs[i] = single_analyse(&row_statistics->CPUs[i], &analyzer->CPUs_total_prev[i], &analyzer->CPUs_idle_prev[i]);
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