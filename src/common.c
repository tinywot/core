/*
  SPDX-FileCopyrightText: 2022 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \internal \file
*/

#include <stdlib.h>
#include <string.h>

#include "common.h"

__attribute__((__weak__)) int tinywot_strcmp(const char *lhs, const char *rhs) {
  return strcmp(lhs, rhs);
}

__attribute__((__weak__)) void *tinywot_malloc(size_t size) {
  return malloc(size);
}

__attribute__((__weak__)) void tinywot_free(void *ptr) { free(ptr); }
