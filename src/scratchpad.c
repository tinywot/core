/*
  SPDX-FileCopyrightText: 2022 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \internal \file
*/

#include "common.h"
#include <string.h>

#include <tinywot.h>

void tinywot_scratchpad_forget(struct tinywot_scratchpad *self) {
  if (!self) {
    return;
  }

  memset(self, 0, sizeof(struct tinywot_scratchpad));
}

void tinywot_scratchpad_free(struct tinywot_scratchpad *self) {
  if (!self) {
    return;
  }

  tinywot_free(self->data);
  memset(self, 0, sizeof(struct tinywot_scratchpad));
}
