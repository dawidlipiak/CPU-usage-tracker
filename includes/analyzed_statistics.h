#ifndef ANALYSED_STATISTICS_H
#define ANALYSED_STATISTICS_H

#include <stdint.h>

typedef struct AnalysedProcStats {
    uint8_t CPUs_number;
    float total;
    float* CPUs;
} AnalysedProcStats;

#endif