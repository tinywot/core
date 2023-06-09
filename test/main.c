/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

#include "tests.h"
#include <tinywot/core.h>
#include <unity.h>

/* These are necessary to keep Unity happy. */

void setUp(void) {}
void tearDown(void) {}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_tinywot_thing_dynamic_get_form_normal);
  RUN_TEST(test_tinywot_thing_dynamic_get_form_mismatch_opty);
  RUN_TEST(test_tinywot_thing_dynamic_get_form_mismatch_target);

  RUN_TEST(test_tinywot_thing_dynamic_set_form_normal);

  return UNITY_END();
}
