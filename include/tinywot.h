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

  These macros define possible status codes used across the library. They are
  signed integer (`int`) values, with error codes being equal or less than `0`.

  User-implemented functions that are called by TinyWoT should also use them.

  \{
*/

/*!
  \brief There is insufficient memory to complete an action.
*/
#define TINYWOT_ERROR_NOT_ENOUGH_MEMORY (-5)

/*!
  \brief A requested operation is not allowed.

  This can happen when a path is registered to one or a few handlers accepting
  operations but the requested one.
*/
#define TINYWOT_ERROR_NOT_ALLOWED (-4)

/*!
  \brief A function is not implemented.

  This can happen when a path is registered to a `NULL` handler.
*/
#define TINYWOT_ERROR_NOT_IMPLEMENTED (-3)

/*!
  \brief Something is missing.

  This can happen when a requested path and operation type cannot match any
  handler registered in a `tinywot_thing` instance.
*/
#define TINYWOT_ERROR_NOT_FOUND (-2)

/*!
  \brief General error.

  This is used to indicate an error without specifying a reason.
*/
#define TINYWOT_ERROR_GENERAL 0

/*!
  \brief Success.

  This is not an error.
*/
#define TINYWOT_SUCCESS 1

/*!
  \brief Check if a status code indicates an error.

  \param[in] r An integer.
  \return `1` if `r` is an error, `0` if it is not.
*/
static inline bool tinywot_is_error(int r) { return r <= 0; }

/*!
  \brief Check if a status code indicates a success.

  \param[in] r An integer.
  \return `1` if `r` is a success, `0` if it is not.
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
  \b only this specific value is treated as an error by TinyWoT; others will
  be processed normally.

  [cf-reg]:
  https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#content-formats
  [RFC7252]: https://www.rfc-editor.org/rfc/rfc7252.html
*/
#define TINYWOT_CONTENT_TYPE_UNKNOWN 65535

/*!
  \brief A segment of memory with metadata.

  These attributes with the memory are frequently used together across the
  library.
*/
struct tinywot_scratchpad {
  /*!
    \brief A flag indicating whether `::data` is writable or not.

    This helps preventing accidental write to a constant, provided that this
    field is checked before attempting to write into `::data`.
  */
  bool read_write;

  /*!
    \brief An arbitrary unsigned integer hinting the type of data pointed by
    `data`.

    This field uses a number to represent a media type, but the exact meaning of
    this field is undefined. Different parties producing and consuming this
    structure may have different definitions on the meaningful values of this
    field. It is recommended to use [CoAP Content-Formats], although any
    unsigned integer can be used here -- hence the name "hint", as this is only
    an informative field.

    [CoAP Content-Formats]:
    https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#content-formats
  */
  uint16_t type_hint;

  /*!
    \brief The full size of memory pointed by `::data`, in bytes.
  */
  size_t size_byte;

  /*!
    \brief The actual meaningful size of content pointed by `::data`, in bytes.

    For example, if `::data` points to a string `"hello"`, then this should
    be 6 (including the terminal `NUL` character).
  */
  size_t valid_byte;

  /*!
    \brief A pointer to a segment of memory described by the other fields in
    this structure.

    If `::size_byte` is `0`, then this field can be `NULL`.
  */
  uint8_t *data;

  /*!
    \brief Load data to the memory pointed by `::data`.

    This provides a way for data producers to lazily supply data to a consumer.
    Setting this function pointer to non-`NULL` hints the use of such mechanism.
    The implementation of this function should load data to `::data` according
    to `cursor`, the meaning of which is opaque to the caller, but `0` should be
    the starting point, and should be set by the caller before the 1st call.
    After updating `::data`, `cursor` should be updated to indicate the next
    available chunk of data.

    \param[inout] self This scratchpad (although any one works).
    \param[inout] cursor An indicator of the starting point of memory that will
    be loaded and pointed by `::data`.
    \return \ref tinywot_status
  */
  int (*update)(struct tinywot_scratchpad *self, size_t *cursor);
};

/*!
  \brief Create a new scratchpad.

  This inline function is designed for scratchpad variable initialization,
  setting `tinywot_scratchpad::type_hint` to `TINYWOT_CONTENT_TYPE_UNKNOWN`.

  ```c
  struct tinywot_scratchpad scratchpad = tinywot_scratchpad_new();
  ```

  \return An initialized scratchpad.
  \sa `tinywot_scratchpad_initialize()`
*/
static inline struct tinywot_scratchpad tinywot_scratchpad_new(void)
{
  return (struct tinywot_scratchpad) {
    .type_hint = TINYWOT_CONTENT_TYPE_UNKNOWN,
  };
}

/*!
  \brief Initialize a scratchpad.

  This function is designed to be used on a pointer to an existing scratchpad,
  setting `tinywot_scratchpad::type_hint` to `TINYWOT_CONTENT_TYPE_UNKNOWN`.

  ```c
  struct tinywot_scratchpad scratchpad;
  tinywot_scratchpad_initialize(&scratchpad);
  ```

  \param[inout] self A pointer to a scratchpad.
  \sa `tinywot_scratchpad_new()`
*/
void tinywot_scratchpad_initialize(struct tinywot_scratchpad *self);

/*!
  \brief Split a scratchpad into two halves.

  `left` is also the scratchpad to be split. `right` needs to point to an empty
  `tinywot_scratchpad` and cannot be `NULL`, as this function will not allocate
  memory for it.

  `left` will have its full size reduced by `to_split_byte` bytes.
  `to_split_byte` bytes of memory is then taken from the end of the `left`
  scratchpad, forming the `right` scratchpad. Metadata is written back to their
  corresponding structure fields.

  If `left` does not have enough available room for a split, then the function
  will fail with `TINYWOT_ERROR_NOT_ENOUGH_MEMORY`.

  \param[inout] left The scratchpad to be split, as well as the left-hand-side
  scratchpad to create after the split.
  \param[inout] right The right-hand-side scratchpad to create after the split.
  \param[inout] to_split_byte How much memory should be sliced out of `left`, in
  bytes.
  \return \ref tinywot_status
*/
int tinywot_scratchpad_split(
  struct tinywot_scratchpad *restrict left,
  struct tinywot_scratchpad *restrict right,
  size_t to_split_byte
);

/*! \} */ /* defgroup tinywot_scratchpad */

/*!
  \defgroup tinywot_req_and_res Requests and Responses

  \{
*/

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

  `::TINYWOT_RESPONSE_STATUS_UNKNOWN` is a special value (`0`) indicating an
  uninitialized response status. TinyWoT treats this value as an error.
*/
enum tinywot_response_status {
  /*! \brief An unknown or uninitialized response status. */
  TINYWOT_RESPONSE_STATUS_UNKNOWN = 0,

  /*! \brief Operation successful. */
  TINYWOT_RESPONSE_STATUS_OK,

  /*! \brief Operation failed (usually due to a Thing-side problem). */
  TINYWOT_RESPONSE_STATUS_ERROR,

  /*! \brief Something is missing. */
  TINYWOT_RESPONSE_STATUS_NOT_FOUND,

  /*! \brief No implementation for the request. */
  TINYWOT_RESPONSE_STATUS_NOT_SUPPORTED,

  /*! \brief The request cannot be accepted. */
  TINYWOT_RESPONSE_STATUS_NOT_ALLOWED
};

/*!
  \brief A received request.
*/
struct tinywot_request {
  /*!
    \brief The path name of the target IRI in the request.

    The use of a scratchpad here is because it could be sliced from `::content`
    to use a single space provided by the user.

    \sa `tinywot_scratchpad_split()`
  */
  struct tinywot_scratchpad path;

  /*! \brief The operation type requested. */
  enum tinywot_operation_type op;

  /*! \brief The payload data sent along with the request. */
  struct tinywot_scratchpad *content;
};

/*!
  \brief A response to be sent.
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

  \param[inout] inout A memory buffer carrying input arguments and output
  values.
  \param[inout] user_data The context data registered in `tinywot_form`.
  \return \ref tinywot_status
*/
typedef int
tinywot_form_handler_t(struct tinywot_scratchpad *inout, void *user_data);

/*!
  \brief An operation endpoint.
*/
struct tinywot_form {
  /*!
    \brief The name of affordance that this form is attached to.

    For a top-level form, this field should be left empty (`NULL`).
  */
  char const *name;

  /*!
    \brief The path name of the IRI in `href` in the form.
  */
  char const *path;

  /*!
    \brief The allowed operation type on this form.
  */
  enum tinywot_operation_type op;

  /*!
    \brief A function pointer to the actual implementation of the form.
  */
  tinywot_form_handler_t *handler;

  /*!
    \brief Arbitrary data to pass to `::handler` when it is called.
  */
  void *user_data;
};

/*!
  \brief A representation of Thing.
*/
struct tinywot_thing {
  /*!
    \brief A flag indicating whether `::forms` is writable or not.

    This helps preventing accidental write to a constant, provided that this
    field is checked before attempting to write into `::forms`.
  */
  bool read_write;

  /*! \brief The number of registered forms in `::forms`. */
  size_t forms_count_n;

  /*! \brief The maximum number of forms in `::forms`. */
  size_t forms_max_n;

  /*! \brief A list of forms describing behaviors of this Thing. */
  struct tinywot_form *forms;
};

/*!
  \brief Initialize a Thing with an existing array of forms.

  This will also mark the Thing as read-only, as most often the supplied list
  is a constant.

  \param[inout] self A Thing.
  \param[in] forms An array of `tinywot_form`s.
  \param[in] forms_size_n How many `tinywot_forms` are in `forms`.
*/
void tinywot_thing_initialize_with_forms(
  struct tinywot_thing *self,
  struct tinywot_form const *forms,
  size_t forms_size_n
);

/*!
  \brief Initialize a Thing with a memory space.

  This makes the Thing dynamic. `tinywot_thing::forms_max_n` is calculated from
  `memory_size_byte`. Handlers are set with `tinywot_thing_set_handler()`.

  \param[inout] self A Thing.
  \param[in] memory A pointer to a memory space.
  \param[in] memory_size_byte The allocated size of `memory`.
*/
void tinywot_thing_initialize_with_memory(
  struct tinywot_thing *self, void *memory, size_t memory_size_byte
);

/*!
  \brief Find and get a handler from a Thing.

  Depending on the supplied arguments:

  - If both `name` and `path` are non-`NULL`, then the first closure with `name`
    on `path` is returned.
  - If `name` is non-`NULL`, `path` is `NULL`, then the first closure with
    `name` is returned.
  - If `name` is `NULL`, `path` is non-`NULL`, then the first closure on `path`
    is returned.
  - If both `name` and `path` are `NULL`, then the first closure allowing `op`
    is returned.

  In any case, `op` is checked for equality and does not prevent
  `::TINYWOT_OPERATION_TYPE_UNKNOWN` from being returned.

  \param[in] self A Thing.
  \param[in] name The name of affordance. This can be found on a corresponding
  Thing Description.
  \param[in] path The path name in IRI described on a corresponding Thing
  Description (in `href`).
  \param[in] op The operation type allowed on the handler we are looking for.
  \param[out] handler If found, the pointer to the handler function is returned
  here. Ignore this by setting it to `NULL`.
  \param[out] user_data The arbitrary data associated to `handler`, usually
  used to form a closure. Ignore this by setting it to `NULL`.
  \return \ref tinywot_status
  \sa `tinywot_thing_get_handler_by_name()`,
  `tinywot_thing_get_handler_by_path()`
*/
int tinywot_thing_get_handler(
  struct tinywot_thing const *self,
  char const *name,
  char const *path,
  enum tinywot_operation_type op,
  tinywot_form_handler_t **handler,
  void **user_data
);

/*!
  \brief Find and get a handler from a Thing according to the affordance name.

  This is a convenience wrapper on `tinywot_thing_get_handler()` without the
  `path` parameter.

  \param[in] self A Thing.
  \param[in] name The name of affordance. This can be found on a corresponding
  Thing Description.
  \param[in] op The operation type allowed on the handler we are looking for.
  \param[out] handler If found, the pointer to the handler function is returned
  here. Ignore this by setting it to `NULL`.
  \param[out] user_data The arbitrary data associated to `handler`, usually
  used to form a closure. Ignore this by setting it to `NULL`.
  \return \ref tinywot_status
  \sa `tinywot_thing_get_handler()`, `tinywot_thing_get_handler_by_path()`
*/
static inline int tinywot_thing_get_handler_by_name(
  struct tinywot_thing const *self,
  char const *name,
  enum tinywot_operation_type op,
  tinywot_form_handler_t **handler,
  void **user_data
)
{
  return tinywot_thing_get_handler(self, name, NULL, op, handler, user_data);
}

/*!
  \brief Find and get a handler from a Thing according to the affordance name.

  This is a convenience wrapper on `tinywot_thing_get_handler()` without the
  `name` parameter.

  \param[in] self A Thing.
  \param[in] path The path name in IRI described on a corresponding Thing
  Description (in `href`).
  \param[in] op The operation type allowed on the handler we are looking for.
  \param[out] handler If found, the pointer to the handler function is returned
  here. Ignore this by setting it to `NULL`.
  \param[out] user_data The arbitrary data associated to `handler`, usually
  used to form a closure. Ignore this by setting it to `NULL`.
  \return \ref tinywot_status
  \sa `tinywot_thing_get_handler()`, `tinywot_thing_get_handler_by_name()`
*/
static inline int tinywot_thing_get_handler_by_path(
  struct tinywot_thing const *self,
  char const *path,
  enum tinywot_operation_type op,
  tinywot_form_handler_t **handler,
  void **user_data
)
{
  return tinywot_thing_get_handler(self, NULL, path, op, handler, user_data);
}

/*!
  \brief Set a handler on an affordance.

  Note that `path` should be a path name, not a full IRI. The protocol stack
  implementation should take care of this and ignore the domain part after
  validation.

  \param[inout] self A Thing.
  \param[in] name The name of affordance. This can be found on a corresponding
  Thing Description.
  \param[in] path The path name in IRI described on a corresponding Thing
  Description.
  \param[in] op The operation type allowed on the handler we are looking for.
  \param[out] handler If found, the pointer to the handler function is returned
  here. If this is `NULL`, then this is not returned.
  \param[out] user_data The arbitrary data associated to `handler`, usually
  forming a closure.
  \return \ref tinywot_status
*/
int tinywot_thing_set_handler(
  struct tinywot_thing *self,
  char const *name,
  char const *path,
  enum tinywot_operation_type op,
  tinywot_form_handler_t *handler,
  void *user_data
);

/*!
  \brief Perform an operation on a Thing.

  The function invokes `tinywot_thing_get_handler()` first to find a handler
  based on `name`, `path` and `op`. See the documentation for that function for
  a detailed behavior of this function.

  \param[in] self A Thing.
  \param[in] name The name of affordance. This can be found on a corresponding
  Thing Description.
  \param[in] path The path name in IRI described on a corresponding Thing
  Description.
  \param[in] op The operation type allowed on the handler we are looking for.
  \param[inout] inout A buffer to supply to the invoked handler function for it
  to use as both a parameter input buffer and a return output buffer.
  \return \ref tinywot_status
  \sa `tinywot_thing_get_handler()`
*/
int tinywot_thing_do(
  struct tinywot_thing const *self,
  char const *name,
  char const *path,
  enum tinywot_operation_type op,
  struct tinywot_scratchpad *inout
);

/*!
  \brief Transform a Request into a Response with a Thing.

  \param[in] self A Thing.
  \param[in] request A received request.
  \param[out] response A response ready to be sent.
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
  \brief Signature of a function reading in raw data.

  \todo Clarify behavior.

  \param[inout] buffer A pointer pointing to a valid region of memory.
  \param[in] to_read_byte The number of bytes to read and place to `buffer`.
  \param[out] read_byte The number of bytes that have actually been read.
  \return \ref tinywot_status
*/
typedef int tinywot_io_read_function_t(
  unsigned char *buffer, size_t to_read_byte, size_t *read_byte
);

/*!
  \brief Signature of a function writing out raw data.

  \todo Clarify behavior.

  \param[in] buffer A pointer pointing to a valid region of memory.
  \param[in] to_write_byte The number of bytes to write from `buffer`.
  \param[out] written_byte THe number of bytes that have actually been written
  out.
  \return \ref tinywot_status
*/
typedef int tinywot_io_write_function_t(
  unsigned char const *buffer, size_t to_write_byte, size_t *written_byte
);

/*!
  \brief A read function and a write function.
*/
struct tinywot_io {
  /*! \brief A pointer to the function reading in raw data. */
  tinywot_io_read_function_t *read;

  /*! \brief A pointer to the function writing out raw data. */
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

  \param[inout] request A valid pointer to a `tinywot_request` storing the
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

  \param[in] response A valid pointer to a `tinywot_response` storing the
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
  struct tinywot_thing thing;

  /*! \brief \ref tinywot_protocol */
  struct tinywot_protocol protocol;

  /*! \brief \ref tinywot_io */
  struct tinywot_io io;
};

/*!
  \brief Run the service routine of Servient once.

  \param[in] self A `tinywot_servient` instance.
  \param[in] scratchpad A segment of memory for successive procedures to use.
  \return \ref tinywot_status
*/
int tinywot_servient_process(
  struct tinywot_servient const *self, struct tinywot_scratchpad *scratchpad
);

/*! \} */ /* defgroup tinywot_servient */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TINYWOT_H */
