/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

#include <stdlib.h>
#include <tinywot.h>
#include <unity.h>

void test_tinywot_scratchpad_initialize_normal(void) {
  struct tinywot_scratchpad scratchpad;

  tinywot_scratchpad_initialize(&scratchpad);

  TEST_ASSERT_EQUAL_UINT(0, scratchpad.read_write);
  TEST_ASSERT_EQUAL_UINT(TINYWOT_CONTENT_TYPE_UNKNOWN, scratchpad.type_hint);
  TEST_ASSERT_EQUAL_UINT(0, scratchpad.size_byte);
  TEST_ASSERT_EQUAL_UINT(0, scratchpad.valid_byte);
  TEST_ASSERT_NULL(scratchpad.data);
  TEST_ASSERT_NULL(scratchpad.update);
}

void test_tinywot_scratchpad_split_normal(void) {
  struct tinywot_scratchpad scratchpad_1 = tinywot_scratchpad_new();
  struct tinywot_scratchpad scratchpad_2 = tinywot_scratchpad_new();
  void *memory = malloc(256);
  int r = 0;

  if (!memory) {
    TEST_ABORT();
  }

  scratchpad_1.read_write = 1;
  scratchpad_1.size_byte = 256;
  scratchpad_1.data = memory;
  scratchpad_2.read_write = 1;

  r = tinywot_scratchpad_split(&scratchpad_1, &scratchpad_2, 96);

  TEST_ASSERT_EQUAL_INT(TINYWOT_SUCCESS, r);

  TEST_ASSERT_EQUAL_UINT(1, scratchpad_1.read_write);
  TEST_ASSERT_EQUAL_UINT(TINYWOT_CONTENT_TYPE_UNKNOWN, scratchpad_1.type_hint);
  TEST_ASSERT_EQUAL_UINT(160, scratchpad_1.size_byte);
  TEST_ASSERT_EQUAL_UINT(0, scratchpad_1.valid_byte);
  TEST_ASSERT_NOT_NULL(scratchpad_1.data);
  TEST_ASSERT_NULL(scratchpad_1.update);

  TEST_ASSERT_EQUAL_UINT(1, scratchpad_2.read_write);
  TEST_ASSERT_EQUAL_UINT(TINYWOT_CONTENT_TYPE_UNKNOWN, scratchpad_2.type_hint);
  TEST_ASSERT_EQUAL_UINT(96, scratchpad_2.size_byte);
  TEST_ASSERT_EQUAL_UINT(0, scratchpad_2.valid_byte);
  TEST_ASSERT_NOT_NULL(scratchpad_2.data);
  TEST_ASSERT_NULL(scratchpad_2.update);

  free(memory);
}

void test_tinywot_scratchpad_split_left_read_only(void) {
  struct tinywot_scratchpad scratchpad_1 = tinywot_scratchpad_new();
  struct tinywot_scratchpad scratchpad_2 = tinywot_scratchpad_new();
  void *memory = malloc(256);
  int r = 0;

  if (!memory) {
    TEST_ABORT();
  }

  scratchpad_1.size_byte = 256;
  scratchpad_1.data = memory;
  scratchpad_2.read_write = 1;

  TEST_ASSERT_EQUAL_UINT(0, scratchpad_1.read_write);

  r = tinywot_scratchpad_split(&scratchpad_1, &scratchpad_2, 96);

  TEST_ASSERT_EQUAL_INT(TINYWOT_ERROR_NOT_ALLOWED, r);

  /* Scratchpads should not be touched. */

  TEST_ASSERT_EQUAL_UINT(0, scratchpad_1.read_write);
  TEST_ASSERT_EQUAL_UINT(TINYWOT_CONTENT_TYPE_UNKNOWN, scratchpad_1.type_hint);
  TEST_ASSERT_EQUAL_UINT(256, scratchpad_1.size_byte);
  TEST_ASSERT_EQUAL_UINT(0, scratchpad_1.valid_byte);
  TEST_ASSERT_NOT_NULL(scratchpad_1.data);
  TEST_ASSERT_NULL(scratchpad_1.update);

  TEST_ASSERT_EQUAL_UINT(1, scratchpad_2.read_write);
  TEST_ASSERT_EQUAL_UINT(TINYWOT_CONTENT_TYPE_UNKNOWN, scratchpad_2.type_hint);
  TEST_ASSERT_EQUAL_UINT(0, scratchpad_2.size_byte);
  TEST_ASSERT_EQUAL_UINT(0, scratchpad_2.valid_byte);
  TEST_ASSERT_NULL(scratchpad_2.data);
  TEST_ASSERT_NULL(scratchpad_2.update);

  free(memory);
}

void test_tinywot_scratchpad_split_right_read_only(void) {
  struct tinywot_scratchpad scratchpad_1 = tinywot_scratchpad_new();
  struct tinywot_scratchpad scratchpad_2 = tinywot_scratchpad_new();
  void *memory = malloc(256);
  int r = 0;

  if (!memory) {
    TEST_ABORT();
  }

  scratchpad_1.read_write = 1;
  scratchpad_1.size_byte = 256;
  scratchpad_1.data = memory;

  TEST_ASSERT_EQUAL_UINT(0, scratchpad_2.read_write);

  r = tinywot_scratchpad_split(&scratchpad_1, &scratchpad_2, 96);

  TEST_ASSERT_EQUAL_INT(TINYWOT_ERROR_NOT_ALLOWED, r);

  /* Scratchpads should not be touched. */

  TEST_ASSERT_EQUAL_UINT(1, scratchpad_1.read_write);
  TEST_ASSERT_EQUAL_UINT(TINYWOT_CONTENT_TYPE_UNKNOWN, scratchpad_1.type_hint);
  TEST_ASSERT_EQUAL_UINT(256, scratchpad_1.size_byte);
  TEST_ASSERT_EQUAL_UINT(0, scratchpad_1.valid_byte);
  TEST_ASSERT_NOT_NULL(scratchpad_1.data);
  TEST_ASSERT_NULL(scratchpad_1.update);

  TEST_ASSERT_EQUAL_UINT(0, scratchpad_2.read_write);
  TEST_ASSERT_EQUAL_UINT(TINYWOT_CONTENT_TYPE_UNKNOWN, scratchpad_2.type_hint);
  TEST_ASSERT_EQUAL_UINT(0, scratchpad_2.size_byte);
  TEST_ASSERT_EQUAL_UINT(0, scratchpad_2.valid_byte);
  TEST_ASSERT_NULL(scratchpad_2.data);
  TEST_ASSERT_NULL(scratchpad_2.update);

  free(memory);
}

void test_tinywot_scratchpad_split_not_enough_size(void) {
  struct tinywot_scratchpad scratchpad_1 = tinywot_scratchpad_new();
  struct tinywot_scratchpad scratchpad_2 = tinywot_scratchpad_new();
  void *memory = malloc(32);
  int r = 0;

  if (!memory) {
    TEST_ABORT();
  }

  scratchpad_1.read_write = 1;
  scratchpad_1.size_byte = 32;
  scratchpad_1.data = memory;
  scratchpad_2.read_write = 1;

  /* 96 (to_split_byte) > 32 (scratchpad_1.size_byte) */
  r = tinywot_scratchpad_split(&scratchpad_1, &scratchpad_2, 96);

  TEST_ASSERT_EQUAL_INT(TINYWOT_ERROR_NOT_ENOUGH_MEMORY, r);

  /* Scratchpads should not be touched. */

  TEST_ASSERT_EQUAL_UINT(1, scratchpad_1.read_write);
  TEST_ASSERT_EQUAL_UINT(TINYWOT_CONTENT_TYPE_UNKNOWN, scratchpad_1.type_hint);
  TEST_ASSERT_EQUAL_UINT(32, scratchpad_1.size_byte);
  TEST_ASSERT_EQUAL_UINT(0, scratchpad_1.valid_byte);
  TEST_ASSERT_NOT_NULL(scratchpad_1.data);
  TEST_ASSERT_NULL(scratchpad_1.update);

  TEST_ASSERT_EQUAL_UINT(1, scratchpad_2.read_write);
  TEST_ASSERT_EQUAL_UINT(TINYWOT_CONTENT_TYPE_UNKNOWN, scratchpad_2.type_hint);
  TEST_ASSERT_EQUAL_UINT(0, scratchpad_2.size_byte);
  TEST_ASSERT_EQUAL_UINT(0, scratchpad_2.valid_byte);
  TEST_ASSERT_NULL(scratchpad_2.data);
  TEST_ASSERT_NULL(scratchpad_2.update);

  free(memory);
}

void test_tinywot_scratchpad_split_not_enough_valid(void) {
  struct tinywot_scratchpad scratchpad_1 = tinywot_scratchpad_new();
  struct tinywot_scratchpad scratchpad_2 = tinywot_scratchpad_new();
  void *memory = malloc(256);
  int r = 0;

  if (!memory) {
    TEST_ABORT();
  }

  scratchpad_1.read_write = 1;
  scratchpad_1.size_byte = 256;
  scratchpad_1.valid_byte = 192;
  scratchpad_1.data = memory;
  scratchpad_2.read_write = 1;

  /* 96 (to_split_byte) > 256 - 192 (64) */
  r = tinywot_scratchpad_split(&scratchpad_1, &scratchpad_2, 96);

  TEST_ASSERT_EQUAL_INT(TINYWOT_ERROR_NOT_ENOUGH_MEMORY, r);

  /* Scratchpads should not be touched. */

  TEST_ASSERT_EQUAL_UINT(1, scratchpad_1.read_write);
  TEST_ASSERT_EQUAL_UINT(TINYWOT_CONTENT_TYPE_UNKNOWN, scratchpad_1.type_hint);
  TEST_ASSERT_EQUAL_UINT(256, scratchpad_1.size_byte);
  TEST_ASSERT_EQUAL_UINT(192, scratchpad_1.valid_byte);
  TEST_ASSERT_NOT_NULL(scratchpad_1.data);
  TEST_ASSERT_NULL(scratchpad_1.update);

  TEST_ASSERT_EQUAL_UINT(1, scratchpad_2.read_write);
  TEST_ASSERT_EQUAL_UINT(TINYWOT_CONTENT_TYPE_UNKNOWN, scratchpad_2.type_hint);
  TEST_ASSERT_EQUAL_UINT(0, scratchpad_2.size_byte);
  TEST_ASSERT_EQUAL_UINT(0, scratchpad_2.valid_byte);
  TEST_ASSERT_NULL(scratchpad_2.data);
  TEST_ASSERT_NULL(scratchpad_2.update);

  free(memory);
}
