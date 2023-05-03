/*
  SPDX-FileCopyrightText: 2021-2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \internal \file
  \brief TinyWoT public API implementations.
*/

#include <stddef.h>
#include <string.h>

#include <tinywot.h>

void tinywot_thing_initialize_with_forms(
  struct tinywot_thing *self,
  struct tinywot_form const *forms,
  size_t forms_size_n,
  tinywot_flag_t flags
) {
  self->flags = flags;
  self->forms_count_n = forms_size_n;
  self->forms_max_n = forms_size_n;
  self->forms = forms;
}

void tinywot_thing_initialize_with_memory(
  struct tinywot_thing *self,
  void *memory,
  size_t memory_size_byte,
  tinywot_flag_t flags
) {
  self->flags = flags;
  self->forms_count_n = 0;
  self->forms_max_n = memory_size_byte / sizeof(struct tinywot_form);
  self->forms = memory;
}

tinywot_status_t tinywot_thing_get_form(
  struct tinywot_thing const *self,
  char const *target,
  tinywot_operation_type_t operation_types,
  struct tinywot_form const **form
) {
  tinywot_status_t status = TINYWOT_ERROR_NOT_FOUND;

  for (size_t i = 0; i < self->forms_count_n; i += 1) {
    struct tinywot_form const *form_i_p = &self->forms[i];

    if (((form_i_p->allowed_operation_types & operation_types) == operation_types) &&
        (strcmp(form_i_p->target, target) == 0)) {
      *form = form_i_p;
      status = TINYWOT_SUCCESS;
      break;
    }
  }

  return status;
}

tinywot_status_t tinywot_thing_set_form(
  struct tinywot_thing *self,
  struct tinywot_form const *form
) {
  if (tinywot_flag_is_read_only(self->flags)) {
    return TINYWOT_ERROR_READ_ONLY;
  }

  size_t form_i = 0;
  struct tinywot_form *form_i_p = NULL;

  for (; form_i < self->forms_count_n; form_i += 1) {
    form_i_p = (struct tinywot_form *)&self->forms[form_i];

    if ((form_i_p->allowed_operation_types & form->allowed_operation_types) &&
        (strcmp(form_i_p->target, form->target) == 0)) {
      memcpy(form_i_p, form, sizeof(struct tinywot_form));
      return TINYWOT_SUCCESS;
    }
  }

  if (form_i >= self->forms_max_n) {
    return TINYWOT_ERROR_NOT_ENOUGH_MEMORY;
  }

  self->forms_count_n += 1;
  memcpy(form_i_p, form, sizeof(struct tinywot_form));

  return TINYWOT_SUCCESS;
}

tinywot_status_t tinywot_thing_do(
  struct tinywot_thing const *self,
  char const *target,
  tinywot_operation_type_t operation_type,
  struct tinywot_payload *payload
) {
  struct tinywot_form const *form = NULL;
  tinywot_status_t status = TINYWOT_ERROR_GENERAL;

  status = tinywot_thing_get_form(
    self, target, operation_type, &form
  );
  if (tinywot_is_error(status)) {
    return status;
  }

  return form->handler(
    form->name, form->target, operation_type, payload, form->user_data
  );
}

tinywot_status_t tinywot_thing_process_request(
  struct tinywot_thing const *self,
  struct tinywot_request *request,
  struct tinywot_response *response
) {
  tinywot_status_t status = TINYWOT_ERROR_GENERAL;

  status = tinywot_thing_do(
    self, request->target, request->operation_type, request->payload
  );

  switch (status) {
    case TINYWOT_SUCCESS:
      response->status = TINYWOT_RESPONSE_STATUS_OK;
      break;

    case TINYWOT_ERROR_NOT_ALLOWED:
      response->status = TINYWOT_RESPONSE_STATUS_NOT_ALLOWED;
      break;

    case TINYWOT_ERROR_NOT_IMPLEMENTED:
      response->status = TINYWOT_RESPONSE_STATUS_NOT_SUPPORTED;
      break;

    case TINYWOT_ERROR_NOT_FOUND:
      response->status = TINYWOT_RESPONSE_STATUS_NOT_FOUND;
      break;

    case TINYWOT_ERROR_NOT_ENOUGH_MEMORY: /* fall through */
    case TINYWOT_ERROR_GENERAL: /* fall through */
    default:
      response->status = TINYWOT_RESPONSE_STATUS_ERROR;
      break;
  }

  response->payload = request->payload;

  return status;
}

tinywot_status_t tinywot_servient_process(struct tinywot_servient const *self) {
  struct tinywot_request request = {0};
  struct tinywot_response response = {0};
  struct tinywot_payload payload = {0};
  tinywot_status_t status = TINYWOT_ERROR_GENERAL;

  payload.content = self->memory;
  payload.size_byte = self->memory_size_byte;

  response.payload = &payload;

  status = self->protocol.receive(&request, &self->io);
  if (tinywot_is_error(status)) {
    return status;
  }

  status = tinywot_thing_process_request(
    &self->thing, &request, &response
  );
  if (tinywot_is_error(status)) {
    return status;
  }

  status = self->protocol.send(&response, &self->io);
  if (tinywot_is_error(status)) {
    return status;
  }

  return TINYWOT_SUCCESS;
}
