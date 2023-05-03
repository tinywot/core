/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

#include <stddef.h>
#include <stdio.h>
#include <tinywot.h>

/******************** Handlers ********************/

static tinywot_status_t a0_handler(
  char const *name,
  char const *target,
  tinywot_operation_type_t operation_type,
  struct tinywot_payload *payload,
  void *user_data
) {
  (void)payload;
  (void)user_data;

  printf("> %s (target %s, op %#lx)\n", name, target, operation_type);

  return TINYWOT_SUCCESS;
}

/******************** Forms ********************/

static struct tinywot_form const example_forms[] = {
  {
    .name = "affordance0",
    .target = "/a0",
    .allowed_operation_types = TINYWOT_OPERATION_TYPE_READPROPERTY,
    .handler = a0_handler,
    .user_data = NULL,
  },
};

size_t const example_forms_size =
  sizeof(example_forms) / sizeof(struct tinywot_form);

/******************** Thing ********************/

static struct tinywot_thing const example_thing = {
  .flags = TINYWOT_FLAG_READ_ONLY,
  .forms_count_n = example_forms_size,
  .forms_max_n = example_forms_size,
  .forms = example_forms,
};

/******************** Public Interfaces ********************/

void init_example_forms(struct tinywot_form const **forms, size_t *forms_size) {
  *forms = example_forms;
  *forms_size = example_forms_size;
}

void init_example_thing(struct tinywot_thing const **thing) {
  *thing = &example_thing;
}
