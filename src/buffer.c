#include <pthread.h>    
#include <stdlib.h>     
#include <string.h>     
#include <time.h>       

// Inside libraries includes
#include "../includes/buffer.h"

typedef struct Buffer {
    pthread_cond_t can_produce;
    pthread_cond_t can_consume;
    pthread_mutex_t mutex;
    size_t tail;
    size_t head;
    size_t elements_number;
    size_t capacity;
    size_t element_size;
    uint8_t buffer[];   
} Buffer;


Buffer* buffer_new(size_t data_size, size_t buffer_max_size) {
    if(data_size == 0) return NULL;

    if(buffer_max_size == 0) return NULL;

    Buffer* buffer = malloc(sizeof(*buffer) + (data_size * buffer_max_size));
    if(buffer == NULL) return NULL;

    *buffer = (Buffer){.mutex = PTHREAD_MUTEX_INITIALIZER,
      .can_consume = PTHREAD_COND_INITIALIZER,
      .can_produce = PTHREAD_COND_INITIALIZER,
      .tail = 0,
      .head = 0,
      .elements_number = 0,
      .capacity = buffer_max_size,
      .element_size = data_size,
    };

    return buffer;
}

bool buffer_is_empty(Buffer* buffer) {
    if(buffer == NULL) return false;

    if(buffer->elements_number == 0) return true;

    return false;
}

bool buffer_is_full(Buffer* buffer) {
    if(buffer == NULL) return false;

    if(buffer->elements_number == buffer->capacity) return true;

    return false;
}

Result_enum buffer_push(Buffer* buffer, void* element, uint8_t max_push_time) {
    if(buffer == NULL) return NULL_TARGET_ERROR;

    if(element == NULL) return NULL_TARGET_ERROR;

    struct timespec timeout;
    timeout.tv_sec = max_push_time;

    pthread_mutex_lock(&buffer->mutex);

    while(buffer_is_full(buffer)) {
        if (pthread_cond_timedwait(&buffer->can_produce, &buffer->mutex, &timeout) != 0) {
            pthread_mutex_unlock(&buffer->mutex);
            
            return TIMEOUT_ERROR;
        }
    }

    uint8_t* const ptr = &buffer->buffer[buffer->head * buffer->element_size];
    memcpy(ptr, element, buffer->element_size);

    buffer->elements_number++;
    buffer->head = (buffer->head + 1) % buffer->capacity;

    pthread_cond_signal(&buffer->can_consume);
    pthread_mutex_unlock(&buffer->mutex);

    return SUCCESS;
}

Result_enum buffer_pop(Buffer* buffer, void* element, uint8_t max_pop_time) {
    if(buffer == NULL) return NULL_TARGET_ERROR;

    if(element == NULL) return NULL_TARGET_ERROR;

    struct timespec timeout;
    timeout.tv_sec = max_pop_time;

    pthread_mutex_lock(&buffer->mutex);
    while (buffer_is_empty(buffer)) {
        if(pthread_cond_timedwait(&buffer->can_consume, &buffer->mutex, &timeout)!=0){
            pthread_mutex_unlock(&buffer->mutex);

            return TIMEOUT_ERROR;
        }
    }

    uint8_t * const ptr = &buffer->buffer[buffer->tail * buffer->element_size];
    memcpy(element, ptr, buffer->element_size);

    buffer->elements_number--;
    buffer->tail = (buffer->tail + 1) % buffer->capacity;

    pthread_cond_signal(&buffer->can_produce);
    pthread_mutex_unlock(&buffer->mutex);

    return SUCCESS;
}

void buffer_delete(Buffer* buffer) {
    if(buffer == NULL) return;

    pthread_mutex_destroy(&buffer->mutex);
    pthread_cond_destroy(&buffer->can_produce);
    pthread_cond_destroy(&buffer->can_consume);

    free(buffer);
}