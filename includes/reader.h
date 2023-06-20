#ifndef READER_H
#define READER_H

#include <stddef.h> 
#include "cpu_stat.h"
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
 *            stats - pointer to ProcStatistics
 * 
 * return: operation result
*/
Result_enum reader_read_latest_statistics(Reader* reader, ProcStatistics* stats);



/** Reader destroyer
 * 
 *  arguments:  reader - pointer to Reader object
*/
void reader_delete(Reader* reader);

#endif 