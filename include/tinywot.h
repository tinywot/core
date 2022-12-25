/*
  SPDX-FileCopyrightText: 2021-2022 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \file
*/

#ifndef TINYWOT_H
#define TINYWOT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
  \defgroup tinywot_status TinyWoT Status Codes

  These macros define possible status codes used across the library, and
  returned by library functions. Thing implementations should also use them to
  indicate their status.

  Status codes are signed integer (`int`) values. Positive ones are various
  run-time exceptions. Negative ones are critical -- usually programmaic --
  errors. For example, `::TINYWOT_NULL_POINTER_ERROR` indicates that one or more
  argument that should be valid pointers turn out to be `NULL` pointers at
  run-time, which indicates a bug.

  TinyWoT functions have their possible return values documented. TinyWoT also
  expects and check all these status codes returned by the thing implementation
  functions. The exact behaviors are discussed in the documentation of every
  library function.

  @{
*/

/*!
  \brief Null pointer error.

  TinyWoT checks for NULL pointers for all public interfaces (functions). Any
  non-nullable parameter that is found to be a `NULL` in run-time will trigger
  an immediate return with this status code. Consult the documentation for
  interfaces / functions for which parameters are non-nullable.
*/
#define TINYWOT_NULL_POINTER_ERROR (-1)

/*!
  \brief General error.

  This is used to indicate an error without specifying a reason.
*/
#define TINYWOT_GENERAL_ERROR 0

/*!
  \brief Success.

  This is not an error.
*/
#define TINYWOT_SUCCESS 1

/*!
  \brief Something is missing.

  For example, if a given WoT affordance name does not exist in a thing, then
  this code will be returned.
*/
#define TINYWOT_NOT_FOUND 2

/*!
  \brief A function is not implemented.

  For example, if a given WoT affordance name is associated without a valid
  handler function (set to `NULL`), then this code will be returned.
*/
#define TINYWOT_NOT_IMPLEMENTED 3

/*!
  \brief A requested operation is not allowed.

  For example, if a given WoT affordance name exists, but cannot accept the
  given operation type, then this code will be returned.
*/
#define TINYWOT_NOT_ALLOWED 4

/*! @} */

/*!
  \brief A value specifying an unknown content type / format.

  This is used in e.g. `tinywot_scratchpad::type_hint` to indicate an
  uninitialized or erroneous type of data. TinyWoT treats this value as an
  exception (error) and will not send it out or accept it as an input. It is a
  magic value chosen to be within the "Reserved for Experimental Use" allocation
  in the [IANA CoAP Content-Formats
  registry](https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#content-formats),
  in the hope that it is not usually used by other developers.

  \note In fact, according to
  [RFC7252](https://www.rfc-editor.org/rfc/rfc7252.html), CoAP Content-Format
  "... identifiers between 65000 and 65535 inclusive are reserved for
  experiments. They are not meant for vendor-specific use of any kind and MUST
  NOT be used in operational deployments." TinyWoT at its core is not a CoAP
  producer nor consumer, so **only** this specific value is treated as an error
  by TinyWoT; others will be processed normally.
*/
#define TINYWOT_TYPE_UNKNOWN 65535u

/*!
  \brief A segment of memory with metadata.

  This data structure is used across the library to reduce the clutter of
  passing multiple if not all of them as function parameters.
*/
struct tinywot_scratchpad {
  /*!
    \brief The full size of memory pointed by `::data`.

    The unit of this field is undefined. Depending on `::type_hint` the
    application is free to choose a suitable unit, but byte is recommended.
  */
  size_t size;

  /*!
    \brief The actual meaningful size of content pointed by `::data`.

    For example, if `::data` points to a non-NUL-terminated string `"hello"`,
    then this should be 5.

    The unit of this field is undefined. Depending on `::type_hint` the
    application is free to choose a suitable unit, but byte is recommended.
  */
  size_t valid_size;

  /*!
    \brief An arbitrary unsigned integer hinting the type of data pointed by
    `data`.

    The meaning of this field is undefined; different parties producing and
    consuming this structure may have different definitions on the meaningful
    values of this field. You are recommended to use [CoAP
    Content-Formats](https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#content-formats),
    although you may use any unsigned integer here -- hence the name "hint", as
    this is only an informative field.
  */
  unsigned int type_hint;

  /*!
    \brief A pointer to a segment of memory described by the other fields in
    this structure.
    \note This pointer is nullable, provided that ::size or ::valid_size is
    `0`.
  */
  void *data;
};

void tinywot_scratchpad_forget(struct tinywot_scratchpad *self);
void tinywot_scratchpad_free(struct tinywot_scratchpad *self);

enum tinywot_operation_type {
  TINYWOT_OPERATION_TYPE_UNKNOWN = 0,
  TINYWOT_OPERATION_TYPE_PROPERTY_READ,
  TINYWOT_OPERATION_TYPE_PROPERTY_WRITE,
  TINYWOT_OPERATION_TYPE_INVOKE_ACTION
};

typedef int tinywot_handler_func(
  const struct tinywot_scratchpad *input, struct tinywot_scratchpad *output
);

struct tinywot_handler {
  const char *name;
  enum tinywot_operation_type op;
  tinywot_handler_func *func;
};

struct tinywot_thing {
  size_t handlers_size;
  struct tinywot_handler *handlers;
};

int tinywot_thing_get_handler(
  const struct tinywot_thing *self,
  const char *name,
  enum tinywot_operation_type *op,
  tinywot_handler_func **func
);

int tinywot_thing_set_handler(
  struct tinywot_thing *self,
  const char *name,
  enum tinywot_operation_type op,
  tinywot_handler_func *func
);

int tinywot_thing_read_property(
  const struct tinywot_thing *self,
  const char *name,
  struct tinywot_scratchpad *output
);

int tinywot_thing_write_property(
  const struct tinywot_thing *self,
  const char *name,
  const struct tinywot_scratchpad *input
);

int tinywot_thing_invoke_action(
  const struct tinywot_thing *self,
  const char *name,
  const struct tinywot_scratchpad *input
);

enum tinywot_response_status {
  TINYWOT_RESPONSE_STATUS_UNKNOWN = 0,
  TINYWOT_RESPONSE_STATUS_OK,
  TINYWOT_RESPONSE_STATUS_ERROR,
  TINYWOT_RESPONSE_STATUS_NOT_FOUND,
  TINYWOT_RESPONSE_STATUS_NOT_SUPPORTED,
  TINYWOT_RESPONSE_STATUS_NOT_ALLOWED
};

struct tinywot_request {
  char *name;
  enum tinywot_operation_type op;
  struct tinywot_scratchpad *content;
};

struct tinywot_response {
  enum tinywot_response_status status;
  struct tinywot_scratchpad *content;
};

int tinywot_thing_process_request(
  struct tinywot_thing *thing,
  const struct tinywot_request *request,
  struct tinywot_response *response
);

struct tinywot_simplehttp_config {
  int (*read)(struct tinywot_scratchpad *buffer);
  int (*write)(const struct tinywot_scratchpad *buffer);
  struct tinywot_scratchpad *buffer;
};

int tinywot_simplehttp_receive(
  const struct tinywot_simplehttp_config *config,
  struct tinywot_request *request
);

int tinywot_simplehttp_send(
  const struct tinywot_simplehttp_config *config,
  const struct tinywot_response *response
);

int tinywot_simplehttp_process(const struct tinywot_simplehttp_config *config);

struct tinywot_framework {
  int _unused;
};

int tinywot_framework_init(
  struct tinywot_framework *self, const unsigned char *td, size_t td_size
);

void tinywot_framework_set_reader(
  struct tinywot_framework *self,
  int (*reader)(struct tinywot_scratchpad *buffer)
);

void tinywot_framework_set_writer(
  struct tinywot_framework *self,
  int (*writer)(const struct tinywot_scratchpad *buffer)
);

int tinywot_framework_set_handler(
  struct tinywot_framework *self,
  const char *name,
  enum tinywot_operation_type op,
  tinywot_handler_func *func
);

int tinywot_framework_start(struct tinywot_framework *self);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ifndef TINYWOT_H */
