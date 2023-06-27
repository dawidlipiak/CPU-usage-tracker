#ifndef LOGGER_H
#define LOGGER_H

// Enum for log tags
typedef enum {
    Debug,
    Info,
    Error
} LogTag;


// Logger object structure
typedef struct Logger Logger;


/** Create new Logger
 * 
 * return: pointer to a new Logger object
*/
Logger* logger_create(char* logs_file_path);



/** Print processor statistics
 * 
 * arguments: logger - pointer to a Logger object
 *            tag - message tag for logging
 *            message - message to log
*/
void logger_log(Logger* logger, LogTag tag, const char* message);



/** Logger destroyer
 * 
 * arguments: logger - pointer to a Logger object
 */
void logger_delete(Logger* logger);

#endif