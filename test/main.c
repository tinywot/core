/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

#include "thing.h"
#include <tinywot.h>
#include <unity.h>

/* These are necessary to keep Unity happy. */

void setUp(void) {}
void tearDown(void) {}

int main(void) {
  UNITY_BEGIN();

  suite_tinywot_thing_get_form();

  return UNITY_END();
}
