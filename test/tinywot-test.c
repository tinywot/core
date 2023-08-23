/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \file
  \brief Implementation for the unit test supporting library of TinyWoT
  Core.
*/

#include <stdlib.h>
#include <string.h>
#include <tinywot/core.h>
#include <unity.h>

void tinywot_test_free(void *mem) {
  free(mem);
}

void *tinywot_test_mallocx(size_t size) {
  void *mem = malloc(size);
  if (!mem) {
    TEST_ABORT();
  }

  return mem;
}

void *tinywot_test_mallocd(size_t size) {
  return memset(tinywot_test_mallocx(size), 0xcd, size);
}

void *tinywot_test_malloc0(size_t size) {
  return memset(tinywot_test_mallocx(size), 0x00, size);
}

void tinywot_test_tinywot_payload_delete(struct tinywot_payload *pl) {
  tinywot_test_free(pl->content);
  tinywot_test_free(pl);
}

struct tinywot_payload *tinywot_test_tinywot_payload_new(size_t size) {
  struct tinywot_payload *pl =
    tinywot_test_malloc0(sizeof(struct tinywot_payload));

  pl->content = tinywot_test_mallocd(size);
  pl->content_buffer_size_byte = size;

  return pl;
}

void tinywot_test_tinywot_thing_delete(struct tinywot_thing *thing) {
  tinywot_test_free(thing);
}

struct tinywot_thing *tinywot_test_tinywot_thing_new(void) {
  return tinywot_test_malloc0(sizeof(struct tinywot_thing));
}

struct tinywot_thing *tinywot_test_tinywot_thing_new_example(void) {
  struct tinywot_thing *thing = tinywot_test_tinywot_thing_new();

  /* TODO */

  return thing;
}
