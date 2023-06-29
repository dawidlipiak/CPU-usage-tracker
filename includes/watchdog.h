#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <pthread.h>

// Signal Wrapper object structure
typedef struct SignalWrapper{
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t signal_cv;
} SignalWrapper;



/** Create watchdog thread
 * 
 * arguments: thread -  pointer to the thread instance
 *            th_fun - thread function 
 *            wd_thread - watchdog thread 
 *            watchdog_func - watchdog function
 * 
 * return 0 if everything is gone correctly, otherwise -1
 */
int watchdog_create_thread(pthread_t* thread, void* (*th_fun)(void*), pthread_t* wd_thread, void* (*watchdog_func) (void*));


/** Send signal  
 *  arguments: signal - signal to be sent
*/
void watchdog_send_signal(SignalWrapper* signal);

#endif