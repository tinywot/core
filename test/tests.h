/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

#ifndef TINYWOT_TEST_TESTS_H
#define TINYWOT_TEST_TESTS_H

void test_tinywot_thing_do_normal(void);
void test_tinywot_thing_do_mismatch_opty(void);
void test_tinywot_thing_do_mismatch_target(void);

void test_tinywot_thing_get_form_normal(void);
void test_tinywot_thing_get_form_mismatch_opty(void);
void test_tinywot_thing_get_form_mismatch_target(void);

void test_tinywot_thing_set_form_overlap_opty(void);
void test_tinywot_thing_set_form_read_only(void);

#endif /* TINYWOT_TEST_TESTS_H */
