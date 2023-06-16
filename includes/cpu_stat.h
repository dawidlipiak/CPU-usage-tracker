#ifndef CPU_STAT_H
#define CPU_STAT_H

#include <stdint.h> // uint8_t, uint32_t

// Data for 1 CPU core structure
typedef struct CpuStatistics {
  uint32_t user;
  uint32_t nice;
  uint32_t system;
  uint32_t idle;
  uint32_t iowait;
  uint32_t irq;
  uint32_t sortirq;
  uint32_t steal;
} CpuStatistics;


// Data for processor structure
typedef struct ProcStatistics {
    uint8_t CPUs_number;
    CpuStatistics total;
    CpuStatistics* CPUs;
} ProcStatistics;

#endif 