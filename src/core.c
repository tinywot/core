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

bool tinywot_status_is_error(enum tinywot_status self) {
  return self == 0 || self > 0x7f;
}

enum tinywot_response_status tinywot_response_status_from_tinywot_status(
  enum tinywot_status const status
) {
  switch (status) {
    case TINYWOT_STATUS_ERROR_NOT_ALLOWED:
      return TINYWOT_RESPONSE_STATUS_NOT_ALLOWED;

    case TINYWOT_STATUS_ERROR_NOT_FOUND:
      return TINYWOT_RESPONSE_STATUS_NOT_FOUND;

    case TINYWOT_STATUS_ERROR_NOT_IMPLEMENTED:
      return TINYWOT_RESPONSE_STATUS_NOT_SUPPORTED;

    case TINYWOT_STATUS_SUCCESS:
      return TINYWOT_RESPONSE_STATUS_OK;

    default:
      return TINYWOT_RESPONSE_STATUS_INTERNAL_ERROR;
  }
}

enum tinywot_status tinywot_payload_append(
  struct tinywot_payload *self,
  void const *data,
  size_t data_size_byte
) {
  unsigned char *head = (unsigned char *)(self->content);
  unsigned char *tail = head + self->content_length_byte;

  if (self->content_length_byte + data_size_byte
      > self->content_buffer_size_byte) {
    return TINYWOT_STATUS_ERROR_NOT_ENOUGH_MEMORY;
  }

  memcpy(tail, data, data_size_byte);
  self->content_length_byte += data_size_byte;

  return TINYWOT_STATUS_SUCCESS;
}

enum tinywot_status tinywot_payload_append_string(
  struct tinywot_payload *self,
  char const *str
) {
  unsigned char *head = (unsigned char *)(self->content);
  unsigned char *tail = head + self->content_length_byte;

  if (self->content_length_byte > 0) {
    /* Remove any (and only) trailing NUL in self->content. */
    while (*(tail - 1) == '\0') {
      --tail;
    }
  }

  /* The old content without trailing NULs is regarded as a string
     without any NUL. The pointer difference is then the strlen() of
     the old string. */
  ptrdiff_t old_str_len = tail - head;
  size_t new_str_len = strlen(str);

  /* +1 for the trailing NUL - the result still needs to be a string! */
  size_t new_content_length_byte = old_str_len + new_str_len + 1;

  if (new_content_length_byte > self->content_buffer_size_byte) {
    return TINYWOT_STATUS_ERROR_NOT_ENOUGH_MEMORY;
  }

  /* This is essentially strcpy(tail, str). */
  memcpy(tail, str, new_str_len + 1);
  self->content_length_byte = new_content_length_byte;

  return TINYWOT_STATUS_SUCCESS;
}

void tinywot_thing_init_static(
  struct tinywot_thing *self,
  struct tinywot_form const *forms,
  size_t forms_size_byte
) {
  self->forms = (struct tinywot_form *)forms;
  self->forms_count_n = forms_size_byte / sizeof(struct tinywot_form);
  self->forms_max_n = 0;
}

void tinywot_thing_init_dynamic(
  struct tinywot_thing *self, void *memory, size_t memory_size_byte
) {
  self->forms = (struct tinywot_form *)memory;
  self->forms_count_n = 0;
  self->forms_max_n = memory_size_byte / sizeof(struct tinywot_form);
}

enum tinywot_status tinywot_thing_init_dynamic_from_static(
  struct tinywot_thing *self,
  void *memory,
  size_t memory_size_byte,
  struct tinywot_form const *forms,
  size_t forms_size_byte
) {
  if (forms_size_byte > memory_size_byte) {
    return TINYWOT_STATUS_ERROR_NOT_ENOUGH_MEMORY;
  }

  memcpy(memory, forms, forms_size_byte);

  self->forms = (struct tinywot_form *)memory;
  self->forms_count_n = forms_size_byte / sizeof(struct tinywot_form);
  self->forms_max_n = memory_size_byte / sizeof(struct tinywot_form);

  return TINYWOT_STATUS_SUCCESS;
}

enum tinywot_status tinywot_thing_find_form(
  struct tinywot_thing const *self,
  struct tinywot_form **form,
  char const *target,
  enum tinywot_operation_type op
) {
  enum tinywot_status status = TINYWOT_STATUS_ERROR_NOT_FOUND;

  /* When count is 0, there is no form added yet, so the status must be
     NOT_FOUND. */
  if (self->forms_count_n == 0) {
    return TINYWOT_STATUS_ERROR_NOT_FOUND;
  }

  /* Search from the end of self->forms. This allows dynamic override;
     forms added later would be found first, and thus would have a
     higher priority.

     When self->forms_count_n cannot be 0, we can always subtract 1 from
     it. Note that i is an unsigned value, so a condition like i >= 0 is
     always true to the compiler. To properly terminate the loop, we
     write i != 0 as the loop continuing condition, which is equivalent
     to i > 0. Inside the loop, we subtract 1 to obtain the array index.
     */
  for (size_t i = self->forms_count_n; i != 0; --i) {
    struct tinywot_form *form_i = &self->forms[i - 1];

    if (strcmp(form_i->target, target) == 0) {
      if (form_i->op == op) {
        /* Both target and op matches. */
        *form = form_i;
        status = TINYWOT_STATUS_SUCCESS;

        break;
      } else {
        /* Only target matches. However, there could be another separate
           handler for the specified target and op in the front of the
           list. We just skip this one now to continue searching, but
           set the correct status in case there is no more. */
        status = TINYWOT_STATUS_ERROR_NOT_ALLOWED;

        /* continue; */
      }
    } else {
      /* Continue searching if the target does not match. */
      /* continue; */
    }
  }

  return status;
}

enum tinywot_status tinywot_thing_add_form(
  struct tinywot_thing *self, struct tinywot_form const *form
) {
  if (self->forms_count_n + 1 > self->forms_max_n) {
    return TINYWOT_STATUS_ERROR_NOT_ENOUGH_MEMORY;
  }

  /* XXX: this also copies the padding of the supplied tinywot_form,
     which contains undefined content. */
  memcpy(&self->forms[self->forms_count_n], form, sizeof(struct tinywot_form));
  self->forms_count_n += 1;

  return TINYWOT_STATUS_SUCCESS;
}

enum tinywot_status tinywot_thing_change_form(
  struct tinywot_thing *self,
  char const *target,
  enum tinywot_operation_type op,
  struct tinywot_form const *form
) {
  enum tinywot_status status = TINYWOT_STATUS_ERROR_GENERIC;
  struct tinywot_form *form_old = NULL;

  status = tinywot_thing_find_form(self, &form_old, target, op);
  if (status != TINYWOT_STATUS_SUCCESS) {
    return status;
  }

  /* XXX: this also copies the padding of the supplied tinywot_form,
     which contains undefined content. */
  memcpy(form_old, form, sizeof(struct tinywot_form));

  return TINYWOT_STATUS_SUCCESS;
}

enum tinywot_status tinywot_thing_remove_form(
  struct tinywot_thing *self,
  char const *target,
  enum tinywot_operation_type op
) {
  /* not implemented yet */
  (void)self;
  (void)target;
  (void)op;

  return TINYWOT_STATUS_ERROR_NOT_IMPLEMENTED;
}

enum tinywot_status tinywot_thing_process_request(
  struct tinywot_thing const *self,
  struct tinywot_response *response,
  struct tinywot_request *request
) {
  enum tinywot_status status = TINYWOT_STATUS_ERROR_GENERIC;
  struct tinywot_form *form = NULL;

  status = tinywot_thing_find_form(self, &form, request->target, request->op);
  if (status != TINYWOT_STATUS_SUCCESS) {
    return status;
  }

  status = form->handler(&response->payload, &request->payload, form->context);
  response->status = tinywot_response_status_from_tinywot_status(status);

  return status;
}
