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
#define TINYWOT_ERROR_NOT_ENOUGH_MEMORY  (-5)

/*!
  \brief A requested operation is not allowed.

  This can happen when a target can be matched, but does not accept the
  operation type requested.
*/
#define TINYWOT_ERROR_NOT_ALLOWED        (-4)

/*!
  \brief A function is not implemented.

  This can happen when a target is registered to a `NULL` handler.
*/
#define TINYWOT_ERROR_NOT_IMPLEMENTED    (-3)

/*!
  \brief Something is missing.

  This can happen when a requested target and operation type cannot match any
  handler registered in a `tinywot_thing` instance.
*/
#define TINYWOT_ERROR_NOT_FOUND          (-2)

/*!
  \brief General error.

  This is used to indicate an error without specifying a reason.
*/
#define TINYWOT_ERROR_GENERAL            (0)

/*!
  \brief Success.

  This is not an error.
*/
#define TINYWOT_SUCCESS                  (1)

/*!
  \brief Check if a status code indicates an error.

  \param[in] r An integer.
  \return `true` if `r` is an error, `false` if it is a success.
*/
static inline bool tinywot_is_error(int r) { return r <= 0; }

/*!
  \brief Check if a status code indicates a success.

  \param[in] r An integer.
  \return `true` if `r` is a success, `false` if it is an error.
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
#define TINYWOT_CONTENT_TYPE_UNKNOWN (65535)

/*!
  \brief A segment of memory with metadata.

  These attributes with the memory are frequently used together across the
  library.
*/
struct tinywot_scratchpad {
  /*!
    \brief A flag indicating whether `data` is writable or not.

    This helps preventing accidental write to a constant, provided that this
    field is checked before attempting to write into `data`.
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
  uint_least16_t type_hint;

  /*!
    \brief The full size of memory pointed by `data`, in bytes.
  */
  size_t size_byte;

  /*!
    \brief The actual meaningful size of content pointed by `data`, in bytes.

    For example, if `data` points to a string `"hello"`, then this should
    be 6 (including the terminal `NUL` character).
  */
  size_t valid_byte;

  /*!
    \brief A pointer to a segment of memory described by the other fields in
    this structure.

    If `size_byte` is `0`, then this field can be `NULL`.
  */
  unsigned char *data;

  /*!
    \brief Load data to the memory pointed by `data`.

    This provides a way for data producers to lazily supply data to a consumer.
    Setting this function pointer to non-`NULL` hints the use of such mechanism.
    The implementation of this function should load data to `data` according
    to `cursor`, the meaning of which is opaque to the caller, but `0` should be
    the starting point, and should be set by the caller before the 1st call.
    After updating `data`, `cursor` should be updated to indicate the next
    available chunk of data.

    \param[inout] self This scratchpad (although any one works).
    \param[inout] cursor An indicator of the starting point of memory that will
    be loaded and pointed by `data`.
    \return \ref tinywot_status
  */
  int (*update)(struct tinywot_scratchpad *self, size_t *cursor);

  /*!
    \brief Free the dynamically allocated memory pointed by `data`.

    This is only useful when `data` points to a dynamically-allocated memory.
    Usually, people do not do this on resource-constrained devices, but we do
    not prevent this either.

    This function should free the occupied memory and reset variables in `self`.

    \param[inout] self This scratchpad (although any one works).
  */
  void (*free)(struct tinywot_scratchpad *self);
};

/*!
  \brief Initialize a scratchpad.

  This function is designed to be used on a pointer to an existing scratchpad,
  setting `tinywot_scratchpad::type_hint` to `TINYWOT_CONTENT_TYPE_UNKNOWN`:

  ```
  struct tinywot_scratchpad sp;
  tinywot_scratchpad_initialize(&sp);
  ```

  \param[inout] self A pointer to a scratchpad.
  \sa `tinywot_scratchpad_new()`
*/
void tinywot_scratchpad_initialize(struct tinywot_scratchpad *self);

/*!
  \brief Create a new scratchpad.

  This inline function is designed for scratchpad variable initialization,
  setting `tinywot_scratchpad::type_hint` to `TINYWOT_CONTENT_TYPE_UNKNOWN`:

  ```
  struct tinywot_scrachpad sp = tinywot_scratchpad_new();
  ```

  \return An initialized scratchpad.
  \sa `tinywot_scratchpad_initialize()`
*/
static inline struct tinywot_scratchpad tinywot_scratchpad_new(void) {
  return (struct tinywot_scratchpad){
    .type_hint = TINYWOT_CONTENT_TYPE_UNKNOWN,
  };
}

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
  \param[in] to_split_byte How much memory should be sliced out of `left`, in
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
  \defgroup tinywot_operation_types Operation Types

  An operation type is a "semantic intention" of a request, according to the
  Thing Description specification.

  In TinyWoT, they are either used in a [Response] to indicate the incoming
  operation type, or in a [Form] to specify allowed operation types to the
  submission target.

  Operation types are bit-masks. Multiple operation types can thus be combined
  together with bitwise OR (`|`). For example, to specify a read-write property,
  write:

  ```
  uint_least32_t op =
    TINYWOT_OPERATION_TYPE_READPROPERTY | TINYWOT_OPERATION_TYPE_WRITEPROPERTY
  ```

  A full list of operation types can be found at [&sect; 5.3.4.2 Form, the
  Web of Things (WoT) Thing Description][td-5.3.4.2]. So far, TinyWoT only
  supports a subset of them.

  TinyWoT defines two extra operation types, but they are special values, and
  are not in the specification:

  - `TINYWOT_OPERATION_TYPE_UNKNOWN` is an uninitialized variable. It is invalid
    to use this value.
  - `TINYWOT_OPERATION_TYPE_ALL` represents all operation types. Because
    operation types in TinyWoT are bit-masks, this is an all-1 mask for use in
    `tinywot_form` to create a target without limiting its acceptable operation
    type.

  [Response]: \ref tinywot_response
  [Form]: \ref tinywot_form
  [td-5.3.4.2]:
  https://www.w3.org/TR/2020/REC-wot-thing-description-20200409/#form

  \{
*/

/*!
  \brief An unknown or uninitialized opeartion type.
*/
#define TINYWOT_OPERATION_TYPE_UNKNOWN                  (UINT32_C(0))

/*!
  \brief "Identifies the read operation on Property Affordances to retrieve
  the corresponding data."
*/
#define TINYWOT_OPERATION_TYPE_READPROPERTY             (UINT32_C(1))

/*!
  \brief "Identifies the write operation on Property Affordances to update the
  corresponding data."
*/
#define TINYWOT_OPERATION_TYPE_WRITEPROPERTY            (UINT32_C(1) << 1)

/*!
  \brief "Identifies the observe operation on Property Affordances to be
  notified with the new data when the Property is updated."
*/
#define TINYWOT_OPERATION_TYPE_OBSERVEPROPERTY          (UINT32_C(1) << 2)

/*!
  \brief "Identifies the unobserve operation on Property Affordances to stop
  the corresponding notifications."
*/
#define TINYWOT_OPERATION_TYPE_UNOBSERVEPROPERTY        (UINT32_C(1) << 3)

/*!
  \brief "Identifies the invoke operation on Action Affordances to perform
  the corresponding action."
*/
#define TINYWOT_OPERATION_TYPE_INVOKEACTION             (UINT32_C(1) << 4)

/*!
  \brief "Identifies the querying operation on Action Affordances to get the
  status of the corresponding action."
*/
#define TINYWOT_OPERATION_TYPE_QUERYACTION              (UINT32_C(1) << 5)

/*!
  \brief "Identifies the cancel operation on Action Affordances to cancel the
  ongoing corresponding action."
*/
#define TINYWOT_OPERATION_TYPE_CANCELACTION             (UINT32_C(1) << 6)

/*!
  \brief "Identifies the subscribe operation on Event Affordances to be
  notified by the Thing when the event occurs."
*/
#define TINYWOT_OPERATION_TYPE_SUBSCRIBEEVENT           (UINT32_C(1) << 7)

/*!
  \brief "Identifies the unsubscribe operation on Event Affordances to stop
  the corresponding notifications."
*/
#define TINYWOT_OPERATION_TYPE_UNSUBSCRIBEEVENT         (UINT32_C(1) << 8)

/*!
  \brief "Identifies the readallproperties operation on a Thing to retrieve
  the data of all Properties in a single interaction."
*/
#define TINYWOT_OPERATION_TYPE_READALLPROPERTIES        (UINT32_C(1) << 9)

/*!
  \brief "Identifies the writeallproperties operation on a Thing to update the
  data of all writable Properties in a single interaction."
*/
#define TINYWOT_OPERATION_TYPE_WRITEALLPROPERTIES       (UINT32_C(1) << 10)

/*!
  \brief "Identifies the readmultipleproperties operation on a Thing to
  retrieve the data of selected Properties in a single interaction."
*/
#define TINYWOT_OPERATION_TYPE_READMULTIPLEPROPERTIES   (UINT32_C(1) << 11)

/*!
  \brief "Identifies the writemultipleproperties operation on a Thing to
  update the data of selected writable Properties in a single interaction."
*/
#define TINYWOT_OPERATION_TYPE_WRITEMULTIPLEPROPERTIES  (UINT32_C(1) << 12)

/*!
  \brief "Identifies the observeallproperties operation on Properties to be
  notified with new data when any Property is updated."
*/
#define TINYWOT_OPERATION_TYPE_OBSERVEALLPROPERTIES     (UINT32_C(1) << 13)

/*!
  \brief "Identifies the unobserveallproperties operation on Properties to
  stop notifications from all Properties in a single interaction."
*/
#define TINYWOT_OPERATION_TYPE_UNOBSERVEALLPROPERTIES   (UINT32_C(1) << 14)

/*!
  \brief "Identifies the queryallactions operation on a Thing to get the
  status of all Actions in a single interaction."
*/
#define TINYWOT_OPERATION_TYPE_QUERYALLACTIONS          (UINT32_C(1) << 15)

/*!
  \brief "Identifies the subscribeallevents operation on Events to subscribe
  to notifications from all Events in a single interaction."
*/
#define TINYWOT_OPERATION_TYPE_SUBSCRIBEALLEVENTS       (UINT32_C(1) << 16)

/*!
  \brief "Identifies the unsubscribeallevents operation on Events to
  unsubscribe from notifications from all Events in a single interaction."
*/
#define TINYWOT_OPERATION_TYPE_UNSUBSCRIBEALLEVENTS     (UINT32_C(1) << 17)

/*!
  \brief All of the operation types defined in the library.
*/
#define TINYWOT_OPERATION_TYPE_ALL                      (~UINT32_C(0))

/*! \} */ /* defgroup tinywot_operation_types */

/*!
  \defgroup tinywot_req_and_res Requests and Responses

  \{
*/

/*!
  \brief Status codes used in a `tinywot_response`.

  `::TINYWOT_RESPONSE_STATUS_UNKNOWN` is a special value indicating an
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
  \brief An incoming network request.
*/
struct tinywot_request {
  /*!
    \brief The submission target extracted from the request.

    This field is designed to be splitted from the [Scratchpad] supplied by the
    library user.

    \sa `tinywot_scratchpad_split()`

    [Scratchpad]: \ref tinywot_scratchpad
  */
  struct tinywot_scratchpad target;

  /*!
    \brief The [Operation Type] extracted from the request.

    [Operation Type]: \ref tinywot_operation_types
  */
  uint_least32_t op;

  /*!
    \brief The payload data sent along with the request.
  */
  struct tinywot_scratchpad *content;
};

/*!
  \brief An outgoing network response.
*/
struct tinywot_response {
  /*!
    \brief A status code for the response.

    This is normally calculated from the \ref tinywot_status by the library.
  */
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
  \brief Signature of a handler function implementing the behavior of a
  submission target.

  \param[in] name The name of affordance specified in a [Form].
  \param[in] target The submission target of the [Form] that this function is
  handling.
  \param[in] op The [Operation Type] of request.
  \param[inout] content A dual-use [Scratchpad] containing the incoming payload
  and acting as a buffer for sending response payload.
  \param[inout] user_data The context information set in the corresponding
  [Form].
  \return \ref tinywot_status

  [Form]: \ref tinywot_form
  [Operation Type]: \ref tinywot_operation_types
  [Scratchpad]: \ref tinywot_scratchpad
*/
typedef int tinywot_form_handler_t(
  char const *name,
  char const *target,
  uint_least32_t op,
  struct tinywot_scratchpad *content,
  void *user_data
);

/*!
  \brief An operation endpoint.
*/
struct tinywot_form {
  /*!
    \brief The name of affordance that this form is attached to.

    For a top-level form, this field should be set to `NULL`.

    This field is so far informative only -- no procedure relies on its value.
    The form handler (`handler`) receives this as a parameter, and can use it
    as a reference.
  */
  char const *name;

  /*!
    \brief The submission target of the form.

    The exact way to specify a _submission target_ is undefined. Often, for an
    IRI specified in a Thing Description document, this should be the path name
    of it. For example, for the following IRI:

        http://thing.example.com/action/update

    Its path name is `/action/update`, so this field should be `/action/update`.
    This largely depends on the protocol implementation, who is responsible for
    extracting this information from the request message, and methods like
    `tinywot_thing_do()` finds the form according to this field. Check their
    documentation for more information.
  */
  char const *target;

  /*!
    \brief The allowed operation types on this form.

    \ref tinywot_operation_types can be bitwise-ORed here to allow multiple
    operation types:

    ```
    form.allowed_ops =
      TINYWOT_OPERATION_TYPE_READPROPERTY |
      TINYWOT_OPERATION_TYPE_WRITEPROPERTY;
    ```

    `TINYWOT_OPERATION_TYPE_ALL` can also be used to indicate that all
    operations are allowed on this form. See \ref tinywot_operation_types for
    more details about opeartion types.
  */
  uint_least32_t allowed_ops;

  /*!
    \brief A function pointer to the actual implementation of the form.
  */
  tinywot_form_handler_t *handler;

  /*!
    \brief Arbitrary data to pass to `handler` when it is called.
  */
  void *user_data;
};

/*!
  \brief A representation of Thing.
*/
struct tinywot_thing {
  /*!
    \brief A flag indicating whether the `forms` field is writable or not.

    This helps preventing accidental write to a (true) constant, provided that
    this field is checked before attempting to write into `forms`.
  */
  bool read_write;

  /*! \brief The number of registered forms in `forms`. */
  size_t forms_count_n;

  /*! \brief The maximum number of forms in `forms`. */
  size_t forms_max_n;

  /*! \brief A list of forms describing behaviors of this Thing. */
  struct tinywot_form const *forms;
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
  struct tinywot_thing *self,
  void *memory,
  size_t memory_size_byte
);

/*!
  \brief Find and get a [Form] from a [Thing].

  \param[in] self A [Thing].
  \param[in] target The submission target. This can be derived from a
  corresponding Thing Description.
  \param[in] allowed_ops The operation type(s) allowed on the anticipated form.
  \param[out] form A pointer to the [Form] that matches conditions.
  \return \ref tinywot_status

  [Form]: \ref tinywot_form
  [Thing]: \ref tinywot_thing
*/
int tinywot_thing_get_form(
  struct tinywot_thing const *self,
  char const *target,
  uint_least32_t allowed_ops,
  struct tinywot_form const **form
);

/*!
  \brief Set a [Form] to a [Thing].

  If another form in the [Thing] has its `tinywot_form::target` and
  `tinywot_form::op` being the same as the supplied one, its contents will be
  replaced, instead of a new one being inserted into the list directly.

  \param[inout] self A [Thing].
  \param[in] form The [Form] to be registered to the [Thing].
  \return \ref tinywot_status

  [Form]: \ref tinywot_form
  [Thing]: \ref tinywot_thing
*/
int tinywot_thing_set_form(
  struct tinywot_thing *self,
  struct tinywot_form const *form
);

/*!
  \brief Perform an operation on a [Thing].

  This function first invokes `tinywot_thing_get_form()` to find a matching
  submission target, then try to invoke the handler of it.

  \param[in] self A Thing.
  \param[in] target The submission target. This can be derived from a
  corresponding Thing Description.
  \param[in] op The operation type allowed on the handler we are looking for.
  \param[inout] inout A buffer to supply to the invoked handler function for it
  to use as both a parameter input buffer and a return output buffer.
  \return \ref tinywot_status

  [Form]: \ref tinywot_form
  [Thing]: \ref tinywot_thing
*/
int tinywot_thing_do(
  struct tinywot_thing const *self,
  char const *target,
  uint_least32_t op,
  struct tinywot_scratchpad *inout
);

/*!
  \brief Process a [Request] transforming it into a [Response] with a [Thing].

  \param[in] self A [Thing].
  \param[in] request A [Request].
  \param[out] response A [Response].
  \return \ref tinywot_status

  [Request]: \ref tinywot_request
  [Response]: \ref tinywot_response
  [Thing]: \ref tinywot_thing
*/
int tinywot_thing_process_request(
  struct tinywot_thing const *self,
  struct tinywot_request *request,
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

  \param[out] buffer A pointer pointing to a valid region of memory.
  \param[in] to_read_byte The number of bytes to read and place to `buffer`.
  \param[out] read_byte The number of bytes that have actually been read.
  \return \ref tinywot_status
*/
typedef int tinywot_io_read_function_t(
  unsigned char *buffer,
  size_t to_read_byte,
  size_t *read_byte
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
  unsigned char const *buffer,
  size_t to_write_byte,
  size_t *written_byte
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
  \brief A function that reads with [I/O functions] and produces a [Request].

  \param[inout] request A pointer to a valid [Request].
  \param[in] io [I/O functions].
  \return \ref tinywot_status

  [Request]: \ref tinywot_request
  [I/O functions]: \ref tinywot_io
*/
typedef int tinywot_protocol_receive_function_t(
  struct tinywot_request *request,
  struct tinywot_io const *io
);

/*!
  \brief A function that writes with [I/O functions] and a [Response].

  \param[inout] response A pointer to a valid [Response].
  \param[in] io [I/O functions].
  \return \ref tinywot_status

  [Response]: \ref tinywot_response
  [I/O functions]: \ref tinywot_io
*/
typedef int tinywot_protocol_send_function_t(
  struct tinywot_response *response,
  struct tinywot_io const *io
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
  \brief Run the Thing's service routine once.

  \param[in] self A [Servient] instance.
  \param[in] scratchpad A segment of memory for successive procedures to use.
  \return \ref tinywot_status

  [Servient]: \ref tinywot_servient
*/
int tinywot_servient_process(
  struct tinywot_servient const *self,
  struct tinywot_scratchpad *scratchpad
);

/*! \} */ /* defgroup tinywot_servient */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TINYWOT_H */
