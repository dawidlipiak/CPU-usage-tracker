#ifndef ANALYZER_H
#define ANALYZER_H

#include "cpu_stat.h"
#include "analyzed_statistics.h"
#include "result_enums.h"

// Analyzer structure
typedef struct Analyzer Analyzer;


/** Create Analyzer
 * 
 * return: pointer to created analyzer
*/
Analyzer* analyzer_create();



/** Analyze statistics
 * 
 * arguments:   analyzer - pointer to Analyzer object
 *              row_statistics - pointer to row statistics structure
 *              analysed_statistics - pointer to analysed statistics structure
 * 
 * return: Operation result
*/
Result_enum analyzer_analyse_stats(Analyzer* analyzer, ProcStatistics* row_statistics, AnalysedProcStats* analysed_statistics);


/** Analyzer destroyer
 * 
 *  arguments:  analyzer - pointer to Analyzer object
*/
void analyzer_delete(Analyzer* analyzer);

#endif 