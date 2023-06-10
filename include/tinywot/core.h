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
  \defgroup doc_group_status_codes Status Codes

  Function return values recognized and used across the library.

  Errors are always defined to be less than 0. All values greater or equal to 0
  are reserved for successful statuses. `tinywot_status_is_error()` and
  `tinywot_status_is_success()` are provided as two convenient functions for
  checking success or error status in general.

  \{
*/

/*!
  \brief The type of status codes recognized and returned by the library.
*/
typedef int_fast8_t tinywot_status_t;

/*!
  \brief A function is not implemented.

  This can happen when a target is registered to a `NULL` handler.
*/
#define TINYWOT_STATUS_ERROR_NOT_IMPLEMENTED   ((tinywot_status_t)-14)

/*!
  \brief There is insufficient memory to complete an action.
*/
#define TINYWOT_STATUS_ERROR_NOT_ENOUGH_MEMORY ((tinywot_status_t)-12)

/*!
  \brief Something is missing.

  This can happen when a requested target and operation type cannot match any
  handler registered in a Thing.
*/
#define TINYWOT_STATUS_ERROR_NOT_FOUND         ((tinywot_status_t)-2)

/*!
  \brief A requested operation is not allowed.

  This can happen when a target can be matched, but does not accept the
  operation type requested.
*/
#define TINYWOT_STATUS_ERROR_NOT_ALLOWED       ((tinywot_status_t)-1)

/*!
  \brief Success.

  This is not an error.
*/
#define TINYWOT_STATUS_SUCCESS                 ((tinywot_status_t)0)

/*!
  \brief Check if a status code indicates an error.

  \param[in] sc A status code returned by the library.
  \return `true` if `sc` is an error, `false` if it is a success.
*/
static inline bool tinywot_status_is_error(tinywot_status_t sc) {
  return sc < ((tinywot_status_t)0);
}

/*!
  \brief Check if a status code indicates a success.

  \param[in] sc A status code returned by the library.
  \return `true` if `sc` is an success, `false` if it is an error.
*/
static inline bool tinywot_status_is_success(tinywot_status_t sc) {
  return sc >= ((tinywot_status_t)0);
}

/*! \} */ /* doc_group_status_codes */

/*!
  \defgroup doc_group_op_types Operation Types

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
typedef uint_fast32_t tinywot_operation_type_t;

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

/*! \} */ /* doc_group_op_types */

/*!
  \defgroup doc_group_req_n_res Requests and Responses

  \{
*/

/*!
  \defgroup doc_group_res_status Response Status

  They are usually converted from \ref doc_group_status_codes returned by
  functions. Interestingly, they are defined as the positive variant of
  \ref doc_group_status_codes. Some \ref doc_group_status_codes do not have
  corresponding Response Statuses; they are mapped into
  `TINYWOT_RESPONSE_STATUS_INTERNAL_ERROR`.

  \{
*/

/*! \brief The data type of a response status. */
typedef uint_fast8_t tinywot_response_status_t;

/*! \brief The requested action has not been implemented. */
#define TINYWOT_RESPONSE_STATUS_NOT_SUPPORTED  ((tinywot_response_status_t)14)

/*!
  \brief Cannot finish a request due to an unspecified and unexpected error.
*/
#define TINYWOT_RESPONSE_STATUS_INTERNAL_ERROR ((tinywot_response_status_t)5)

/*! \brief Something could not be found. */
#define TINYWOT_RESPONSE_STATUS_NOT_FOUND      ((tinywot_response_status_t)2)

/*! \brief The requested action is not allowed on the target. */
#define TINYWOT_RESPONSE_STATUS_NOT_ALLOWED    ((tinywot_response_status_t)1)

/*! \brief All good. */
#define TINYWOT_RESPONSE_STATUS_OK             ((tinywot_response_status_t)0)

/*! \} */ /* doc_group_res_status */

/*!
  \brief A metadata structure about data.
*/
struct tinywot_payload {
  /*! \brief A pointer to the buffer holding the actual data. */
  void *content;

  /*! \brief The size of buffer pointed by `content` in byte. */
  size_t content_buffer_size_byte;

  /*! \brief The valid data length of `content`. */
  size_t content_length_byte;

  /*! \brief An indicator of the type of `content`. */
  uint_fast16_t content_type;
};

/*!
  \brief The size of buffer reserved for the `target` string in
  `tinywot_request`.
*/
#ifndef TINYWOT_REQUEST_TARGET_BUFFER_SIZE_BYTE
#define TINYWOT_REQUEST_TARGET_BUFFER_SIZE_BYTE 32
#endif

/*!
  \brief An incoming request.
*/
struct tinywot_request {
  /*! \brief The content carried with the request. */
  struct tinywot_payload payload;

  /*! \brief The intended operation type extracted from the request. */
  tinywot_operation_type_t operation_type;

  /*! \brief The intended submision target extracted from the request. */
  char target[TINYWOT_REQUEST_TARGET_BUFFER_SIZE_BYTE];
};

/*!
  \brief An outgoing response.
*/
struct tinywot_response {
  /*! \brief The content to send with the response. */
  struct tinywot_payload payload;

  /*!
    \brief A status code for the response.

    This is normally calculated from the \ref doc_group_status_codes returned by functions.
  */
  tinywot_response_status_t status;
};

/*! \} */ /* doc_group_req_n_res */

/*!
  \defgroup doc_group_impl Behavior Implementation (Forms and Things)

  \{
*/

/*!
  \brief The signature of a handler function implementing the behavior of a
  submission target.

  \param[out] response A pointer to a new `tinywot_response`. The handler
  should fill in this `tinywot_response` to return something.
  \param[in] request A pointer to an incoming `tinywot_request`.
  \param[inout] context The context information set in the corresponding
  `tinywot_form`.
  \return \ref doc_group_status_codes
*/
typedef tinywot_status_t tinywot_form_handler_t(
  struct tinywot_response *response,
  struct tinywot_request *request,
  void *context
);

/*!
  \brief An operation endpoint.
*/
struct tinywot_form {
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

    \ref doc_group_op_types can be bitwise-ORed here to allow multiple
    operation types:

    ```
    form.allowed_operation_types = TINYWOT_OPERATION_TYPE_READPROPERTY |
      TINYWOT_OPERATION_TYPE_WRITEPROPERTY;
    ```

    `TINYWOT_OPERATION_TYPE_ALL` can also be used to indicate that all
    operations are allowed on this form. See \ref doc_group_op_types for
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
  void *context;
};

/*!
  \internal
  \brief The base type of a Thing.

  The layout of this structure is made to be identical to `tinywot_thing_static`
  and `tinywot_thing_dynamic`. This allows them to be upcasted to this type, and
  thus implementing polymorphism (sort of) -- functions accept this base type to
  work on all derived types (classes).

  One should never use this type directly.

  \sa `tinywot_thing_static`, `tinywot_thing_dynamic`
*/
struct tinywot_thing_base {
  /*! \brief A list of forms describing behaviors of this Thing. */
  struct tinywot_form *forms;

  /*! \brief The number of registered forms in `forms`. */
  size_t forms_count_n;
};

/*!
  \brief A Thing that is statically configured.
*/
struct tinywot_thing_static {
  /*! \brief A list of forms describing behaviors of this Thing. */
  struct tinywot_form const *forms;

  /*! \brief The number of registered forms in `forms`. */
  size_t forms_count_n;
};

/*!
  \brief A Thing that supports runtime configuration.
*/
struct tinywot_thing_dynamic {
  /*! \brief A list of forms describing behaviors of this Thing. */
  struct tinywot_form *forms;

  /*! \brief The number of registered forms in `forms`. */
  size_t forms_count_n;

  /*! \brief The maximum number of forms in `forms`. */
  size_t forms_max_n;
};

/*!
  \internal
  \brief Find a [Form] from a [Base Thing].

  A [Form] is found when all the following apply:

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

  [Thing]: \ref tinywot_thing_base
  [Form]: \ref tinywot_form
*/
tinywot_status_t tinywot_thing_base_get_form(
  struct tinywot_thing_base const *self,
  char const *target,
  tinywot_operation_type_t operation_types,
  struct tinywot_form const **form
);

/*!
  \brief Find a [Form] from a [Static Thing].

  A [Form] is found when all the following apply:

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

  [Thing]: \ref tinywot_thing_static
  [Form]: \ref tinywot_form
*/
static inline tinywot_status_t tinywot_thing_static_get_form(
  struct tinywot_thing_static const *self,
  char const *target,
  tinywot_operation_type_t operation_types,
  struct tinywot_form const **form
) {
  return tinywot_thing_base_get_form(
    (struct tinywot_thing_base const *)self, target, operation_types, form
  );
}

/*!
  \brief Find a [Form] from a [Dynamic Thing].

  A [Form] is found when all the following apply:

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

  [Thing]: \ref tinywot_thing_dynamic
  [Form]: \ref tinywot_form
*/
static inline tinywot_status_t tinywot_thing_dynamic_get_form(
  struct tinywot_thing_dynamic const *self,
  char const *target,
  tinywot_operation_type_t operation_types,
  struct tinywot_form const **form
) {
  return tinywot_thing_base_get_form(
    (struct tinywot_thing_base const *)self, target, operation_types, form
  );
}

/*!
  \brief Set a [Form] to a [Dynamic Thing].

  This copies the content of `form` into the [Dynamic Thing]. The caller do not
  have to allocate a heap space for `form`, but any memory pointed by pointers
  in a [Form] should be in the heap.

  Before attempting to insert a new [Form] into the [Thing], this function tries
  to use `form->target` and `form->allowed_operation_types` to find an existing
  slot in `self`. If one is found, then the slot will be overwritten with the
  supplied `form`. An existing [Form] is found when all the following apply:

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

  [Thing]: \ref tinywot_thing_dynamic
  [Form]: \ref tinywot_form
*/
tinywot_status_t tinywot_thing_dynamic_set_form(
  struct tinywot_thing_dynamic *self,
  struct tinywot_form const *form
);

/*!
  \internal
  \brief Process a [Request], transforming it into a [Response] with a [Thing].

  \param[in] self A [Thing].
  \param[inout] request A [Request] to be processed by the registered handler
  in `self`.
  \param[inout] response An empty [Response] to be filled in.
  \return \ref doc_group_status_codes

  [Request]: \ref tinywot_request
  [Response]: \ref tinywot_response
  [Thing]: \ref tinywot_thing
*/
tinywot_status_t tinywot_thing_base_process_request(
  struct tinywot_thing_base const *self,
  struct tinywot_request *request,
  struct tinywot_response *response
);

/*!
  \brief Process a [Request], transforming it into a [Response] with a [Thing].

  \param[in] self A [Thing].
  \param[inout] request A [Request] to be processed by the registered handler
  in `self`.
  \param[inout] response An empty [Response] to be filled in.
  \return \ref doc_group_status_codes

  [Request]: \ref tinywot_request
  [Response]: \ref tinywot_response
  [Thing]: \ref tinywot_thing
*/
static inline tinywot_status_t tinywot_thing_static_process_request(
  struct tinywot_thing_static const *self,
  struct tinywot_request *request,
  struct tinywot_response *response
) {
  return tinywot_thing_base_process_request(
    (struct tinywot_thing_base const *)self, request, response
  );
}

/*!
  \brief Process a [Request], transforming it into a [Response] with a [Thing].

  \param[in] self A [Thing].
  \param[inout] request A [Request] to be processed by the registered handler
  in `self`.
  \param[inout] response An empty [Response] to be filled in.
  \return \ref doc_group_status_codes

  [Request]: \ref tinywot_request
  [Response]: \ref tinywot_response
  [Thing]: \ref tinywot_thing
*/
static inline tinywot_status_t tinywot_thing_dynamic_process_request(
  struct tinywot_thing_dynamic const *self,
  struct tinywot_request *request,
  struct tinywot_response *response
) {
  return tinywot_thing_base_process_request(
    (struct tinywot_thing_base const *)self, request, response
  );
}

/*! \} */ /* doc_group_impl */

/*!
  \defgroup doc_group_proto Protocol Stack Interfaces

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
  \param[inout] context The data structure set in the corresponding
  `tinywot_protocol`.
  \return \ref doc_group_status_codes

  [Request]: \ref tinywot_request
*/
typedef tinywot_status_t tinywot_protocol_receive_function_t(
  struct tinywot_request *request, void *context
);

/*!
  \brief A function that writes with [I/O functions] and a [Response].

  \param[inout] response A pointer to a valid [Response].
  \param[inout] context The data structure set in the corresponding
  `tinywot_protocol`.
  \return \ref doc_group_status_codes

  [Response]: \ref tinywot_response
*/
typedef tinywot_status_t tinywot_protocol_send_function_t(
  struct tinywot_response *response, void *context
);

/*!
  \brief A receive function and a send function.
*/
struct tinywot_protocol {
  /*!
    \brief A function to read in and receive a [Request].

    [Request]: \ref tinywot_request
  */
  tinywot_protocol_receive_function_t *recv;

  /*!
    \brief A function to send and write a [Response] out.

    [Response]: \ref tinywot_response
  */
  tinywot_protocol_send_function_t *send;

  /*!
    \brief A pointer to any data structure for the convenience of `recv()` and
    `send()`.
  */
  void *context;
};

/*! \} */ /* doc_group_proto */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TINYWOT_H */
