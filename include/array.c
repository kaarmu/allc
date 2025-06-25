#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>

#include "allocator.h"

struct allc_slice_s {
  void *data;
  size_t stride;
  size_t length;
};

void *allc_slice_at(const struct allc_slice_s slice, ssize_t i) {
  return slice.data + (i < 0 ? slice.length - i : i) * slice.stride;
}

void allc_slice_clear(const struct allc_slice_s slice) {
  memset(slice.data, 0, slice.length * slice.stride);
}

struct allc_array_s {
  struct allc_slice_s
      slice;       // length = number of elements this array currently contains
  size_t capacity; // number of elements this array could fit
  allc_allocator_t allocator;
};

typedef struct allc_array_s Array;

Array allc_array_new(allc_allocator_t allocator, size_t element_size, size_t length) {
  return (Array){
      .slice.data = allocator.alloc(element_size * length),
      .slice.length = length,
      .slice.stride = element_size,
      .allocator = allocator,
  };
}

Array allc_array_copy(const Array *self) {
  Array other =
      allc_array_new(self->allocator, self->slice.stride, self->slice.length);
  memcpy(other.slice.data, self->slice.data,
         self->slice.length * self->slice.stride);
  return other;
}

void allc_array_delete(const Array self) {
  self.allocator.free(self.slice.data);
}

void allc_array_grow(Array *self, size_t n) {
  self->capacity += n;
  self->slice.data = self->allocator.realloc(
      self->slice.data, self->capacity * self->slice.stride);
}

void allc_array_ensure_capacity(Array *self, size_t n) {
  if (self->capacity < n) {
    allc_array_grow(self, n - self->capacity);
  }
}

void allc_array_clear(Array *self) { allc_slice_clear(self->slice); }

void *allc_array_at(const Array *self, ssize_t i) {
  return allc_slice_at(self->slice, i);
}

typedef struct iterator_s {
  void *container;
  void *element;
  void (*next)(struct iterator_s *);
  bool (*cond)(struct iterator_s *);
} Iterator;

void allc_array__iterator_next(Iterator *it) {}

Iterator allc_array_iterator(Array *self) { return (Iterator){}; }

int main() {
  allc_allocator_t allocator = allc_allocator_c();
  Array arr = allc_array_new(allocator, sizeof(int), 2);

  for (Iterator it = allc_array_iterator(&arr); it.cond(&it); it.next(&it)) {
  }
}
