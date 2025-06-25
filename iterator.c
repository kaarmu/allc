typedef struct iterator_s {
  void *object;
  struct iterator_s (*next)();
} Iterator;
