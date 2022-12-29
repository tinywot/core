/*
  SPDX-FileCopyrightText: 2021-2022 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \file
  \brief TinyWoT public API definitions.
*/

#ifndef TINYWOT_H
#define TINYWOT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
  \defgroup tinywot_status TinyWoT Status Codes

  These macros define possible status codes used across the library, and
  returned by library functions. Thing implementations should also use them to
  indicate their status.

  Status codes are signed integer (`int`) values.

  TinyWoT functions have their possible return values documented. TinyWoT also
  checks status codes returned by [Behavior
  Implementation](https://www.w3.org/TR/wot-architecture11/#behavior-implementation)
  functions according to these definitions.

  \{
*/

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

/*! \} */

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

/*!
  \brief "Semantic intention" of a request according to the Thing Description.

  A full list of operation types can be found at [5.3.4.2 Form, the Web of
  Things (WoT) Thing
  Description](https://www.w3.org/TR/wot-thing-description11/#form)
  specification. So far TinyWoT only supports a subset of them.

  `::TINYWOT_OPERATION_TYPE_UNKNOWN` is a special value (`0`) indicating an
  uninitialized operation type. TinyWoT treats this value as an error.
*/
enum tinywot_operation_type {
  /*!
    \brief An unknown or uninitialized opeartion type.
  */
  TINYWOT_OPERATION_TYPE_UNKNOWN = 0,

  /*!
    \brief "Identifies the read operation on Property Affordances to retrieve
    the corresponding data."
  */
  TINYWOT_OPERATION_TYPE_READPROPERTY,

  /*!
    \brief "Identifies the write operation on Property Affordances to update the
    corresponding data."
  */
  TINYWOT_OPERATION_TYPE_WRITEPROPERTY,

  /*!
    \brief "Identifies the observe operation on Property Affordances to be
    notified with the new data when the Property is updated."
  */
  TINYWOT_OPERATION_TYPE_OBSERVEPROPERTY,

  /*!
    \brief "Identifies the unobserve operation on Property Affordances to stop
    the corresponding notifications."
  */
  TINYWOT_OPERATION_TYPE_UNOBSERVEPROPERTY,

  /*!
    \brief "Identifies the invoke operation on Action Affordances to perform the
    corresponding action."
  */
  TINYWOT_OPERATION_TYPE_INVOKEACTION,

  /*!
    \brief "Identifies the querying operation on Action Affordances to get the
    status of the corresponding action."
  */
  TINYWOT_OPERATION_TYPE_QUERYACTION,

  /*!
    \brief "Identifies the cancel operation on Action Affordances to cancel the
    ongoing corresponding action."
  */
  TINYWOT_OPERATION_TYPE_CANCELACTION,

  /*!
    \brief "Identifies the subscribe operation on Event Affordances to be
    notified by the Thing when the event occurs."
  */
  TINYWOT_OPERATION_TYPE_SUBSCRIBEEVENT,

  /*!
    \brief "Identifies the unsubscribe operation on Event Affordances to stop
    the corresponding notifications."
  */
  TINYWOT_OPERATION_TYPE_UNSUBSCRIBEEVENT,

  /*!
    \brief "Identifies the readallproperties operation on a Thing to retrieve
    the data of all Properties in a single interaction."
  */
  TINYWOT_OPERATION_TYPE_READALLPROPERTIES,

  /*!
    \brief "Identifies the writeallproperties operation on a Thing to update the
    data of all writable Properties in a single interaction."
  */
  TINYWOT_OPERATION_TYPE_WRITEALLPROPERTIES,

  /*!
    \brief "Identifies the readmultipleproperties operation on a Thing to
    retrieve the data of selected Properties in a single interaction."
  */
  TINYWOT_OPERATION_TYPE_READMULTIPLEPROPERTIES,

  /*!
    \brief "Identifies the writemultipleproperties operation on a Thing to
    update the data of selected writable Properties in a single interaction."
  */
  TINYWOT_OPERATION_TYPE_WRITEMULTIPLEPROPERTIES,

  /*!
    \brief "Identifies the observeallproperties operation on Properties to be
    notified with new data when any Property is updated."
  */
  TINYWOT_OPERATION_TYPE_OBSERVEALLPROPERTIES,

  /*!
    \brief "Identifies the unobserveallproperties operation on Properties to
    stop notifications from all Properties in a single interaction."
  */
  TINYWOT_OPERATION_TYPE_UNOBSERVEALLPROPERTIES,

  /*!
    \brief "Identifies the queryallactions operation on a Thing to get the
    status of all Actions in a single interaction."
  */
  TINYWOT_OPERATION_TYPE_QUERYALLACTIONS,

  /*!
    \brief "Identifies the subscribeallevents operation on Events to subscribe
    to notifications from all Events in a single interaction."
  */
  TINYWOT_OPERATION_TYPE_SUBSCRIBEALLEVENTS,

  /*!
    \brief "Identifies the unsubscribeallevents operation on Events to
    unsubscribe from notifications from all Events in a single interaction."
  */
  TINYWOT_OPERATION_TYPE_UNSUBSCRIBEALLEVENTS
};

/*!
  \brief Signature of a handler function implementing the behavior of an
  affordance.

  \param[in] input Data in request. The handler should not modify it.
  \param[out] output Data to send out.
  \return `TINYWOT_*` values. See \ref tinywot_status.
*/
typedef int tinywot_handler_function_t(
  struct tinywot_scratchpad const *input, struct tinywot_scratchpad *output
);

/*!
  \brief A structure containing a handler function and its metadata.
*/
struct tinywot_handler {
  /*!
    \brief The name of this handler.

    TinyWoT matches the name of incoming form with this to decide which handler
    function to invoke.
  */
  char const *name;

  /*!
    \brief The allowed operation type of this handler.
  */
  enum tinywot_operation_type op;

  /*!
    \brief A function pointer to the actual handler implementation.
  */
  tinywot_handler_function_t *func;
};

/*!
  \brief A thing.

  It contains behavior implementations, which are `tinywot_handler`s.
*/
struct tinywot_thing {
  /*!
    \brief Size of `::handlers`.

    This should be the number of `tinywot_handler`s in `::handlers`.
  */
  size_t handlers_size;

  /*!
    \brief A list of `tinywot_handler`s.
  */
  struct tinywot_handler *handlers;
};

/*!
  \brief Get the handler function by specifying a name and a operation type.

  A `tinywot_handler_function_t` needs to exist in the `tinywot_thing` with its
  `tinywot_handler::name` and `tinywot_handler::op` be exactly the same as the
  ones passed into this function.

  \param[in] self A `tinywot_thing`.
  \param[in] name A name of the handler / affordance.
  \param[in] op An opeartion type allowed by the handler.
  \param[out] func The found handler function. Note that:
    - This is nullable -- set a `NULL` here to only check if the handler exists
      or not without checking out the pointer to the handler function.
  \return `TINYWOT_*` values. See \ref tinywot_status.
*/
int tinywot_thing_get_handler_function(
  struct tinywot_thing const *self,
  char const *name,
  enum tinywot_operation_type op,
  tinywot_handler_function_t **func
);

/*!
  \brief Perform an operation on a `tinywot_thing`.

  The operation is specified by `::op`. The function looks for the corresponding
  entry in `::self`, invoke it if it exists, and then return its status code.

  \param[in] self A `tinywot_thing`.
  \param[in] name A name of the handler / affordance.
  \param[in] op An opeartion type allowed by the handler.
  \param[in] input A buffer for the invoked handler to read data in.
  \param[out] output A buffer for the invoked handler to write data out.
  \return `TINYWOT_*` values. See \ref tinywot_status.
*/
int tinywot_thing_do(
  struct tinywot_thing const *self,
  char const *name,
  enum tinywot_operation_type op,
  struct tinywot_scratchpad const *input,
  struct tinywot_scratchpad *output
);

/*!
  \brief Read a property from a `tinywot_thing`.

  This is a wrapper of `tinywot_thing_do()` -- the operation type has been
  implied by the function.

  \param[in] self A `tinywot_thing`.
  \param[in] name A name of the handler / affordance.
  \param[out] output A buffer for the invoked handler to write data out.
  \return `TINYWOT_*` values. See \ref tinywot_status.
*/
int tinywot_thing_read_property(
  struct tinywot_thing const *self,
  char const *name,
  struct tinywot_scratchpad *output
);

/*!
  \brief Write a property from a `tinywot_thing`.

  This is a wrapper of `tinywot_thing_do()` -- the operation type has been
  implied by the function.

  \param[in] self A `tinywot_thing`.
  \param[in] name A name of the handler / affordance.
  \param[in] input A buffer for the invoked handler to read data in.
  \return `TINYWOT_*` values. See \ref tinywot_status.
*/
int tinywot_thing_write_property(
  struct tinywot_thing const *self,
  char const *name,
  struct tinywot_scratchpad const *input
);

/*!
  \brief Invoke an action from a `tinywot_thing`.

  This is a wrapper of `tinywot_thing_do()` -- the operation type has been
  implied by the function.

  \param[in] self A `tinywot_thing`.
  \param[in] name A name of the handler / affordance.
  \param[in] input A buffer for the invoked handler to read data in.
  \return `TINYWOT_*` values. See \ref tinywot_status.
*/
int tinywot_thing_invoke_action(
  struct tinywot_thing const *self,
  char const *name,
  struct tinywot_scratchpad const *input
);

/*!
  \brief Status codes used in `tinywot_response`s.
*/
enum tinywot_response_status {
  /*!
    \brief An unknown or uninitialized response status.

    TinyWoT treats this value as an error.
  */
  TINYWOT_RESPONSE_STATUS_UNKNOWN = 0,

  /*! \brief Operation successful. */
  TINYWOT_RESPONSE_STATUS_OK,

  /*! \brief Operation failed. */
  TINYWOT_RESPONSE_STATUS_ERROR,

  /*! \brief Something is missing. */
  TINYWOT_RESPONSE_STATUS_NOT_FOUND,

  /*! \brief No implementation for the request. */
  TINYWOT_RESPONSE_STATUS_NOT_SUPPORTED,

  /*! \brief The request cannot be accepted. */
  TINYWOT_RESPONSE_STATUS_NOT_ALLOWED
};

/*!
  \brief A network request.
*/
struct tinywot_request {
  /*! \brief Name of the Web of Thing affordance requested. */
  char *name;

  /*! \brief The operation type requested. */
  enum tinywot_operation_type op;

  /*! \brief The payload data sent along with the request. */
  struct tinywot_scratchpad *content;
};

/*!
  \brief A network response.
*/
struct tinywot_response {
  /*! \brief A status code for the response. */
  enum tinywot_response_status status;

  /*! \brief The payload data to send along with the response. */
  struct tinywot_scratchpad *content;
};

/*!
  \brief Produce a `tinywot_response` from a `tinywot_request` with a
  `tinywot_thing`.

  \param[in] thing A thing.
  \param[in] request A network request.
  \param[out] response A network response to be sent.
  \return `TINYWOT_*` values. See \ref tinywot_status.
*/
int tinywot_thing_process_request(
  struct tinywot_thing *thing,
  struct tinywot_request const *request,
  struct tinywot_response *response
);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ifndef TINYWOT_H */
