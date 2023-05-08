/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tinywot.h>
#include <unity.h>

/******************** Data ********************/

static char const a0_content[] = "Hello, world!";
static char a1_content[32] = {0};

/******************** Handlers ********************/

static tinywot_status_t a0_handler(
  char const *name,
  char const *target,
  tinywot_operation_type_t operation_type,
  struct tinywot_payload *payload,
  void *user_data
) {
  (void)user_data;

  /* Payload consumption. */

  TEST_ASSERT_EQUAL_STRING("affordance0", name);
  TEST_ASSERT_EQUAL_STRING("/a0", target);
  TEST_ASSERT_EQUAL_UINT(TINYWOT_OPERATION_TYPE_READPROPERTY, operation_type);
  TEST_ASSERT_NOT_NULL(payload);

  printf(
    "%s (name: %s, target: %s, opty: %#lx)\n",
    __func__,
    name,
    target,
    operation_type
  );

  /* Payload re-use for responding. */

  if (tinywot_flag_is_dynamic(payload->flags)) {
    free(payload->content);
  }

  memset(payload, 0, sizeof(struct tinywot_payload));

  payload->flags = TINYWOT_FLAG_READ_ONLY;
  payload->content_type = 0; /* text/plain; charset=utf-8 */
  payload->content_length_byte = strlen(a0_content);
  payload->content = (unsigned char *)a0_content;

  return TINYWOT_SUCCESS;
}

static tinywot_status_t a1_handler(
  char const *name,
  char const *target,
  tinywot_operation_type_t operation_type,
  struct tinywot_payload *payload,
  void *user_data
) {
  (void)user_data;

  /* Payload consumption. */

  TEST_ASSERT_EQUAL_STRING("affordance1", name);
  TEST_ASSERT_EQUAL_STRING("/a1", target);
  TEST_ASSERT(((TINYWOT_OPERATION_TYPE_READPROPERTY |
                TINYWOT_OPERATION_TYPE_WRITEPROPERTY) & operation_type)
                  == operation_type);
  TEST_ASSERT_NOT_NULL(payload);

  printf(
    "%s (name: %s, target: %s, opty: %#lx)\n",
    __func__,
    name,
    target,
    operation_type
  );

  /* Payload re-use for responding. */

  if (operation_type == TINYWOT_OPERATION_TYPE_WRITEPROPERTY) {
    if (payload->content_length_byte > 32) {
      return TINYWOT_ERROR_NOT_ENOUGH_MEMORY;
    }

    memcpy(a1_content, payload->content, payload->content_length_byte);
  }

  if (tinywot_flag_is_dynamic(payload->flags)) {
    free(payload->content);
  }

  memset(payload, 0, sizeof(struct tinywot_payload));

  payload->flags = TINYWOT_FLAG_READ_ONLY;
  payload->content_type = 0; /* text/plain; charset=utf-8 */
  payload->content_length_byte = strlen(a1_content);
  payload->content = (unsigned char *)a1_content;

  return TINYWOT_SUCCESS;
}

/******************** Forms ********************/

static struct tinywot_form const example_form_0_ = {
  .name = "affordance0",
  .target = "/a0",
  .allowed_operation_types = TINYWOT_OPERATION_TYPE_READPROPERTY,
  .handler = a0_handler,
  .user_data = NULL,
};

static struct tinywot_form const example_form_1_ = {
  .name = "affordance1",
  .target = "/a1",
  .allowed_operation_types =
    TINYWOT_OPERATION_TYPE_READPROPERTY | TINYWOT_OPERATION_TYPE_WRITEPROPERTY,
  .handler = a1_handler,
  .user_data = NULL,
};

/******************** Public Interfaces ********************/

void example_thing_free(struct tinywot_thing *thing) {
  free((void *)thing->forms);
  free(thing);
}

__attribute__((__malloc__))
struct tinywot_thing *example_thing_new(void) {
  struct tinywot_thing *new_thing = malloc(sizeof(struct tinywot_thing));
  TEST_ASSERT_NOT_NULL(new_thing);

  struct tinywot_form *new_thing_forms = malloc(
    3 * sizeof(struct tinywot_thing)
  );
  TEST_ASSERT_NOT_NULL(new_thing_forms);

  memcpy(&new_thing_forms[0], &example_form_0_, sizeof(struct tinywot_form));

  new_thing->flags = TINYWOT_FLAG_DYNAMIC;
  new_thing->forms_count_n = 1;
  new_thing->forms_max_n = 3;
  new_thing->forms = new_thing_forms;

  return new_thing;
}

struct tinywot_form const *example_form_0(void) {
  return &example_form_0_;
}

struct tinywot_form const *example_form_1(void) {
  return &example_form_1_;
}
