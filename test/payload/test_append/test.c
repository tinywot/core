/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

#include <stddef.h>
#include <string.h>
#include <tinywot/core.h>
#include <tinywot-test.h>
#include <unity.h>

static char const lorem[] = "Lorem ipsum";
static char const dolor[] = " dolor sit amet";

/* This is because payload_append does not know about strings. There
   should be a payload_append_string to solve this. */
static char const lorem_dolor[] = "Lorem ipsum\0 dolor sit amet";

static void tinywot_payload_append_should_succeed(void) {
  void *mem = tinywot_test_malloc(TINYWOT_TEST_MEMORY_SIZE_BYTE);
  struct tinywot_payload *pl =
    tinywot_test_payload_with_memory(mem, TINYWOT_TEST_MEMORY_SIZE_BYTE);
  tinywot_status_t sc = TINYWOT_STATUS_ERROR_NOT_ALLOWED;

  memset(mem, 0, TINYWOT_TEST_MEMORY_SIZE_BYTE);
  TEST_ASSERT_EQUAL_STRING("", pl->content);

  sc = tinywot_payload_append(pl, lorem, sizeof(lorem));
  TEST_ASSERT_EQUAL(sc, TINYWOT_STATUS_SUCCESS);
  TEST_ASSERT_EQUAL_STRING(lorem, pl->content);

  sc = tinywot_payload_append(pl, dolor, sizeof(dolor));
  TEST_ASSERT_EQUAL(sc, TINYWOT_STATUS_SUCCESS);
  TEST_ASSERT_EQUAL_MEMORY(lorem_dolor, pl->content, sizeof(lorem_dolor));

  tinywot_test_payload_free(pl);
}

static void tinywot_payload_append_should_fail_when_no_enough_memory(void) {
  void *mem = tinywot_test_malloc(TINYWOT_TEST_SMALL_MEMORY_SIZE_BYTE);
  struct tinywot_payload *pl =
    tinywot_test_payload_with_memory(mem, TINYWOT_TEST_SMALL_MEMORY_SIZE_BYTE);
  tinywot_status_t sc = TINYWOT_STATUS_ERROR_NOT_ALLOWED;

  memset(mem, 0, TINYWOT_TEST_SMALL_MEMORY_SIZE_BYTE);
  TEST_ASSERT_EQUAL_STRING("", pl->content);

  sc = tinywot_payload_append(pl, lorem, sizeof(lorem));
  TEST_ASSERT_EQUAL(sc, TINYWOT_STATUS_SUCCESS);
  TEST_ASSERT_EQUAL_STRING(lorem, pl->content);

  sc = tinywot_payload_append(pl, dolor, sizeof(dolor));
  TEST_ASSERT_EQUAL(sc, TINYWOT_STATUS_ERROR_NOT_ENOUGH_MEMORY);
  TEST_ASSERT_EQUAL_STRING(lorem, pl->content);

  tinywot_test_payload_free(pl);
}

void setUp(void) {}
void tearDown(void) {}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(tinywot_payload_append_should_succeed);
  RUN_TEST(tinywot_payload_append_should_fail_when_no_enough_memory);

  return UNITY_END();
}
