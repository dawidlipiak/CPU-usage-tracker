#include <stdio.h>      
#include <assert.h>     

#include "buffer_test.h"
#include "../includes/buffer.h"

static void test_lifecycle(void);
static void test_operations(void);

void test_buffer(void) {
    test_lifecycle();
    test_operations();
}

static void test_lifecycle(void) {
    enum { buffer_max_size = 1024 };
    Buffer* buffer;

    buffer = buffer_new(0, buffer_max_size);
    assert(buffer == NULL);
    printf("Zero Data Size Test Passed!\n");

    buffer = buffer_new(sizeof(int), 0);
    assert(buffer == NULL);
    printf("Zero Buffer Size Test Passed!\n");

    buffer = buffer_new(sizeof(int), buffer_max_size);
    assert(buffer != NULL);
    printf("Create Buffer Test Passed!\n");

    buffer_delete(buffer);
}

static void test_operations(void) {
    enum { buffer_max_size = 3 };
    enum { timeout = 1 };

    Buffer* buffer = buffer_new(sizeof(int), buffer_max_size);
    assert(buffer != NULL);
    assert(buffer_is_empty(buffer) == true);
    assert(buffer_is_full(buffer) == false);
    printf("Empty Buffer Test Passed!\n");

    int value;

    value = 1;
    buffer_push(buffer, &value, timeout);
    value = 2;
    buffer_push(buffer, &value, timeout);
    value = 3;
    buffer_push(buffer, &value, timeout);
    printf("Push To Buffer Test Passed!\n");

    assert(buffer_is_empty(buffer) == false);
    assert(buffer_is_full(buffer) == true);
    printf("Full Buffer Test Passed!\n");

    buffer_pop(buffer, &value, timeout);
    assert(value == 1);
    buffer_pop(buffer, &value, timeout);
    assert(value == 2);
    buffer_pop(buffer, &value, timeout);
    assert(value == 3);

    assert(buffer_is_empty(buffer) == true);
    assert(buffer_is_full(buffer) == false);

    printf("Pop From Buffer Test Passed!\n");

    buffer_delete(buffer);
}