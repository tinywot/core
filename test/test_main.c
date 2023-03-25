/*
  SPDX-FileCopyrightText: 2021-2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

#include "scratchpad.h"
#include "thing.h"
#include <stdlib.h>
#include <tinywot.h>
#include <unity.h>

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

  RUN_TEST(test_tinywot_thing_initialize_with_forms_normal);
  RUN_TEST(test_tinywot_thing_initialize_with_memory_normal);

  return UNITY_END();
}
