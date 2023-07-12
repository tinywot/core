/*
  SPDX-FileCopyrightText: 2021-2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \file
  \brief TinyWoT Core API implementation.
*/

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <tinywot/core.h>

tinywot_response_status_t tinywot_response_status_from_status(
  tinywot_status_t const status
) {
  switch (status) {
    case TINYWOT_STATUS_ERROR_NOT_IMPLEMENTED:
      return TINYWOT_RESPONSE_STATUS_NOT_SUPPORTED;

    case TINYWOT_STATUS_ERROR_NOT_FOUND:
      return TINYWOT_RESPONSE_STATUS_NOT_FOUND;

    case TINYWOT_STATUS_ERROR_NOT_ALLOWED:
      return TINYWOT_RESPONSE_STATUS_NOT_ALLOWED;

    case TINYWOT_STATUS_SUCCESS:
      return TINYWOT_RESPONSE_STATUS_OK;

    case TINYWOT_STATUS_ERROR_NOT_ENOUGH_MEMORY: /* fall through */
    default:
      return TINYWOT_RESPONSE_STATUS_INTERNAL_ERROR;
  }
}

/*!
  \brief Find a [Form] from a [Base Thing].

  A [Form] is found when all the following apply:

  - The supplied `target` equals to `tinywot_form::target`
  - The supplied `operation_types` _matches_
    `tinywot_form::allowed_operation_types` (that is, all bits are set
    within the mask).

  \param[in]  self A [Base Thing].
  \param[out] form A pointer to the [Form] that matches the supplied
  conditions.
  \param[in]  target The submission target.
  \param[in]  operation_types The target operation type(s).
  \return
    - `::TINYWOT_STATUS_SUCCESS` if a [Form] is found. `form` will then
      be set to point to that [Form].
    - `::TINYWOT_STATUS_ERROR_NOT_FOUND` if a [Form] is not found. The
      value of `form` is undefined in this case.

  [Base Thing]: \ref tinywot_thing_base
  [Form]: \ref tinywot_form
*/
static tinywot_status_t tinywot_thing_base_find_form(
  struct tinywot_thing_base const *self,
  struct tinywot_form const **form,
  char const *target,
  tinywot_operation_type_t operation_types
) {
  size_t form_i = 0;
  struct tinywot_form const *form_i_p = self->forms;

  for (; form_i < self->forms_count_n; form_i += 1) {
    if (strcmp(form_i_p->target, target) == 0) {
      /*
        Determine whether the incoming operation type(s) are all
        included by the allowed operation types set in form; that is,
        all bits set in `operation_types` must be also set in
        `form_i_p->allowed_operation_types`. The expression applies the
        allowed operation types as a bit mask to the incoming operation
        type(s), then see if the incoming opeartion type(s) changes. If
        it does, then there exists at least one bit that is not also set
        in the allowed operation types.
      */
      tinywot_operation_type_t allowed =
        (form_i_p->allowed_operation_types & operation_types)
        == operation_types;

      if (allowed) {
        /*
          Allow opting-out form pointer receipt to only perform an
          existence check.
        */
        if (form) {
          *form = form_i_p;
        }

        return TINYWOT_STATUS_SUCCESS;
      } else {
        return TINYWOT_STATUS_ERROR_NOT_ALLOWED;
      }
    }
  }

  return TINYWOT_STATUS_ERROR_NOT_FOUND;
}

static tinywot_status_t tinywot_thing_base_process_request(
  struct tinywot_thing_base const *self,
  struct tinywot_response *response,
  struct tinywot_request *request
) {
  struct tinywot_form const *form = NULL;
  tinywot_status_t status = TINYWOT_STATUS_SUCCESS;

  status = tinywot_thing_base_find_form(
    self, &form, request->target, request->operation_type
  );

  if (tinywot_status_is_error(status)) {
    return status;
  }

  status = form->handler(response, request, form->context);
  response->status = tinywot_response_status_from_status(status);

  return status;
}

tinywot_status_t tinywot_payload_append(
  struct tinywot_payload *self,
  void *data,
  size_t data_size_byte
) {
  unsigned char *head = (unsigned char *)(self->content);
  unsigned char *tail = head + self->content_length_byte;

  if (self->content_length_byte + data_size_byte >
      self->content_buffer_size_byte) {
    return TINYWOT_STATUS_ERROR_NOT_ENOUGH_MEMORY;
  }

  memcpy(tail, data, data_size_byte);
  self->content_length_byte += data_size_byte;

  return TINYWOT_STATUS_SUCCESS;
}

struct tinywot_form const *tinywot_thing_static_get_forms(
  struct tinywot_thing_static const *self
) {
  return self->base.forms;
}

size_t tinywot_thing_static_get_forms_count(
  struct tinywot_thing_static const *self
) {
  return self->base.forms_count_n;
}

tinywot_status_t tinywot_thing_static_find_form(
  struct tinywot_thing_static const *self,
  struct tinywot_form const **form,
  char const *target,
  tinywot_operation_type_t operation_types
) {
  return tinywot_thing_base_find_form(
    (struct tinywot_thing_base const *)self, form, target, operation_types
  );
}

tinywot_status_t tinywot_thing_static_process_request(
  struct tinywot_thing_static const *self,
  struct tinywot_response *response,
  struct tinywot_request *request
) {
  return tinywot_thing_base_process_request(
    (struct tinywot_thing_base const *)self, response, request
  );
}

struct tinywot_form const *tinywot_thing_dynamic_get_forms(
  struct tinywot_thing_dynamic const *self
) {
  return self->base.forms;
}

size_t tinywot_thing_dynamic_get_forms_count(
  struct tinywot_thing_dynamic const *self
) {
  return self->base.forms_count_n;
}

size_t tinywot_thing_dynamic_get_forms_max(
  struct tinywot_thing_dynamic const *self
) {
  return self->forms_max_n;
}

void tinywot_thing_dynamic_set_forms_buffer(
  struct tinywot_thing_dynamic *self,
  void *buffer,
  size_t buffer_size_byte
) {
  self->base.forms = buffer;
  self->base.forms_count_n = 0;
  self->forms_max_n = buffer_size_byte / sizeof(struct tinywot_thing_dynamic);
}

tinywot_status_t tinywot_thing_dynamic_find_form(
  struct tinywot_thing_dynamic const *self,
  struct tinywot_form const **form,
  char const *target,
  tinywot_operation_type_t operation_types
) {
  return tinywot_thing_base_find_form(
    (struct tinywot_thing_base const *)self, form, target, operation_types
  );
}

tinywot_status_t tinywot_thing_dynamic_add_form(
  struct tinywot_thing_dynamic *self, struct tinywot_form const *form
) {
  /* not implemented yet */
  return TINYWOT_STATUS_ERROR_NOT_IMPLEMENTED;
}

tinywot_status_t tinywot_thing_dynamic_change_form(
  struct tinywot_thing_dynamic *self,
  char const *target,
  tinywot_operation_type_t operation_types,
  struct tinywot_form const *form
) {
  /* not implemented yet */
  return TINYWOT_STATUS_ERROR_NOT_IMPLEMENTED;
}

tinywot_status_t tinywot_thing_dynamic_set_form(
  struct tinywot_thing_dynamic *self,
  struct tinywot_form const *form
) {
  size_t form_i = 0;
  struct tinywot_form *form_i_p = self->base.forms;

  /*
    Iterate over the existing list first to see if any form can be
    replaced with the incoming one.
  */
  for (; form_i < self->base.forms_count_n; form_i += 1, form_i_p += 1) {
    if (strcmp(form_i_p->target, form->target) == 0) {
      /*
        Determine whether the incoming `allowed_operation_types` has at
        least one bit that is also set in the `allowed_operation_types`
        in the pointed form in the Thing. The expression masks the two
        to see if the result is still true (not zero). If so, then the
        two forms have at least one overlapping operation type, and the
        corresponding form may be replaced by the incoming one
        (depending one 2).
      */
      tinywot_operation_type_t overlap =
        form_i_p->allowed_operation_types & form->allowed_operation_types;

      if (overlap) {
        /* XXX: copying padding could be an insecure operation. */
        memcpy(form_i_p, form, sizeof(struct tinywot_form));

        return TINYWOT_STATUS_SUCCESS;
      } else {
        /* continue; */
      }
    }
  }

  /*
    At this point, `form_i == self->forms_count_n`. If it is also the
    maximum number of form, then there is no more slot for us to append
    the incoming one.
  */
  if (form_i >= self->forms_max_n) {
    return TINYWOT_STATUS_ERROR_NOT_ENOUGH_MEMORY;
  }

  self->base.forms_count_n += 1;
  /* XXX: copying padding could be an insecure operation. */
  memcpy(form_i_p, form, sizeof(struct tinywot_form));

  return TINYWOT_STATUS_SUCCESS;
}

tinywot_status_t tinywot_thing_dynamic_remove_form(
  struct tinywot_thing_dynamic *self,
  char const *target,
  tinywot_operation_type_t allowed_operation_types
) {
  /* not implemented yet */
  return TINYWOT_STATUS_ERROR_NOT_IMPLEMENTED;
}

tinywot_status_t tinywot_thing_dynamic_process_request(
  struct tinywot_thing_dynamic const *self,
  struct tinywot_response *response,
  struct tinywot_request *request
) {
  return tinywot_thing_base_process_request(
    (struct tinywot_thing_base const *)self, response, request
  );
}
