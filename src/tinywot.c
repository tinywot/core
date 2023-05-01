/*
  SPDX-FileCopyrightText: 2021-2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \internal \file
  \brief TinyWoT public API implementations.
*/

#include <tinywot.h>

void tinywot_thing_initialize_with_forms(
  struct tinywot_thing *self,
  struct tinywot_form const *forms,
  size_t forms_size_n,
  tinywot_flag_t flags
) {
  (void)self;
  (void)forms;
  (void)forms_size_n;
  (void)flags;
}

void tinywot_thing_initialize_with_memory(
  struct tinywot_thing *self,
  void *memory,
  size_t memory_size_byte,
  tinywot_flag_t flags
) {
  (void)self;
  (void)memory;
  (void)memory_size_byte;
  (void)flags;
}

int tinywot_thing_get_form(
  struct tinywot_thing const *self,
  char const *target,
  tinywot_operation_type_t operation_types,
  struct tinywot_form const **form
) {
  (void)self;
  (void)target;
  (void)operation_types;
  (void)form;

  return TINYWOT_ERROR_NOT_IMPLEMENTED;
}

int tinywot_thing_set_form(
  struct tinywot_thing *self,
  struct tinywot_form const *form
) {
  (void)self;
  (void)form;

  return TINYWOT_ERROR_NOT_IMPLEMENTED;
}

int tinywot_thing_do(
  struct tinywot_thing const *self,
  char const *target,
  tinywot_operation_type_t operation_type,
  struct tinywot_payload *payload
) {
  (void)self;
  (void)target;
  (void)operation_type;
  (void)payload;

  return TINYWOT_ERROR_NOT_IMPLEMENTED;
}

int tinywot_thing_process_request(
  struct tinywot_thing const *self,
  struct tinywot_request *request,
  struct tinywot_response *response
) {
  (void)self;
  (void)request;
  (void)response;

  return TINYWOT_ERROR_NOT_IMPLEMENTED;
}

int tinywot_servient_process(struct tinywot_servient const *self) {
  (void)self;

  return TINYWOT_ERROR_NOT_IMPLEMENTED;
}
