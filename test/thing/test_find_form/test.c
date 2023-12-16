/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \file
  \brief Unit tests for `tinywot_thing_find_form()`.
*/

#include <stddef.h>
#include <string.h>
#include <tinywot/core.h>
#include <tinywot-test.h>
#include <unity.h>

static void tinywot_thing_find_form_should_succeed(void) {
  enum tinywot_status status = TINYWOT_STATUS_ERROR_GENERIC;
  struct tinywot_thing *thing = tinywot_test_thing_new_example();
  struct tinywot_form *form = NULL;

  status = tinywot_thing_find_form(
    thing, &form, "/status", TINYWOT_OPERATION_TYPE_READPROPERTY
  );

  TEST_ASSERT_EQUAL(TINYWOT_STATUS_SUCCESS, status);
  TEST_ASSERT_NOT_NULL(form);

  tinywot_test_thing_delete(thing);
}

static void tinywot_thing_find_form_should_fail_when_target_mismatch(void) {
  enum tinywot_status status = TINYWOT_STATUS_ERROR_GENERIC;
  struct tinywot_thing *thing = tinywot_test_thing_new_example();
  struct tinywot_form *form = NULL;

  status = tinywot_thing_find_form(
    thing, &form, "/lorem", TINYWOT_OPERATION_TYPE_INVOKEACTION
  );

  TEST_ASSERT_EQUAL(TINYWOT_STATUS_ERROR_NOT_FOUND, status);
  TEST_ASSERT_NULL(form);

  tinywot_test_thing_delete(thing);
}

static void tinywot_thing_find_form_should_fail_when_op_mismatch(void) {
  enum tinywot_status status = TINYWOT_STATUS_ERROR_GENERIC;
  struct tinywot_thing *thing = tinywot_test_thing_new_example();
  struct tinywot_form *form = NULL;

  status = tinywot_thing_find_form(
    thing, &form, "/status", TINYWOT_OPERATION_TYPE_INVOKEACTION
  );

  TEST_ASSERT_EQUAL(TINYWOT_STATUS_ERROR_NOT_ALLOWED, status);
  TEST_ASSERT_NULL(form);

  tinywot_test_thing_delete(thing);
}

void setUp(void) {}
void tearDown(void) {}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(tinywot_thing_find_form_should_succeed);
  RUN_TEST(tinywot_thing_find_form_should_fail_when_target_mismatch);
  RUN_TEST(tinywot_thing_find_form_should_fail_when_op_mismatch);

  return UNITY_END();
}
