#include <stdio.h>      
#include <stdlib.h>    

#include "../includes/logger.h"

typedef struct Logger {
    FILE* file;
} Logger;

Logger* logger_create(char* logs_file_path) {
  Logger* logger;
  FILE* file;

  if(logs_file_path == NULL) return NULL;

  logger = malloc(sizeof(Logger));
  if(logger == NULL) return NULL;

  file = fopen(logs_file_path, "a+");

  if(file == NULL) {
    free(logger);
    return NULL;
  }

  *logger = (Logger){
    .file = file
  };

  return logger;
}

void logger_log(Logger* logger, LogTag tag, const char* message) {
    if(tag == Debug)
        fprintf(logger->file, "%s", "[Debug]    ");
    else if(tag == Info)
        fprintf(logger->file, "%s", "[Info]     ");
    else if(tag == Error)
        fprintf(logger->file, "%s", "[Error]    ");
    else
        fprintf(logger->file, "%s", "[]    ");

    fprintf(logger->file, "%s\n", message);
    fflush(logger->file);
}

void logger_delete(Logger* logger) {
  if (logger == NULL) return;

  if(logger->file != NULL)
    fclose(logger->file);

  free(logger);
}