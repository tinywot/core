/*
  SPDX-FileCopyrightText: 2022-2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \internal \file
  \brief Basic, underlying internal interface implementations.
*/

#include <stdlib.h>
#include <string.h>

#include "basic.h"

__attribute__((__noreturn__, __weak__)) void
tinywot_fatal_error_handler(char const *file, size_t line, unsigned int code) {
  (void)file;
  (void)line;
  (void)code;

  while (1) {}
}

#ifdef TINYWOT_ENABLE_DYNAMIC_MEMORY
__attribute__((__weak__)) void tinywot_free(void *ptr) { free(ptr); }
__attribute__((__malloc__(tinywot_free)), __weak__) void *
tinywot_malloc(size_t size) {
  return malloc(size);
}
#endif /* ifdef TINYWOT_ENABLE_DYNAMIC_MEMORY */

__attribute__((__weak__)) int tinywot_strcmp(char const *lhs, char const *rhs) {
  return strcmp(lhs, rhs);
}
