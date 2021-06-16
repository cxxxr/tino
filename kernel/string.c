#include "string.h"

bool string_equal(const char *str1, const char *str2) {
  int i;
  for (i = 0; str1[i] != '\0' && str2[i] != '\0'; i++) {
    if (str1[i] != str2[i]) return FALSE;
  }
  return str1[i] == '\0' && str2[i] == '\0';
}

int string_copy(char *dst, const char *src) {
  int i;
  for (i = 0; src[i] != '\0'; i++) {
    dst[i] = src[i];
  }
  return i;
}
