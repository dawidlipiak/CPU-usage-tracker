#ifndef BUFFER_C
#define BUFFER_C

#include <stdbool.h>    
#include <stdint.h>     
#include <stddef.h>     

#include "result_enums.h"

// Thread safety Buffer structure
typedef struct Buffer Buffer;


/** Create new buffer
 * 
 * arguments: data_size - size of stored data
 *            buffer_max_size - maximal buffer capasity
 * 
 * return: pointer to created Buffer
*/
Buffer* buffer_create(size_t data_size, size_t buffer_max_size);



/** Check if buffer is empty
 * 
 * arguments: buffer - pointer to buffer
 * 
 * return: true if buffer is empty, false otherwise
*/
bool buffer_is_empty(Buffer* buffer);



/** Check if buffer is full
 * 
 * arguments: buffer - pointer to buffer
 * 
 * return: true if buffer is full, false otherwise
*/
bool buffer_is_full(Buffer* buffer);




/** Add new element into the buffer
 * 
 * arguments: buffer - pointer to a Buffer object in which we add a new element
 *            element - pointer to element to add
 *            max_push_time - maximum operation time
 * 
 * return: Operation result
*/
Result_enum buffer_push(Buffer* buffer, void* element, uint8_t max_push_time);



/** Extract element from buffer
 * 
 * arguments: buffer - pointer to buffer to extract element
 *            element - pointer to data to save extracted elemnt
 *            max_pop_time - maximum operation time
 * 
 * return: Operation result
*/
Result_enum buffer_pop(Buffer* buffer, void* element, uint8_t max_pop_time);

/** Buffer destroyer
 * 
 *  arguments:  buffer - pointer to Buffer object to delete
*/
void buffer_delete(Buffer* buffer);

#endif 