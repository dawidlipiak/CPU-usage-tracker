#ifndef ANALYSED_STATISTICS_H
#define ANALYSED_STATISTICS_H

#include <stdint.h>

// Analyzed processor statistic object structure
typedef struct AnalysedProcStats {
     float* CPUs;
  float total;
  uint8_t CPUs_number;
  char padding[3];
} AnalysedProcStats;

#endif