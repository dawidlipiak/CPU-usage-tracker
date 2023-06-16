#include <stdio.h>      
#include <stdlib.h>     
#include <sys/stat.h>   
#include <stdbool.h>    
#include "string.h"     

// Inside libraries includes
#include "../includes/reader.h"


static const char* STATISTICS_FILE_PATH = "/proc/stat";

typedef struct Reader {
  size_t read_interval;
  FILE* file;
} Reader;


Reader* reader_create(register const size_t read_interval) {
  if(read_interval == 0) return NULL;

  Reader* reader = malloc(sizeof(*reader));

  if(reader == NULL) return NULL;

  FILE* file = fopen(STATISTICS_FILE_PATH, "r");
  if(file == NULL) {
    free(reader);
    return NULL;
  }

  *reader = (Reader){
    .file = file,
    .read_interval = read_interval
  };

  return reader;
}


Result_enum reader_read_latest_statistics(Reader* const reader, ProcStatistics* statistics) {
  if(reader == NULL){ return NULL_TARGET_ERROR; }

  if(statistics == NULL){ return NULL_TARGET_ERROR; }

  bool data_read_successfully;
  size_t buffer_size = 1024;

  char* buffer = malloc(buffer_size);
  if(buffer == NULL){ return ALLOCATION_ERROR; }

  do {
    fread(buffer, sizeof(char), buffer_size, reader->file);
    data_read_successfully = ferror(reader->file) || !feof(reader->file);

    if(!data_read_successfully) {
      free(buffer);

      buffer_size *= 2;
      buffer = malloc(buffer_size);

      if(buffer == NULL){ return ALLOCATION_ERROR;}
    }
  }
  while(!data_read_successfully);

  uint8_t CPUs_number = 0;
  char* file_content = buffer;
  while((file_content = strstr(file_content, "cpu")) != NULL) {
    CPUs_number++;
    file_content += 3;
  }

  statistics->CPUs_number = CPUs_number;
  statistics->CPUs = malloc(sizeof(CpuStatistics) * CPUs_number);

  if(statistics->CPUs == NULL) {
    free(buffer);
    return ALLOCATION_ERROR;
  }

  char* line = strtok(buffer, "\n");
  size_t cpu_num = 0;
  int trash;
  while (line != NULL){
      if(strstr(line, "cpu") != NULL) {
          if(cpu_num == 0) { // first "cpu" occurrence is statistic for total
              sscanf(line, "cpu %d %d %d %d %d %d %d %d", &(statistics->total.user), &(statistics->total.nice),
                      &(statistics->total.system), &(statistics->total.idle), &(statistics->total.iowait),
                      &(statistics->total.irq), &(statistics->total.sortirq), &(statistics->total.steal));
          }

          else { // next "cpu" occurrences are statistic for single cpu
              sscanf(line, "cpu%d %d %d %d %d %d %d %d %d", &trash, &(statistics->CPUs[cpu_num - 1].user), &(statistics->CPUs[cpu_num - 1].nice),
                      &(statistics->CPUs[cpu_num - 1].system), &(statistics->CPUs[cpu_num - 1].idle), &(statistics->CPUs[cpu_num - 1].iowait),
                      &(statistics->CPUs[cpu_num - 1].irq), &(statistics->CPUs[cpu_num - 1].sortirq), &(statistics->CPUs[cpu_num - 1].steal));
          }

          if(cpu_num == CPUs_number) { 
            break;
          }

          cpu_num++;
      }
      else {
        break;
      }

      line = strtok(NULL, "\n");
  }

  free(buffer);

  return SUCCESS;
}

void reader_delete(Reader* const reader) {
  if(reader == NULL) { return; }

  fclose(reader->file);
  free(reader);
}