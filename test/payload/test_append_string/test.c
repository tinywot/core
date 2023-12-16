/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \file
  \brief Unit tests for `tinywot_payload_append_string()`.
*/

#include <stddef.h>
#include <string.h>
#include <tinywot/core.h>
#include <tinywot-test.h>
#include <unity.h>

static char const lipsum[] = "Lorem ipsum";
static char const lipsum_b[] = "Lorem ipsum\0\0\0\0\0\0";
static char const dsamet[] = " dolor sit amet";
static char const lidsamet[] = "Lorem ipsum dolor sit amet";

static void tinywot_payload_append_string_should_succeed(void) {
  struct tinywot_payload *pl =
    tinywot_test_payload_new(TINYWOT_TEST_MEMORY_SIZE_BYTE);
  enum tinywot_status sc = TINYWOT_STATUS_ERROR_GENERIC;

  /* initial state */
  TEST_ASSERT_NOT_NULL(pl);
  TEST_ASSERT_NOT_NULL(pl->content);
  TEST_ASSERT_EQUAL_UINT(
    TINYWOT_TEST_MEMORY_SIZE_BYTE, pl->content_buffer_size_byte
  );
  TEST_ASSERT_EQUAL_UINT(0U, pl->content_length_byte);
  TEST_ASSERT_EQUAL_UINT(0U, pl->content_type);

  /* ++ 'Lorem ipsum' */
  sc = tinywot_payload_append_string(pl, lipsum);
  TEST_ASSERT_EQUAL(sc, TINYWOT_STATUS_SUCCESS);
  TEST_ASSERT_EQUAL_STRING(lipsum, pl->content);
  TEST_ASSERT_EQUAL_UINT(
    TINYWOT_TEST_MEMORY_SIZE_BYTE, pl->content_buffer_size_byte
  );
  TEST_ASSERT_EQUAL_UINT(sizeof(lipsum), pl->content_length_byte);
  TEST_ASSERT_EQUAL_UINT(0U, pl->content_type);

  /* ++ ' dolor sit amet' */
  sc = tinywot_payload_append_string(pl, dsamet);
  TEST_ASSERT_EQUAL(sc, TINYWOT_STATUS_SUCCESS);
  TEST_ASSERT_EQUAL_STRING(lidsamet, pl->content);
  TEST_ASSERT_EQUAL_UINT(
    TINYWOT_TEST_MEMORY_SIZE_BYTE, pl->content_buffer_size_byte
  );
  TEST_ASSERT_EQUAL_UINT(sizeof(lidsamet), pl->content_length_byte);
  TEST_ASSERT_EQUAL_UINT(0U, pl->content_type);

  tinywot_test_payload_delete(pl);
}

static void
tinywot_payload_append_string_should_succeed_with_many_trailing_nuls_in_lhs(
  void
) {
  struct tinywot_payload *pl =
    tinywot_test_payload_new(TINYWOT_TEST_MEMORY_SIZE_BYTE);
  enum tinywot_status sc = TINYWOT_STATUS_ERROR_GENERIC;

  /* initial state */
  TEST_ASSERT_NOT_NULL(pl);
  TEST_ASSERT_NOT_NULL(pl->content);
  TEST_ASSERT_EQUAL_UINT(
    TINYWOT_TEST_MEMORY_SIZE_BYTE, pl->content_buffer_size_byte
  );
  TEST_ASSERT_EQUAL_UINT(0U, pl->content_length_byte);
  TEST_ASSERT_EQUAL_UINT(0U, pl->content_type);

  /* ++ 'Lorem ipsum\0\0\0\0\0\0\0' */
  sc = tinywot_payload_append(pl, lipsum_b, sizeof(lipsum_b));
  TEST_ASSERT_EQUAL(sc, TINYWOT_STATUS_SUCCESS);
  TEST_ASSERT_EQUAL_MEMORY(lipsum_b, pl->content, sizeof(lipsum_b));
  TEST_ASSERT_EQUAL_UINT(
    TINYWOT_TEST_MEMORY_SIZE_BYTE, pl->content_buffer_size_byte
  );
  TEST_ASSERT_EQUAL_UINT(sizeof(lipsum_b), pl->content_length_byte);
  TEST_ASSERT_EQUAL_UINT(0U, pl->content_type);

  /* ++ ' dolor sit amet' */
  sc = tinywot_payload_append_string(pl, dsamet);
  TEST_ASSERT_EQUAL(sc, TINYWOT_STATUS_SUCCESS);
  TEST_ASSERT_EQUAL_STRING(lidsamet, pl->content);
  TEST_ASSERT_EQUAL_UINT(
    TINYWOT_TEST_MEMORY_SIZE_BYTE, pl->content_buffer_size_byte
  );
  TEST_ASSERT_EQUAL_UINT(sizeof(lidsamet), pl->content_length_byte);
  TEST_ASSERT_EQUAL_UINT(0U, pl->content_type);

  tinywot_test_payload_delete(pl);
}

static void tinywot_payload_append_string_should_fail_when_no_enough_memory(
  void
) {
  struct tinywot_payload *pl =
    tinywot_test_payload_new(TINYWOT_TEST_SMALL_MEMORY_SIZE_BYTE);
  enum tinywot_status sc = TINYWOT_STATUS_ERROR_GENERIC;

  /* initial state */
  TEST_ASSERT_NOT_NULL(pl);
  TEST_ASSERT_NOT_NULL(pl->content);
  TEST_ASSERT_EQUAL_UINT(
    TINYWOT_TEST_SMALL_MEMORY_SIZE_BYTE, pl->content_buffer_size_byte
  );
  TEST_ASSERT_EQUAL_UINT(0U, pl->content_length_byte);
  TEST_ASSERT_EQUAL_UINT(0U, pl->content_type);

  /* ++ 'Lorem ipsum' */
  sc = tinywot_payload_append_string(pl, lipsum);
  TEST_ASSERT_EQUAL(sc, TINYWOT_STATUS_SUCCESS);
  TEST_ASSERT_EQUAL_STRING(lipsum, pl->content);
  TEST_ASSERT_EQUAL_UINT(
    TINYWOT_TEST_SMALL_MEMORY_SIZE_BYTE, pl->content_buffer_size_byte
  );
  TEST_ASSERT_EQUAL_UINT(sizeof(lipsum), pl->content_length_byte);
  TEST_ASSERT_EQUAL_UINT(0U, pl->content_type);

  /* ++ ' dolor sit amet' (fail) */
  sc = tinywot_payload_append_string(pl, dsamet);
  TEST_ASSERT_EQUAL(sc, TINYWOT_STATUS_ERROR_NOT_ENOUGH_MEMORY);
  TEST_ASSERT_EQUAL_STRING(lipsum, pl->content);
  TEST_ASSERT_EQUAL_UINT(
    TINYWOT_TEST_SMALL_MEMORY_SIZE_BYTE, pl->content_buffer_size_byte
  );
  TEST_ASSERT_EQUAL_UINT(sizeof(lipsum), pl->content_length_byte);
  TEST_ASSERT_EQUAL_UINT(0U, pl->content_type);

  tinywot_test_payload_delete(pl);
}

void setUp(void) {}
void tearDown(void) {}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(tinywot_payload_append_string_should_succeed);
  RUN_TEST(
    tinywot_payload_append_string_should_succeed_with_many_trailing_nuls_in_lhs
  );
  RUN_TEST(tinywot_payload_append_string_should_fail_when_no_enough_memory);

  return UNITY_END();
}
