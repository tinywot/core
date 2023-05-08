/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

#include "tests.h"
#include <tinywot.h>
#include <unity.h>

/* These are necessary to keep Unity happy. */

void setUp(void) {}
void tearDown(void) {}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_tinywot_thing_do_normal);
  RUN_TEST(test_tinywot_thing_do_mismatch_opty);
  RUN_TEST(test_tinywot_thing_do_mismatch_target);

  RUN_TEST(test_tinywot_thing_get_form_normal);
  RUN_TEST(test_tinywot_thing_get_form_mismatch_opty);
  RUN_TEST(test_tinywot_thing_get_form_mismatch_target);

  RUN_TEST(test_tinywot_thing_set_form_overlap_opty);
  RUN_TEST(test_tinywot_thing_set_form_read_only);

  return UNITY_END();
}
