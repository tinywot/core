/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

#include <unity.h>

/* These function implementations exist! They're just in separate source
   files. Also, add a declaration here when a new test is defined. */

void test_tinywot_scratchpad_initialize_normal(void);
void test_tinywot_scratchpad_split_normal(void);
void test_tinywot_scratchpad_split_left_read_only(void);
void test_tinywot_scratchpad_split_right_read_only(void);
void test_tinywot_scratchpad_split_not_enough_size(void);
void test_tinywot_scratchpad_split_not_enough_valid(void);

/* These are necessary to keep Unity happy. */

void setUp(void) { }
void tearDown(void) { }

int main(void)
{
  UNITY_BEGIN();

  RUN_TEST(test_tinywot_scratchpad_initialize_normal);
  RUN_TEST(test_tinywot_scratchpad_split_normal);
  RUN_TEST(test_tinywot_scratchpad_split_left_read_only);
  RUN_TEST(test_tinywot_scratchpad_split_right_read_only);
  RUN_TEST(test_tinywot_scratchpad_split_not_enough_size);
  RUN_TEST(test_tinywot_scratchpad_split_not_enough_valid);

  return UNITY_END();
}
