/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

#ifndef TINYWOT_TEST_SCRATCHPAD_H
#define TINYWOT_TEST_SCRATCHPAD_H

void test_tinywot_scratchpad_initialize_normal(void);

void test_tinywot_scratchpad_split_normal(void);
void test_tinywot_scratchpad_split_left_read_only(void);
void test_tinywot_scratchpad_split_right_read_only(void);
void test_tinywot_scratchpad_split_not_enough_size(void);
void test_tinywot_scratchpad_split_not_enough_valid(void);

#endif /* TINYWOT_TEST_SCRATCHPAD_H */
