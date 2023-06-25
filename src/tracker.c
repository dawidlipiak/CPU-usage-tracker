#include <signal.h>     
#include <string.h>     
#include <stdatomic.h>  
#include <stdbool.h>    
#include <time.h>       
#include <stdio.h>      
#include <stdlib.h>     

#include "../includes/tracker.h"
#include "../includes/reader.h"
#include "../includes/analyzer.h"
#include "../includes/printer.h"
#include "../includes/buffer.h"

#define compare_flag(a, b) a == b

void tracker_run(void)
{
    typedef volatile sig_atomic_t flag_type;

    static flag_type termination_flag = ATOMIC_VAR_INIT(0);

    static Reader* reader;
    static Analyzer* analyzer;
    static Printer* printer;

    reader = reader_create("/proc/stat");
    analyzer = analyzer_create();
    printer = printer_create("#");

    ProcStatistics stats;
    AnalysedProcStats analysed;

    reader_read_latest_statistics(reader, &stats);
    printf("%d\n", analyzer_analyse_statistics(analyzer, &stats, &analysed));

    free(stats.CPUs);
    free(analysed.CPUs);

    for(int i = 0; i < 2; i++) {
        struct timespec sleepTime;
        sleepTime.tv_sec = 1;
        sleepTime.tv_nsec = 0;
        nanosleep(&sleepTime, NULL);

        reader_read_latest_statistics(reader, &stats);
        printf("%d\n", analyzer_analyse_statistics(analyzer, &stats, &analysed));

        printer_print_proc_usage(printer, &analysed);

        free(stats.CPUs);
        free(analysed.CPUs);
    }

    reader_delete(reader);
    analyzer_delete(analyzer);
    printer_delete(printer);
}