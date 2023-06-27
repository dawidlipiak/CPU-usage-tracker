#include <stdio.h>      
#include <stdlib.h>     
#include <sys/stat.h>   
#include <stdbool.h>   
#include "string.h"     

#include "../includes/reader.h"


typedef struct Reader {
  FILE* file;
} Reader;


Reader* reader_create(char* file_path) {
  if(file_path == NULL) return NULL;

  Reader* reader = malloc(sizeof(*reader));
  if(reader == NULL) return NULL;

  FILE* file = fopen(file_path, "r");
  if(file == NULL) {
    free(reader);
    return NULL;
  }

  *reader =(Reader){
    .file = file,
  };

  return reader;
}

Result_enum reader_read_latest_stats(Reader* const reader, ProcStatistics* stats) {
  bool data_read_successfully = false;
  size_t buffer_size = 1024;
  size_t chars_read = 0;
  char* buffer;
  uint8_t CPUs_number = 0;
  size_t cpu_num = 0;
  char* file_content;
  char* line;
  int trash;

  if(reader == NULL) return NULL_TARGET_ERROR;

  if(stats == NULL) return NULL_TARGET_ERROR;

  buffer = malloc(buffer_size);
  if(buffer == NULL) return ALLOCATION_ERROR;

  while(!data_read_successfully) {
    fseek(reader->file, 0, SEEK_SET);
    chars_read = fread(buffer, sizeof(char), buffer_size, reader->file);
    data_read_successfully = !ferror(reader->file) && feof(reader->file);

    if(!data_read_successfully) {
      free(buffer);

      buffer_size *= 2;
      buffer = malloc(buffer_size);

      if(buffer == NULL) return ALLOCATION_ERROR;
    }
  }
  buffer[chars_read] = '\0';

  file_content = buffer;
  while((file_content = strstr(file_content, "cpu")) != NULL) {
    CPUs_number++;
    file_content++;
  }

  CPUs_number--;
  
  stats->CPUs_number = CPUs_number;
  stats->CPUs = malloc(sizeof(CpuStatistics) * CPUs_number);

  if(stats->CPUs == NULL) {
    free(buffer);
    return ALLOCATION_ERROR;
  }

  line = strtok(buffer, "\n");
  while (line != NULL){
      if(strstr(line, "cpu") != NULL) {
          if(cpu_num == 0) { 
              sscanf(line, "cpu %d %d %d %d %d %d %d %d", &(stats->total.user), &(stats->total.nice),
                      &(stats->total.system), &(stats->total.idle), &(stats->total.iowait),
                      &(stats->total.irq), &(stats->total.sortirq), &(stats->total.steal));
          }
          else { 
              sscanf(line, "cpu%d %d %d %d %d %d %d %d %d", &trash, &(stats->CPUs[cpu_num - 1].user), &(stats->CPUs[cpu_num - 1].nice),
                      &(stats->CPUs[cpu_num - 1].system), &(stats->CPUs[cpu_num - 1].idle), &(stats->CPUs[cpu_num - 1].iowait),
                      &(stats->CPUs[cpu_num - 1].irq), &(stats->CPUs[cpu_num - 1].sortirq), &(stats->CPUs[cpu_num - 1].steal));
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
  if(reader == NULL) return;

  fclose(reader->file);
  free(reader);
}