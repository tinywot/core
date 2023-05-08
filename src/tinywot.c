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

tinywot_status_t tinywot_thing_get_form(
  struct tinywot_thing const *self,
  char const *target,
  tinywot_operation_type_t operation_types,
  struct tinywot_form const **form
) {
  size_t form_i = 0;
  struct tinywot_form *form_i_p = (struct tinywot_form *)self->forms;

  for (; form_i < self->forms_count_n; form_i += 1) {
    if (strcmp(form_i_p->target, target) == 0) {
      /*
        Determine whether the incoming operation type(s) are all included by
        the allowed operation types set in form; that is, all bits set in
        `operation_types` must be also set in
        `form_i_p->allowed_operation_types`. The expression applies the allowed
        operation types as a bit mask to the incoming operation type(s), then
        see if the incoming opeartion type(s) changes. If it does, then there
        exists at least one bit that is not also set in the allowed operation
        types.
      */
      tinywot_operation_type_t op_ty_all_allowed =
        (form_i_p->allowed_operation_types & operation_types)
          == operation_types;

      if (op_ty_all_allowed) {
        /*
          Allow opting-out form pointer receipt to only perform an existence
          check.
        */
        if (form) {
          *form = form_i_p;
        }

        return TINYWOT_SUCCESS;
      } else {
        return TINYWOT_ERROR_NOT_ALLOWED;
      }
    }
  }

  return TINYWOT_ERROR_NOT_FOUND;
}

tinywot_status_t tinywot_thing_set_form(
  struct tinywot_thing *self,
  struct tinywot_form const *form
) {
  if (tinywot_flag_is_read_only(self->flags)) {
    return TINYWOT_ERROR_READ_ONLY;
  }

  size_t form_i = 0;
  struct tinywot_form *form_i_p = (struct tinywot_form *)self->forms;

  /*
    Iterate over the existing list first to see if any form can be replaced
    with the incoming one.
  */
  for (; form_i < self->forms_count_n; form_i += 1, form_i_p += 1) {
    if (strcmp(form_i_p->target, form->target) == 0) {
      /*
        Determine whether the incoming `allowed_operation_types` has at least
        one bit that is also set in the `allowed_operation_types` in the
        pointed form in the Thing. The expression masks the two to see if the
        result is still true (not zero). If so, then the two forms have at
        least one overlapping operation type, and the corresponding form may be
        replaced by the incoming one (depending one 2).
      */
      tinywot_operation_type_t op_ty_any_overlap =
        form_i_p->allowed_operation_types & form->allowed_operation_types;

      if (op_ty_any_overlap) {
        /* XXX: copying padding could be an insecure operation. */
        memcpy(form_i_p, form, sizeof(struct tinywot_form));

        return TINYWOT_SUCCESS;
      } else {
        /* continue; */
      }
    }
  }

  /*
    At this point, `form_i == self->forms_count_n`. If it is also the maximum
    number of form, then there is no more slot for us to append the incoming
    one.
  */
  if (form_i >= self->forms_max_n) {
    return TINYWOT_ERROR_NOT_ENOUGH_MEMORY;
  }

  self->forms_count_n += 1;
  /* XXX: copying padding could be an insecure operation. */
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

  status = tinywot_thing_get_form(self, target, operation_type, &form);
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

  /* Payload is reused by the handler. */
  response->payload = request->payload;

  return status;
}

tinywot_status_t tinywot_servient_process(struct tinywot_servient const *self) {
  struct tinywot_request request = {0};
  struct tinywot_response response = {0};
  struct tinywot_payload payload = {0};
  tinywot_status_t status = TINYWOT_ERROR_GENERAL;

  /* Prepare memory for payload, and fit it into `request`. */
  payload.content = self->memory;
  payload.size_byte = self->memory_size_byte;
  response.payload = &payload;

  status = self->protocol.receive(&request, &self->io);
  if (tinywot_is_error(status)) {
    return status;
  }

  status = tinywot_thing_process_request(&self->thing, &request, &response);
  if (tinywot_is_error(status)) {
    return status;
  }

  status = self->protocol.send(&response, &self->io);
  if (tinywot_is_error(status)) {
    return status;
  }

  return TINYWOT_SUCCESS;
}
