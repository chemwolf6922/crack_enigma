#ifndef __HEAP_H
#define __HEAP_H

#include <stdbool.h>

typedef bool(*compare_t)(void* A, void* B);

typedef void* heap_handle_t;

int heap_create(heap_handle_t* p_heap,int len,int size,compare_t compare);

int heap_add(heap_handle_t heap,void* data);

void* heap_get_all(heap_handle_t heap,int* len);

int heap_delete(heap_handle_t heap);

void print_heap(heap_handle_t handle);

#endif