#ifndef ANALYZER_Hanalyzer
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



/** Analyse statistics
 * 
 * arguments:   analyzer - pointer to Analyzer object
 *              row_stats - pointer to row stats structure
 *              analysed_stats - pointer to analysed statistics structure
 * 
 * return: Operation result
*/
Result_enum analyzer_analyse_stats(Analyzer* analyzer, ProcStatistics* row_stats, AnalysedProcStats* analysed_stats);


/** Analyzer destroyer
 * 
 *  arguments:  analyzer - pointer to Analyzer object
*/
void analyzer_delete(Analyzer* analyzer);

#endif 