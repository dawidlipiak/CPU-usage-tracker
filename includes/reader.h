#ifndef READER_H
#define READER_H

#include <stddef.h> 
#include "proc_stat.h"
#include "result_enums.h"

// Reader structure
typedef struct Reader Reader;


/** Create reader method
 * 
 * arguments: read_interval - time between successive file readings
 * 
 * return: pointer to created Reader 
*/ 
Reader* reader_create(size_t read_interval);



/** Read statistics
 * 
 * arguments: reader - pointer to Reader object
 *            statistics - pointer to ProcStatistics
 * 
 * return: operation result
*/
Result_return reader_read_latest_statistics(Reader* reader, ProcStatistics* statistics);



/** Reader destroyer
 * s
 *  arguments:  reader - pointer to Reader object
*/
void reader_delete(Reader* reader);

#endif 