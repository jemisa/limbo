// vim:filetype=c:textwidth=80:shiftwidth=4:softtabstop=4:expandtab
#include "vector.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define INIT_SIZE 4
#define RESIZE_FACTOR 2

vector_t *vector_new(void)
{
    vector_t *vec = malloc(sizeof(vector_t));
    vector_init(vec);
    return vec;
}

void vector_init(vector_t *vec)
{
    vec->array = malloc(INIT_SIZE * sizeof(void *));
    vec->capacity = INIT_SIZE;
    vec->length = 0;
}

void vector_free(vector_t *vec)
{
    if (vec->array != NULL) {
        free(vec->array);
    }
    vec->array = NULL;
}

void vector_copy(vector_t *dst, const vector_t *src)
{
    if (dst->capacity < src->length) {
        dst->array = realloc(dst->array, src->capacity * sizeof(void *));
    }
    memcpy(dst->array, src->array, src->length * sizeof(void *));
    dst->capacity = src->capacity;
    dst->length = src->length;
}

void vector_copy_range(vector_t *dst, const vector_t *src, int from, int to)
{
    if (dst->capacity < to - from) {
        dst->array = realloc(dst->array, src->capacity * sizeof(void *));
    }
    memcpy(dst->array, src->array + from, (to - from) * sizeof(void *));
    dst->capacity = src->capacity;
    dst->length = src->length;
}

const void *vector_get(const vector_t *vec, int index)
{
    return vec->array[index];
}

bool vector_eq(const vector_t *vec1, const vector_t *vec2, int (*compar)(const void *, const void *))
{
    if (vec1->length != vec2->length) {
        return false;
    }
    if (compar != NULL) {
        int i = vec1->length;
        while (--i >= 0) {
            if (vec1->array[i] != vec2->array[i] && compar(vec1->array[i], vec2->array[i]) != 0) {
                return false;
            }
        }
        return true;
    } else {
        return memcmp(vec1->array, vec2->array, vec1->length * sizeof(void *)) == 0;
    }
}

int vector_size(const vector_t *vec)
{
    return vec->length;
}

void vector_prepend(vector_t *vec, const void *elem)
{
    vector_insert(vec, 0, elem);
}

void vector_append(vector_t *vec, const void *elem)
{
    vector_insert(vec, vec->length, elem);
}

void vector_insert(vector_t *vec, int index, const void *elem)
{
    assert(0 <= index && index <= vec->length);
    if (index >= vec->capacity) {
        vec->capacity *= RESIZE_FACTOR;
        vec->array = realloc(vec->array, vec->capacity * sizeof(void *));
    }
    memmove(vec->array + index + 1, vec->array + index, (vec->length - index) * sizeof(void *));
    vec->array[index] = elem;
    ++vec->length;
}

void vector_prepend_all(vector_t *vec, const vector_t *elems)
{
    vector_insert_all(vec, 0, elems);
}

void vector_append_all(vector_t *vec, const vector_t *elems)
{
    vector_insert_all(vec, vec->length, elems);
}

void vector_insert_all(vector_t *vec, int index, const vector_t *elems)
{
    assert(0 <= index && index <= vec->length);
    while (index + elems->length >= vec->capacity) {
        vec->capacity *= RESIZE_FACTOR;
        vec->array = realloc(vec->array, vec->capacity * sizeof(void *));
    }
    memmove(vec->array + index + elems->length, vec->array + index, (vec->length - index) * sizeof(void *));
    memcpy(vec->array + index, elems->array, elems->length * sizeof(void *));
    vec->length += elems->length;
}

const void *vector_remove(vector_t *vec, int index)
{
    const void *elem;
    assert(0 <= index && index < vec->length);
    elem = vec->array[index];
    memmove(vec->array + index, vec->array + index + 1, vec->length - index - 1);
    return elem;
}

