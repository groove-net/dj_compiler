#include "../../include/strmethods.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/* Returns true(1) if strings are identical */
int strCompare(const char *s1, const char *s2) {
  while (*s1)
    if (*s1++ != *s2++)
      return 0;
  if (*s2)
    return 0;
  else
    return 1;
}

/* Returns pointer to the concatenated string */
char *strConcat(const char *first, ...) {
  if (first == NULL)
    return NULL;

  va_list args;
  size_t total_len = strlen(first);

  // First pass: calculate total length
  va_start(args, first);
  const char *s;
  while ((s = va_arg(args, const char *)) != NULL) {
    total_len += strlen(s);
  }
  va_end(args);

  // Allocate result buffer
  char *result = malloc(total_len + 1); // +1 for null terminator
  if (result == NULL)
    return NULL;

  // Second pass: copy strings
  strcpy(result, first);
  va_start(args, first);
  while ((s = va_arg(args, const char *)) != NULL) {
    strcat(result, s);
  }
  va_end(args);

  return result;
}
