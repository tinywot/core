/*
  SPDX-FileCopyrightText: 2022 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \internal \file
  \brief TinyWoT internal API declarations.
*/

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"

__attribute__((__noreturn__, __weak__)) void
tinywot_fatal_error_handler(char const *file, size_t line, unsigned int code) {
  (void)file;
  (void)line;
  (void)code;

  while (true) {}
}

#ifdef TINYWOT_ENABLE_CONTRACTS
__attribute__((__weak__)) void tinywot_contract_check(
  bool cond, char const *file, size_t line, unsigned int code
) {
  if (!cond) {
    tinywot_fatal_error_handler(file, line, code);
  }
}
#endif /* ifdef TINYWOT_ENABLE_CONTRACTS */

#ifdef TINYWOT_ENABLE_DYNAMIC_MEMORY
__attribute__((__weak__)) void *tinywot_malloc(size_t size) {
  return malloc(size);
}

__attribute__((__weak__)) void tinywot_free(void *ptr) { free(ptr); }
#endif /* ifdef TINYWOT_ENABLE_DYNAMIC_MEMORY */

__attribute__((__weak__)) int tinywot_strcmp(char const *lhs, char const *rhs) {
  return strcmp(lhs, rhs);
}
