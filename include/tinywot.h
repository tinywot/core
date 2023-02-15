/*
  SPDX-FileCopyrightText: 2021-2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \file
  \brief TinyWoT public API definitions.
*/

#ifndef TINYWOT_H
#define TINYWOT_H

#include <stdbool.h>
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

  Status codes are signed integer (`int`) values. Errors are negative.

  \{
*/

/*!
  \brief A requested operation is not allowed.

  For example, if a given WoT affordance name exists, but cannot accept the
  given operation type, then this code will be returned.
*/
#define TINYWOT_ERROR_NOT_ALLOWED (-4)

/*!
  \brief A function is not implemented.

  For example, if a given WoT affordance name is associated without a valid
  handler function (set to `NULL`), then this code will be returned.
*/
#define TINYWOT_ERROR_NOT_IMPLEMENTED (-3)

/*!
  \brief Something is missing.

  For example, if a given WoT affordance name does not exist in a thing, then
  this code will be returned.
*/
#define TINYWOT_ERROR_NOT_FOUND (-2)

/*!
  \brief General error.

  This is used to indicate an error without specifying a reason.
*/
#define TINYWOT_ERROR_GENERAL_ERROR (0)

/*!
  \brief Success.

  This is not an error.
*/
#define TINYWOT_SUCCESS (1)

/*!
  \brief Check if a status code indicates an error.

  \param[in] r An integer.
  \return `true` or `false`.
*/
static inline bool tinywot_is_error(int r) { return r <= 0; }

/*!
  \brief Check if a status code indicates a success.

  \param[in] r An integer.
  \return `true` or `false`.
*/
static inline bool tinywot_is_success(int r) { return r > 0; }

/*! \} */ /* defgroup tinywot_status */

/*!
  \defgroup tinywot_scratchpad Scratchpad

  A segment of memory with metadata.

  \{
*/

/*!
  \brief A value specifying an unknown content type / format.

  This is used in e.g. `tinywot_scratchpad::type_hint` to indicate an
  uninitialized or erroneous type of data. TinyWoT treats this value as an
  exception (error) and will not send it out or accept it as an input. It is a
  magic value chosen to be within the "Reserved for Experimental Use"
  allocation in the [IANA CoAP Content-Formats registry][cf-reg], in the hope
  that it is not usually used by other developers. It is not `0` because it has
  been allocated to `text/plain; charset=utf-8` in the registry.

  \note In fact, according to [RFC7252], CoAP Content-Format "... identifiers
  between 65000 and 65535 inclusive are reserved for experiments. They are not
  meant for vendor-specific use of any kind and MUST NOT be used in operational
  deployments." TinyWoT at its core is not a CoAP producer nor consumer, so
  **only** this specific value is treated as an error by TinyWoT; others will
  be processed normally.

  [cf-reg]:
  https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#content-formats
  [RFC7252]: https://www.rfc-editor.org/rfc/rfc7252.html
*/
#define TINYWOT_TYPE_UNKNOWN (65535u)

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
    values of this field. You are recommended to use [CoAP Content-Formats],
    although you may use any unsigned integer here -- hence the name "hint",
    as this is only an informative field.

    [CoAP Content-Formats]:
    https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#content-formats
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

/*! \} */ /* defgroup tinywot_scratchpad */

/*!
  \defgroup tinywot_req_and_res Requests and Responses

  \{
*/

/*!
  \brief The maximum number of character allowed in a `tinywot_request::name`.
*/
#ifndef TINYWOT_REQUEST_PATH_MAX_LENGTH
#define TINYWOT_REQUEST_PATH_MAX_LENGTH (32u)
#endif

/*!
  \brief "Semantic intention" of a request according to the Thing
  Description.

  A full list of operation types can be found at [&sect; 5.3.4.2 Form, the
  Web of Things (WoT) Thing Description][td-5.3.4.2]. So far, TinyWoT only
  supports a subset of them.

  `::TINYWOT_OPERATION_TYPE_UNKNOWN` is a special value (`0`) indicating an
  uninitialized operation type. TinyWoT treats this value as an error.

  [td-5.3.4.2]:
  https://www.w3.org/TR/2020/REC-wot-thing-description-20200409/#form
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
    \brief "Identifies the invoke operation on Action Affordances to perform
    the corresponding action."
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
  \brief Status codes used in a `tinywot_response`.
*/
enum tinywot_response_status {
  /*! \brief An unknown or uninitialized response status. */
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
  /*! \brief The path name of the target IRI in the request. */
  char path[TINYWOT_REQUEST_PATH_MAX_LENGTH];

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

/*! \} */ /* defgroup tinywot_req_and_res */

/*!
  \defgroup tinywot_thing Behavior Implementation

  \{
*/

/*!
  \brief Signature of a handler function implementing the behavior of an
  affordance.

  \param[in] input Data in request. The handler should not modify it.
  \param[out] output Data to send out.
  \return \ref tinywot_status
*/
typedef int tinywot_handler_function_t(
  struct tinywot_scratchpad const *input,
  struct tinywot_scratchpad *output,
  void *user_data
);

/*!
  \brief A structure containing a handler function and its metadata.
*/
struct tinywot_handler {
  /*!
    \brief The allowed operation type of this handler.
  */
  enum tinywot_operation_type op;

  /*!
    \brief The path name of the IRI in `href` in the form.
  */
  char const *path;

  /*!
    \brief A function pointer to the actual handler implementation.
  */
  tinywot_handler_function_t *func;

  /*!
    \brief Arbitrary data to pass when `::func` is called.
  */
  void *user_data;
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
  \brief Get the handler function (closure) by specifying a path name and an
  operation type.

  \param[in] self A `tinywot_thing`.
  \param[in] op An opeartion type allowed by the handler.
  \param[in] path The path name to use to search for a handler.
  \param[out] func The found handler function. This is nullable -- set a
  `NULL` here to only check if the handler exists or not without checking out
  the pointer to the handler function. \param[out] user_data The associated
  user data for `func`. This is nullable
  -- set a `NULL` here to not retrieve the pointer to `user_data`.
  \return \ref tinywot_status
*/
int tinywot_thing_get_handler_function(
  struct tinywot_thing const *self,
  char const *path,
  enum tinywot_operation_type op,
  tinywot_handler_function_t **func,
  void **user_data
);

/*!
  \brief Perform an operation on a `tinywot_thing`.

  \param[in] self A `tinywot_thing`.
  \param[in] op An opeartion type allowed by the handler.
  \param[in] path The path name to use to search for a handler.
  \param[in] input A buffer for the invoked handler to read data in.
  \param[out] output A buffer for the invoked handler to write data out.
  \return \ref tinywot_status
*/
int tinywot_thing_do(
  struct tinywot_thing const *self,
  char const *path,
  enum tinywot_operation_type op,
  struct tinywot_scratchpad const *input,
  struct tinywot_scratchpad *output
);

/*!
  \brief Read a property from a `tinywot_thing`.

  This is a wrapper of `tinywot_thing_do` -- the operation type has been
  implied by the function.

  \param[in] self A `tinywot_thing`.
  \param[in] path The path name to use to search for a handler.
  \param[out] output A buffer for the invoked handler to write data out.
  \return \ref tinywot_status
*/
int tinywot_thing_read_property(
  struct tinywot_thing const *self,
  char const *path,
  struct tinywot_scratchpad *output
);

/*!
  \brief Write a property from a `tinywot_thing`.

  This is a wrapper of `tinywot_thing_do` -- the operation type has been
  implied by the function.

  \param[in] self A `tinywot_thing`.
  \param[in] path The path name to use to search for a handler.
  \param[in] input A buffer for the invoked handler to read data in.
  \return \ref tinywot_status
*/
int tinywot_thing_write_property(
  struct tinywot_thing const *self,
  char const *path,
  struct tinywot_scratchpad const *input
);

/*!
  \brief Invoke an action from a `tinywot_thing`.

  This is a wrapper of `tinywot_thing_do` -- the operation type has been
  implied by the function.

  \param[in] self A `tinywot_thing`.
  \param[in] path The path name to use to search for a handler.
  \param[in] input A buffer for the invoked handler to read data in.
  \return \ref tinywot_status
*/
int tinywot_thing_invoke_action(
  struct tinywot_thing const *self,
  char const *path,
  struct tinywot_scratchpad const *input
);

/*!
  \brief Transform a `tinywot_request` into a `tinywot_response` using a
  `tinywot_thing`.

  This function finds a handler that supports the `request` in the supplied
  `thing`. If none is found, `TINYWOT_ERROR_NOT_FOUND` is returned.

  \param[in] self A `tinywot_thing`.
  \param[in] request A network request.
  \param[out] response A network response to be sent.
  \return \ref tinywot_status
*/
int tinywot_thing_process_request(
  struct tinywot_thing const *self,
  struct tinywot_request const *request,
  struct tinywot_response *response
);

/*! \} */ /* defgroup tinywot_thing */

/*!
  \defgroup tinywot_io Platform-specific Input / Output Interfaces

  The two interfaces (`read` and `write`) are essential for TinyWoT to work
  on a higher level of abstraction. They should be simple functions that
  fills in a given buffer and sends out a given buffer respectively. You must
  implement them. See `tinywot_io_read_function_t` and
  `tinywot_io_write_function_t` for the required function signatures.

  \{
*/

/*!
  \brief Signature of a function reading data from the network.

  \param[out] ptr A pointer pointing to a valid region of memory.
  \param[in] toread The number of bytes to read and place to `ptr`.
  \param[out] read THe number of bytes that have actually been read.
  \return \ref tinywot_status
*/
typedef int
tinywot_io_read_function_t(uint8_t *ptr, size_t toread, size_t *read);

/*!
  \brief Signature of a function writing data to the network.

  \param[out] ptr A pointer pointing to a valid region of memory.
  \param[in] towrite The number of bytes to write from `ptr`.
  \param[out] written THe number of bytes that have actually been written out.
  \return \ref tinywot_status
*/
typedef int tinywot_io_write_function_t(
  uint8_t const *ptr, size_t towrite, size_t *written
);

/*!
  \brief A read function and a write function.
*/
struct tinywot_io {
  /*! \brief A pointer to the function reading data from the network. */
  tinywot_io_read_function_t *read;

  /*! \brief A pointer to the function writing data to the network. */
  tinywot_io_write_function_t *write;
};

/*! \} */ /* defgroup tinywot_io */

/*!
  \defgroup tinywot_protocol Protocol Stack Interfaces

  [&sect; 8.6 Protocol Stack Implementation, Web of Things (WoT)
  Architecture][wota-8.6] bridges the gap between the external world and a
  Web Thing by translating protocol messages from and to a [&sect; 8.2 WoT
  Runtime, Web of Things (WoT) Architecture][wota-8.2].

  [wota-8.6]:
  https://www.w3.org/TR/2020/REC-wot-architecture-20200409/#protocol-stack-implementation
  [wota-8.2]:
  https://www.w3.org/TR/2020/REC-wot-architecture-20200409/#wot-runtime
  \{
*/

/*!
  \brief Signature of a function producing a `tinywot_request` using
  \ref tinywot_io functions.

  \param[out] request A valid pointer to a `tinywot_request` storing the
  received network request.
  \param[in] io A `tinywot_io` containing a read function and a write function.
  \return \ref tinywot_status
  \sa \ref tinywot_io
*/
typedef int tinywot_protocol_receive_function_t(
  struct tinywot_request *request, struct tinywot_io const *io
);

/*!
  \brief Signature of a function producing a `tinywot_response` using
  \ref tinywot_io functions.

  \param[out] response A valid pointer to a `tinywot_response` storing the
  network response to be sent.
  \param[in] io A `tinywot_io` containing a read function and a write function.
  \return \ref tinywot_status
  \sa \ref tinywot_io
*/
typedef int tinywot_protocol_send_function_t(
  struct tinywot_response const *response, struct tinywot_io const *io
);

/*!
  \brief A receive function and a send function.
*/
struct tinywot_protocol {
  /*! \brief A pointer pointing to the receive. */
  tinywot_protocol_receive_function_t *receive;

  /*! \brief A pointer pointing to the send. */
  tinywot_protocol_send_function_t *send;
};

/*! \} */ /* defgroup tinywot_protocol */

/*!
  \defgroup tinywot_servient Servient

  [&sect; 6.7 WoT System Components and Their Interconnectivity, Web of
  Things (WoT) Architecture][wota-6.7] defines a _Servient_ as "a software
  stack that implements the WoT building blocks." It's a higher level
  abstraction of a _Web Thing_ implementation in software. To some extents,
  it can be seen as an _WoT App_.

  [wota-6.7]:
  https://www.w3.org/TR/2020/REC-wot-architecture-20200409/#sec-wot-servient-architecture-high-level

  \{
*/

/*!
  \brief A software stack implementing a Web Thing.
*/
struct tinywot_servient {
  /*! \brief \ref tinywot_thing */
  struct tinywot_thing const *thing;

  /*! \brief \ref tinywot_protocol */
  struct tinywot_protocol const *protocol;

  /*! \brief \ref tinywot_io */
  struct tinywot_io const *io;
};

/*!
  \brief Run the service routine of Servient once.

  \param[in] self A `tinywot_servient`.
  \return \ref tinywot_status
*/
int tinywot_servient_process(struct tinywot_servient const *self);

/*! \} */ /* defgroup tinywot_servient */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ifndef TINYWOT_H */
