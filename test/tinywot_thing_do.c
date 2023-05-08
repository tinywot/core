/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

#include "example.h"
#include <tinywot.h>
#include <unity.h>

void test_tinywot_thing_do_normal(void) {
  struct tinywot_thing *thing = example_thing_new();
  struct tinywot_payload payload = {0};
  tinywot_status_t status = TINYWOT_ERROR_GENERAL;

  status = tinywot_thing_do(
    thing, "/a0", TINYWOT_OPERATION_TYPE_READPROPERTY, &payload
  );

  TEST_ASSERT_EQUAL(TINYWOT_SUCCESS, status);

  example_thing_free(thing);
}

void test_tinywot_thing_do_mismatch_opty(void) {
  struct tinywot_thing *thing = example_thing_new();
  struct tinywot_payload payload = {0};
  tinywot_status_t status = TINYWOT_ERROR_GENERAL;

  status = tinywot_thing_do(
    thing, "/a0", TINYWOT_OPERATION_TYPE_INVOKEACTION, &payload
  );

  TEST_ASSERT_EQUAL(TINYWOT_ERROR_NOT_ALLOWED, status);

  example_thing_free(thing);
}

void test_tinywot_thing_do_mismatch_target(void) {
  struct tinywot_thing *thing = example_thing_new();
  struct tinywot_payload payload = {0};
  tinywot_status_t status = TINYWOT_ERROR_GENERAL;

  status = tinywot_thing_do(
    thing, "/az", TINYWOT_OPERATION_TYPE_READPROPERTY, &payload
  );

  TEST_ASSERT_EQUAL(TINYWOT_ERROR_NOT_FOUND, status);

  example_thing_free(thing);
}
