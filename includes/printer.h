#ifndef PRINTER_H
#define PRINTER_H

#include "analyzed_statistics.h"

// Printer Object structure
typedef struct Printer Printer;

/** Create new Printer object
 * 
 * arguments: usage_bar_char - symbol that will be used for showing cpu usage bar
 */ 
Printer* printer_create(char* usage_bar_char);

/** Printing analysed processor statistics
 * 
 * arguments: printer - pointer to a Printer Object
 *            stats - pointer to a Analyzed Processor statiscitc object
*/
void printer_print_proc_usage(Printer* printer, AnalysedProcStats* stats);


/** Printer destroyer
 * 
 *  arguments:  printer - pointer to Printer object
*/
void printer_delete(Printer* printer);

#endif 