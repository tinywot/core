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
  \defgroup grp_sc Status Codes

  These are function return values used across the library. User-implemented
  functions should use them too.

  They are signed values. All values less than or equal to `0` are errors. `1`
  is the only successful status, albeit all values greater than `0` are reserved
  for successful status.

  \{
*/

/*!
  \brief The type of status codes recognized and returned by the library.
*/
typedef signed char tinywot_status_t;

/*!
  \brief Attempt to write to a read-only data structure.
*/
#define TINYWOT_ERROR_READ_ONLY          ((tinywot_status_t)-6)

/*!
  \brief There is insufficient memory to complete an action.
*/
#define TINYWOT_ERROR_NOT_ENOUGH_MEMORY  ((tinywot_status_t)-5)

/*!
  \brief A requested operation is not allowed.

  This can happen when a target can be matched, but does not accept the
  operation type requested.
*/
#define TINYWOT_ERROR_NOT_ALLOWED        ((tinywot_status_t)-4)

/*!
  \brief A function is not implemented.

  This can happen when a target is registered to a `NULL` handler.
*/
#define TINYWOT_ERROR_NOT_IMPLEMENTED    ((tinywot_status_t)-3)

/*!
  \brief Something is missing.

  This can happen when a requested target and operation type cannot match any
  handler registered in a `tinywot_thing` instance.
*/
#define TINYWOT_ERROR_NOT_FOUND          ((tinywot_status_t)-2)

/*!
  \brief General error.

  This is used to indicate an error without specifying a reason.
*/
#define TINYWOT_ERROR_GENERAL            ((tinywot_status_t)0)

/*!
  \brief Success.

  This is not an error.
*/
#define TINYWOT_SUCCESS                  ((tinywot_status_t)1)

/*!
  \brief Check if a status code indicates an error.

  \param[in] sc A status code returned by the library.
  \return `true` if `sc` is an error, `false` if it is a success.
*/
static inline bool tinywot_is_error(tinywot_status_t sc) {
  return sc <= ((tinywot_status_t)0);
}

/*!
  \brief Check if a status code indicates a success.

  \param[in] sc A status code returned by the library.
  \return `true` if `sc` is an success, `false` if it is an error.
*/
static inline bool tinywot_is_success(tinywot_status_t sc) {
  return sc > ((tinywot_status_t)0);
}

/*! \} */ /* grp_sc */

/*!
  \defgroup grp_flags Flags

  Flag values are used across the library to mark attributes of a data structure
  in a compact way. They are bit flags that can be bitwise OR-ed together to
  indicate multiple attributes.

  \{
*/

/*!
  \brief The type of a flag value.
*/
typedef unsigned char tinywot_flag_t;

/*!
  \defgroup grp_flags_is_read_only Flags - Is Read Only

  This flag indicates whether the associated value can be written or not. User-
  supplied contents can locate in either RAM or ROM, and often this cannot be
  indicated by the variable qualifier. This flag helps with distinguishing this
  attribute in run-time.

  \{
*/

/*! \brief The bit mask of this flag. */
#define TINYWOT_FLAG_READ_ONLY (((tinywot_flag_t)1) << 0)

/*!
  \brief Check if a flag value says "read only".

  \param[in] flags A flags field from somewhere.
  \return `true` if `flags` has its read-only bit set; `false` otherwise.
  \sa `tinywot_flag_is_read_write`
*/
static inline bool tinywot_flag_is_read_only(tinywot_flag_t flags) {
  return flags & TINYWOT_FLAG_READ_ONLY;
}

/*!
  \brief Check if a flag value says "read and write".

  \param[in] flags A flags field from somewhere.
  \return `true` if `flags` doesn't have its read-only bit set; `false`
  otherwise.
  \sa `tinywot_flag_is_read_only`
*/
static inline bool tinywot_flag_is_read_write(tinywot_flag_t flags) {
  return !tinywot_flag_is_read_only(flags);
}

/*! \} */ /* grp_flags_is_read_only */

/*!
  \defgroup grp_flags_is_dynamic Flags - Is Dynamic

  This flag indicates whether the associated value is from the heap or not.
  Although not recommended in embedded platforms, this is possible. If this flag
  is present, then relevant functions should free the memory occupied by the
  associated value. Otherwise, they should take no action.

  \{
*/

/*! \brief The bit mask of this flag. */
#define TINYWOT_FLAG_DYNAMIC (((tinywot_flag_t)1) << 1)

/*!
  \brief Check if a flag value says "dynamic memory".

  \param[in] flags A flags field from somewhere.
  \return `true` if `flags` has its dynamic memory bit set; `false` otherwise.
  \sa `tinywot_flag_is_static`
*/
static inline bool tinywot_flag_is_dynamic(tinywot_flag_t flags) {
  return flags & TINYWOT_FLAG_DYNAMIC;
}

/*!
  \brief Check if a flag value says "static memory".

  \param[in] flags A flags field from somewhere.
  \return `true` if `flags` doesn't have its dynamic memory bit set; `false`
  otherwise.
  \sa `tinywot_flag_is_dynamic`
*/
static inline bool tinywot_flag_is_static(tinywot_flag_t flags) {
  return !tinywot_flag_is_dynamic(flags);
}

/*! \} */ /* grp_flags_is_dynamic */

/*! \} */ /* grp_flags */

/*!
  \defgroup grp_pld Payload

  A payload can be the content of a network request or a network response.
  It is usually associated with a segment of memory. Most of the time, it acts
  as a buffer with metadata associated with it.

  \{
*/

/*!
  \brief A segment of memory with metadata.
*/
struct tinywot_payload {
  /*! \brief Attributes of this data structure. */
  tinywot_flag_t flags;

  /*! \brief The allocated memory size of `content` in byte. */
  size_t size_byte;

  /*! \brief An indicator of the type of `content`. */
  unsigned int content_type;

  /*! \brief The valid data length of `content`. */
  size_t content_length_byte;

  /*!
    \brief An opaque data structure holding the state of stream / cursor
    information from `update`.
  */
  void *content_cursor;

  /*!
    \brief A function for pulling content out from somewhere to `content`.
  */
  tinywot_status_t (*update)(struct tinywot_payload *self);

  /*!
    \brief A pointer to a segment of consectutive memory.
  */
  unsigned char *content;
};

/*! \} */ /* grp_pld */

/*!
  \defgroup grp_op_ty Operation Types

  An operation type is a "semantic intention" of a request, according to the
  Thing Description specification.

  In TinyWoT, they are either used in a [Response] to indicate the incoming
  operation type, or in a [Form] to specify allowed operation types to the
  submission target.

  Operation types are bit-masks. Multiple operation types can thus be combined
  together with bitwise OR (`|`). For example, to specify a read-write property,
  write:

  ```
  tinywot_operation_type_t operation_type =
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
  \brief The data type of an operation type.
*/
typedef unsigned long tinywot_operation_type_t;

/*!
  \brief An unknown or uninitialized opeartion type.
*/
#define TINYWOT_OPERATION_TYPE_UNKNOWN                 \
  ((tinywot_operation_type_t)0)

/*!
  \brief "Identifies the read operation on Property Affordances to retrieve
  the corresponding data."
*/
#define TINYWOT_OPERATION_TYPE_READPROPERTY            \
  ((tinywot_operation_type_t)1)

/*!
  \brief "Identifies the write operation on Property Affordances to update the
  corresponding data."
*/
#define TINYWOT_OPERATION_TYPE_WRITEPROPERTY           \
  (((tinywot_operation_type_t)1) << 1)

/*!
  \brief "Identifies the observe operation on Property Affordances to be
  notified with the new data when the Property is updated."
*/
#define TINYWOT_OPERATION_TYPE_OBSERVEPROPERTY         \
  (((tinywot_operation_type_t)1) << 2)

/*!
  \brief "Identifies the unobserve operation on Property Affordances to stop
  the corresponding notifications."
*/
#define TINYWOT_OPERATION_TYPE_UNOBSERVEPROPERTY       \
  (((tinywot_operation_type_t)1) << 3)

/*!
  \brief "Identifies the invoke operation on Action Affordances to perform
  the corresponding action."
*/
#define TINYWOT_OPERATION_TYPE_INVOKEACTION            \
  (((tinywot_operation_type_t)1) << 4)

/*!
  \brief "Identifies the querying operation on Action Affordances to get the
  status of the corresponding action."
*/
#define TINYWOT_OPERATION_TYPE_QUERYACTION             \
  (((tinywot_operation_type_t)1) << 5)

/*!
  \brief "Identifies the cancel operation on Action Affordances to cancel the
  ongoing corresponding action."
*/
#define TINYWOT_OPERATION_TYPE_CANCELACTION            \
  (((tinywot_operation_type_t)1) << 6)

/*!
  \brief "Identifies the subscribe operation on Event Affordances to be
  notified by the Thing when the event occurs."
*/
#define TINYWOT_OPERATION_TYPE_SUBSCRIBEEVENT          \
  (((tinywot_operation_type_t)1) << 7)

/*!
  \brief "Identifies the unsubscribe operation on Event Affordances to stop
  the corresponding notifications."
*/
#define TINYWOT_OPERATION_TYPE_UNSUBSCRIBEEVENT        \
  (((tinywot_operation_type_t)1) << 8)

/*!
  \brief "Identifies the readallproperties operation on a Thing to retrieve
  the data of all Properties in a single interaction."
*/
#define TINYWOT_OPERATION_TYPE_READALLPROPERTIES       \
  (((tinywot_operation_type_t)1) << 9)

/*!
  \brief "Identifies the writeallproperties operation on a Thing to update the
  data of all writable Properties in a single interaction."
*/
#define TINYWOT_OPERATION_TYPE_WRITEALLPROPERTIES      \
  (((tinywot_operation_type_t)1) << 10)

/*!
  \brief "Identifies the readmultipleproperties operation on a Thing to
  retrieve the data of selected Properties in a single interaction."
*/
#define TINYWOT_OPERATION_TYPE_READMULTIPLEPROPERTIES  \
  (((tinywot_operation_type_t)1) << 11)

/*!
  \brief "Identifies the writemultipleproperties operation on a Thing to
  update the data of selected writable Properties in a single interaction."
*/
#define TINYWOT_OPERATION_TYPE_WRITEMULTIPLEPROPERTIES \
  (((tinywot_operation_type_t)1) << 12)

/*!
  \brief "Identifies the observeallproperties operation on Properties to be
  notified with new data when any Property is updated."
*/
#define TINYWOT_OPERATION_TYPE_OBSERVEALLPROPERTIES    \
  (((tinywot_operation_type_t)1) << 13)

/*!
  \brief "Identifies the unobserveallproperties operation on Properties to
  stop notifications from all Properties in a single interaction."
*/
#define TINYWOT_OPERATION_TYPE_UNOBSERVEALLPROPERTIES  \
  (((tinywot_operation_type_t)1) << 14)

/*!
  \brief "Identifies the queryallactions operation on a Thing to get the
  status of all Actions in a single interaction."
*/
#define TINYWOT_OPERATION_TYPE_QUERYALLACTIONS         \
  (((tinywot_operation_type_t)1) << 15)

/*!
  \brief "Identifies the subscribeallevents operation on Events to subscribe
  to notifications from all Events in a single interaction."
*/
#define TINYWOT_OPERATION_TYPE_SUBSCRIBEALLEVENTS      \
  (((tinywot_operation_type_t)1) << 16)

/*!
  \brief "Identifies the unsubscribeallevents operation on Events to
  unsubscribe from notifications from all Events in a single interaction."
*/
#define TINYWOT_OPERATION_TYPE_UNSUBSCRIBEALLEVENTS    \
  (((tinywot_operation_type_t)1) << 17)

/*!
  \brief All of the operation types defined in the library.
*/
#define TINYWOT_OPERATION_TYPE_ALL                      \
  (~((tinywot_operation_type_t)0))

/*! \} */ /* grp_op_ty */

/*!
  \defgroup grp_req_res Requests and Responses

  \{
*/

/*!
  \defgroup grp_res_status Response Status

  \{
*/

/*! \brief The data type of a response status. */
typedef unsigned char tinywot_response_status_t;

/*! \brief An uninitialized status code. */
#define TINYWOT_RESPONSE_STATUS_UNKNOWN       ((tinywot_response_status_t)0)

/*! \brief All good. */
#define TINYWOT_RESPONSE_STATUS_OK            ((tinywot_response_status_t)1)

/*! \brief A general error. */
#define TINYWOT_RESPONSE_STATUS_ERROR         ((tinywot_response_status_t)2)

/*! \brief Something could not be found. */
#define TINYWOT_RESPONSE_STATUS_NOT_FOUND     ((tinywot_response_status_t)3)

/*! \brief The requested action has not been implemented. */
#define TINYWOT_RESPONSE_STATUS_NOT_SUPPORTED ((tinywot_response_status_t)4)

/*! \brief The requested action is not allowed on the target. */
#define TINYWOT_RESPONSE_STATUS_NOT_ALLOWED   ((tinywot_response_status_t)5)

/*! \} */ /* grp_res_status */

/*!
  \brief A request of operation.
*/
struct tinywot_request {
  /*! \brief The intended submision target extracted from the request. */
  char const *target;

  /*! \brief The intended operation type extracted from the request. */
  tinywot_operation_type_t operation_type;

  /*! \brief The content carried with the request. */
  struct tinywot_payload *payload;
};

/*!
  \brief A response as a result of an operation.
*/
struct tinywot_response {
  /*!
    \brief A status code for the response.

    This is normally calculated from the \ref grp_sc returned by functions.
  */
  tinywot_response_status_t status;

  /*! \brief The content to send with the response. */
  struct tinywot_payload *payload;
};

/*! \} */ /* grp_req_res */

/*!
  \defgroup grp_impl Behavior Implementation

  \{
*/

/*!
  \brief Signature of a handler function implementing the behavior of a
  submission target.

  \param[in] name The name of affordance specified in a [Form].
  \param[in] target The submission target of the [Form] that this function is
  handling.
  \param[in] operation_type The [Operation Type] of request.
  \param[inout] payload A [Payload] data structure used for both acquiring
  incoming data and sending outgoing data.
  \param[inout] user_data The context information set in the corresponding
  [Form].
  \return \ref grp_sc

  [Form]: \ref tinywot_form
  [Operation Type]: \ref grp_op_ty
  [Payload]: \ref tinywot_payload
*/
typedef int tinywot_form_handler_t(
  char const *name,
  char const *target,
  tinywot_operation_type_t operation_type,
  struct tinywot_payload *payload,
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

    \ref grp_op_ty can be bitwise-ORed here to allow multiple
    operation types:

    ```
    form.allowed_operation_types = TINYWOT_OPERATION_TYPE_READPROPERTY |
      TINYWOT_OPERATION_TYPE_WRITEPROPERTY;
    ```

    `TINYWOT_OPERATION_TYPE_ALL` can also be used to indicate that all
    operations are allowed on this form. See \ref grp_op_ty for
    more details about opeartion types.
  */
  tinywot_operation_type_t allowed_operation_types;

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
  \brief A Thing.
*/
struct tinywot_thing {
  /*! \brief Attributes associated with this Thing. */
  tinywot_flag_t flags;

  /*! \brief The number of registered forms in `forms`. */
  size_t forms_count_n;

  /*! \brief The maximum number of forms in `forms`. */
  size_t forms_max_n;

  /*! \brief A list of forms describing behaviors of this Thing. */
  struct tinywot_form const *forms;
};

/*!
  \brief Initialize a [Thing] with an existing array of [Form]s.

  The memory pointed by `forms` will be regarded as an already-initialized,
  valid [Form] array. `self->forms_count_n` and `self->forms_max_n` will both be
  set to the supplied `forms_size_n`. If this is undesirable (e.g. the available
  memory is actually bigger), change `tinywot_thing::forms_max_n` afterwards.

  Usually, the [Form] array is pre-declared as a constant stored in ROM. If this
  is the case, set `::TINYWOT_FLAG_READ_ONLY` in `flags` to indicate that the
  [Form] array cannot be changed.

  \param[inout] self A [Thing].
  \param[in] forms An array of [Form]s.
  \param[in] forms_size_n How many [Form]s are there.
  \param[in] flags Attributes apply to the initializing [Thing].

  [Thing]: \ref tinywot_thing
  [Form]: \ref tinywot_form
*/
void tinywot_thing_initialize_with_forms(
  struct tinywot_thing *self,
  struct tinywot_form const *forms,
  size_t forms_size_n,
  tinywot_flag_t flags
);

/*!
  \brief Initialize a [Thing] with an empty memory space.

  The memory pointed by `memory` will be treated as an empty memory space.
  `self->forms_count_n` will be set to `0`, while `self->forms_max_n` will be
  calculated from the supplied `memory_size_byte`.

  Use this function to make it possible to modify the initialized [Thing], such
  as adding or removing [Form]s. If `memory` is allocated from the heap, set
  `::TINYWOT_FLAG_DYNAMIC` in `flags` to indicate that it should be freed when
  the [Thing] is destructed.

  \param[inout] self A [Thing].
  \param[in] memory A pointer to a memory space.
  \param[in] memory_size_byte The allocated size of `memory` in bytes.
  \param[in] flags Attributes apply to the initializing [Thing].

  [Thing]: \ref tinywot_thing
  [Form]: \ref tinywot_form
*/
void tinywot_thing_initialize_with_memory(
  struct tinywot_thing *self,
  void *memory,
  size_t memory_size_byte,
  tinywot_flag_t flags
);

/*!
  \brief Find a [Form] from a [Thing].

  A [Form] is found when:

  - The supplied `target` equals to `tinywot_form::target`
  - The supplied `operation_types` _matches_
    `tinywot_form::allowed_operation_types` (that is, all bits are set within
    the mask).

  \param[in] self A [Thing].
  \param[in] target The submission target.
  \param[in] operation_types The target operation type(s).
  \param[out] form A pointer to the [Form] that matches the supplied conditions.
  \return
    - `::TINYWOT_SUCCESS` if a [Form] is found. `form` will point to that
      [Form].
    - `::TINYWOT_ERROR_NOT_FOUND` if a [Form] is not found. The value of `form`
      is undefined in this case.

  [Thing]: \ref tinywot_thing
  [Form]: \ref tinywot_form
*/
tinywot_status_t tinywot_thing_get_form(
  struct tinywot_thing const *self,
  char const *target,
  tinywot_operation_type_t operation_types,
  struct tinywot_form const **form
);

/*!
  \brief Set a [Form] to a [Thing].

  This copies the content of `form` into the [Thing] if it is not read-only; the
  caller do not have to allocate a heap space for `form`.

  Before attempting to insert a new [Form] into the [Thing], this function tries
  to use `form->target` and `form->allowed_operation_types` to find an existing
  slot in `self`. If one is found, then the slot will be overwritten with the
  supplied `form`. An existing [Form] is found when:

  - The supplied `target` equals to `tinywot_form::target`
  - The supplied `operation_types` _is not mutually exclusive to_
    `tinywot_form::allowed_operation_types` (that is, if any bit set in
    `operation_types` is also set in `tinywot_form::allowed_operation_types`).
    This is different from `tinywot_thing_get_form()`. For example, a [Form]
    with target `foo` and allowed operation types `READPROPERTY` will be
    replaced by a new form with target `foo` and allowed operation types
    `READPROPERTY | WRITEPROPERTY`. A new [Form] with `INVOKEACTION` will
    instead be appended as a new entry.

  If `self` is read-only, then this function fails with
  `::TINYWOT_ERROR_NOT_ALLOWED`.

  \param[inout] self A [Thing].
  \param[in] form The [Form] to be registered to the [Thing].
  \return
    - `::TINYWOT_SUCCESS` if `form` has been successfully copied into `self`.
    - `::TINYWOT_ERROR_NOT_ALLOWED` if `form` cannot be written into `self`,
      usually because `self` is read-only.
    - `::TINYWOT_ERROR_NOT_ENOUGH_MEMORY` if no more [Form] can be added to
      `self` because there is no space left.

  [Thing]: \ref tinywot_thing
  [Form]: \ref tinywot_form
*/
tinywot_status_t tinywot_thing_set_form(
  struct tinywot_thing *self,
  struct tinywot_form const *form
);

/*!
  \brief Perform an operation on a [Thing].

  This function first invokes `tinywot_thing_get_form()` to find a matching
  [Form], then tries to invoke the handler of it.

  \param[in] self A [Thing].
  \param[in] target The submission target.
  \param[in] operation_type The target operation type.
  \param[inout] payload The content of request. It is reused in the handler to
  place the content of response.
  \return What the actual handler of the invoked [Form] returns, plus:
    - `::TINYWOT_ERROR_NOT_FOUND` if `target` is not found.
    - `::TINYWOT_ERROR_NOT_ALLOWED` if `target` is found, but `operation_type`
      does not match.
    - `::TINYWOT_ERROR_NOT_IMPLEMENTED` if a [Form] is found, but there is no
      handler registered (`NULL`).

  [Thing]: \ref tinywot_thing
  [Form]: \ref tinywot_form
*/
tinywot_status_t tinywot_thing_do(
  struct tinywot_thing const *self,
  char const *target,
  tinywot_operation_type_t operation_type,
  struct tinywot_payload *payload
);

/*!
  \brief Process a [Request], transforming it into a [Response] with a [Thing].

  `request` should be a valid one ready for handling. `response` will be
  overwritten by the subsequent functions.

  Note that `request->payload` will be reused and moved to `response->payload`
  after this function.

  \param[in] self A [Thing].
  \param[inout] request A [Request] to be processed by the registered handler
  in `self`.
  \param[inout] response An empty [Response] to be filled in.
  \return What `tinywot_thing_do()` returns.

  [Request]: \ref tinywot_request
  [Response]: \ref tinywot_response
  [Thing]: \ref tinywot_thing
*/
tinywot_status_t tinywot_thing_process_request(
  struct tinywot_thing const *self,
  struct tinywot_request *request,
  struct tinywot_response *response
);

/*! \} */ /* grp_impl */

/*!
  \defgroup grp_io Platform-specific Input / Output Interfaces

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
  \return \ref grp_sc
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
  \return \ref grp_sc
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

/*! \} */ /* grp_io */

/*!
  \defgroup grp_proto Protocol Stack Interfaces

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
  \return \ref grp_sc

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
  \return \ref grp_sc

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

/*! \} */ /* grp_proto */

/*!
  \defgroup grp_servient Servient

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
  /*! \brief Attributes associated with this Servient. */
  tinywot_flag_t flags;

  /*! \brief \ref grp_impl */
  struct tinywot_thing thing;

  /*! \brief \ref grp_proto */
  struct tinywot_protocol protocol;

  /*! \brief \ref grp_io */
  struct tinywot_io io;

  /*! \brief A segment of memory for the exclusive use of the Servient. */
  void *memory;

  /*! \brief The size of `memory` in bytes. */
  size_t memory_size_byte;
};

/*!
  \brief Run the Thing's service routine once.

  \param[in] self A [Servient] instance.
  \return \ref grp_sc

  [Servient]: \ref tinywot_servient
*/
tinywot_status_t tinywot_servient_process(struct tinywot_servient const *self);

/*! \} */ /* grp_servient */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TINYWOT_H */
