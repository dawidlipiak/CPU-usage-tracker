#include <signal.h>     
#include <string.h>     
#include <stdatomic.h>  
#include <stdbool.h>    
#include <time.h>       
#include <stdio.h>      
#include <stdlib.h>     
#include <pthread.h>    
#include <sys/time.h>   

#include "../includes/tracker.h"
#include "../includes/reader.h"
#include "../includes/analyzer.h"
#include "../includes/printer.h"
#include "../includes/watchdog.h"
#include "../includes/logger.h"
#include "../includes/buffer.h"

static volatile sig_atomic_t termination_flag = ATOMIC_VAR_INIT(0);
static atomic_flag watchdog_flag = ATOMIC_FLAG_INIT;

static Reader* reader;
static Analyzer* analyzer;
static Printer* printer;
static Logger* logger;

static Buffer* reader_analyzer_buffer;
static Buffer* analyzer_printer_buffer;

static uint8_t CPUs_number;

void signal_handler(int signum);
static void* reader_thread_function(void* args);
static void* analyzer_thread_function(void* args);
static void* printer_thread_function(void* args);
static void* watchdog_thread(void* args);
static void cleanup(void);
static void thread_join_create_error(const char* message);


int cpu_tracker_run(void) {
  enum { buffer_max_size = 32 };
  ProcStatistics stats;

  pthread_t reader_thread;
  pthread_t analyzer_thread;
  pthread_t printer_thread;

  pthread_t watchdogs[3];

  reader = reader_new("/proc/stat");
  analyzer = analyzer_new();
  printer = printer_new("#");
  logger = logger_new("logs.txt");

  reader_read_latest_statistics(reader, &stats);
  CPUs_number = stats.CPUs_number;

  reader_analyzer_buffer = buffer_new(sizeof(ProcStatistics) + sizeof(CpuStatistics) * CPUs_number, buffer_max_size);
  analyzer_printer_buffer = buffer_new(sizeof(AnalysedProcStats) + sizeof(float) * CPUs_number, buffer_max_size);
  free(stats.CPUs);

  if(signal(SIGTERM, signal_handler)== SIG_ERR) return EXIT_FAILURE;

  if(signal(SIGINT, signal_handler)== SIG_ERR) return EXIT_FAILURE;

  if(watchdog_create_thread(&reader_thread, reader_thread_function, &watchdogs[0], watchdog_thread) != 0) {
    thread_join_create_error("Failed to create reader thread");
    return EXIT_FAILURE;
  }
  logger_log(logger, Info, "Reader thread created");

  if(watchdog_create_thread(&analyzer_thread, analyzer_thread_function, &watchdogs[1], watchdog_thread) != 0) {
    thread_join_create_error("Failed to create analyzer thread");
    return EXIT_FAILURE;
  }
  logger_log(logger, Info, "Analyzer thread created");

  if(watchdog_create_thread(&printer_thread, printer_thread_function, &watchdogs[2], watchdog_thread) != 0) {
    thread_join_create_error("Failed to create printer thread");
    return EXIT_FAILURE;
  }
  logger_log(logger, Info, "Printer thread created");

  if(pthread_join(reader_thread, NULL) != 0) {
    thread_join_create_error("Failed to join reader thread");
    return EXIT_FAILURE;
  }
  logger_log(logger, Info, "Reader thread finished");

  if(pthread_join(analyzer_thread, NULL) != 0) {
    thread_join_create_error("Failed to join analyzer thread");
    return EXIT_FAILURE;
  }
  logger_log(logger, Info, "Analyzer thread finished");

  if(pthread_join(printer_thread, NULL) != 0) {
    thread_join_create_error("Failed to join printer thread");
    return EXIT_FAILURE;
  }
  logger_log(logger, Info, "Printer thread finished");

  for(size_t i = 0; i < 3; i++) {
    if(pthread_join(watchdogs[i], NULL) != 0){
      thread_join_create_error("Watchdog thread join error");
      return EXIT_FAILURE;
    }
  }

  cleanup();

  return 0;
}

void signal_handler(int signum) {
  if(signum == SIGTERM || signum == SIGINT)
    termination_flag = 1;
}

static void* reader_thread_function(void* args) {
    SignalWrapper* signal = (SignalWrapper*) args;
    ProcStatistics stats;
    struct timespec sleepTime;

    while(termination_flag == 0) {   
        reader_read_latest_statistics(reader, &stats);
        if(buffer_push(reader_analyzer_buffer, &stats, 2) != SUCCESS) {
            free(stats.CPUs);
            logger_log(logger, Error, "Cannot push data to reader-analyzer buffer.");
            pthread_exit(NULL);
        }

        logger_log(logger, Info, "Data pushed to reader-analyzer buffer.");

        watchdog_send_signal(signal);
        
        sleepTime.tv_sec = 1;
        sleepTime.tv_nsec = 0;
        nanosleep(&sleepTime, NULL);
    }

    pthread_exit(NULL);
}

static void* analyzer_thread_function(void* args) {
    SignalWrapper* signal = (SignalWrapper*) args;
    ProcStatistics* stats = malloc(sizeof(ProcStatistics) + CPUs_number * sizeof(CpuStatistics));
    AnalysedProcStats analysed_stats;
    struct timespec sleepTime;

    if(stats == NULL) {
        logger_log(logger, Error, "Allocation error in analyzer thread.");
        pthread_exit(NULL);
    }

    while(termination_flag == 0) {
        if(buffer_pop(reader_analyzer_buffer, stats, 2) != SUCCESS) {
            free(stats);
            logger_log(logger, Error, "Cannot pop data from reader-analyzer buffer.");
            break;
        }

        logger_log(logger, Info, "Data poped from reader-analyzer buffer.");
        
        if(analyzer_analyse_statistics(analyzer, stats, &analysed_stats) == SUCCESS) {
          if(buffer_push(analyzer_printer_buffer, &analysed_stats, 2) != SUCCESS) {
            logger_log(logger, Error, "Cannot push data to analyzer-printer buffer.");

            free(stats->CPUs);
            free(analysed_stats.CPUs);
            break;
          }

          logger_log(logger, Info, "Data pushed to analyzer-printer buffer.");
        }

        free(stats->CPUs);
        watchdog_send_signal(signal);

        sleepTime.tv_sec = 1;
        sleepTime.tv_nsec = 0;
        nanosleep(&sleepTime, NULL);
    }
    
    free(stats);
    pthread_exit(NULL);
}

static void* printer_thread_function(void* args) {
    SignalWrapper* signal = (SignalWrapper*) args;
    AnalysedProcStats* analysed_stats = malloc(sizeof(AnalysedProcStats) + sizeof(float) * CPUs_number);
    struct timespec sleepTime;

    if(analysed_stats == NULL) {
        logger_log(logger, Error, "Allocation error in printer thread.");
        pthread_exit(NULL);
    }

    while(termination_flag == 0) {
        if(buffer_pop(analyzer_printer_buffer, analysed_stats, 2) != SUCCESS) {
            logger_log(logger, Error, "Cannot pop data from analyzer-printer buffer.");
            break;
        }

        logger_log(logger, Info, "Data poped from analyzer-printer buffer.");

        printer_print_proc_usage(printer, analysed_stats);

        free(analysed_stats->CPUs);

        watchdog_send_signal(signal);

        sleepTime.tv_sec = 1;
        sleepTime.tv_nsec = 0;
        nanosleep(&sleepTime, NULL);
    }

    free(analysed_stats);
    pthread_exit(NULL);
}

static void* watchdog_thread(void* args) {
    SignalWrapper* signal = (SignalWrapper*)args;

    struct timespec timeout;
    struct timeval now;

    pthread_mutex_lock(&signal->mutex);
    gettimeofday(&now, NULL);
    timeout.tv_sec = now.tv_sec + 2;
    timeout.tv_nsec = now.tv_usec * 1000;

    while(termination_flag == 0) {
      int result = pthread_cond_timedwait(&signal->signal_cv, &signal->mutex, &timeout);

      if(result != 0 && termination_flag == 0) {
        logger_log(logger, Info, "Logger didn't get signal from thread.");

        perror("Logger didn't get signal from thread.");

        if(!atomic_flag_test_and_set(&watchdog_flag)) {
          termination_flag = 1;
          break;
        }

      } 
      else {
        gettimeofday(&now, NULL);
        timeout.tv_sec = now.tv_sec + 2;
        timeout.tv_nsec = now.tv_usec * 1000;
      }
    }
    
    pthread_mutex_destroy(&signal->mutex);
    pthread_cond_destroy(&signal->signal_cv);
    free(signal);
    pthread_exit(NULL);
}

static void cleanup(void) {
    ProcStatistics trasher1;
    AnalysedProcStats trasher2;
    
    while(!buffer_is_empty(reader_analyzer_buffer)) {
      buffer_pop(reader_analyzer_buffer, &trasher1, 2);
      free(trasher1.CPUs);
    }

    while(!buffer_is_empty(analyzer_printer_buffer)) {
        buffer_pop(analyzer_printer_buffer, &trasher2, 2);
        free(trasher2.CPUs);
    }

    reader_delete(reader);
    analyzer_delete(analyzer);
    printer_delete(printer);
    logger_delete(logger);
    buffer_delete(reader_analyzer_buffer);
    buffer_delete(analyzer_printer_buffer);
}

static void thread_join_create_error(const char* message) {
    logger_log(logger, Debug, message);
    cleanup();
}