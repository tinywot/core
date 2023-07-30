/*
  SPDX-FileCopyrightText: 2021-2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \file
  \brief TinyWoT Core API definition.

  The Core part of TinyWoT is only a small portion of data structure
  definitions and helper functions. Other platform-dependent components
  of TinyWoT must be provided to complete a runtime platform:

  - Protocol Binding
  - Servient

  They can be found under the [TinyWoT GitHub organization][org].

  [org]: https://github.com/tinywot
*/

#ifndef TINYWOT_CORE_H
#define TINYWOT_CORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
  \defgroup status_codes Status Codes

  Function return values recognized and used across the library.

  @{
*/

/*!
  \brief The data type of status / return codes.

  Errors are always defined to be less than 0. All values greater or
  equal to 0 are reserved for successful statuses.

  This could be an `enum`, however we try to use only as smaller as
  possible data type to optimize for 8-bit platforms (namely AVR8,
  ATmega, Arduino UNO R3). The `-fshort-enums` flag in avr-gcc helps
  with this, but it requires user intervention.
*/
typedef int_fast8_t tinywot_status_t;

/*!
  \brief The function is not implemented.

  This can happen when a target is registered to a `NULL` handler, or
  the handler is just a stub.
*/
#define TINYWOT_STATUS_ERROR_NOT_IMPLEMENTED   ((tinywot_status_t)-14)

/*!
  \brief There is insufficient memory to complete an action.
*/
#define TINYWOT_STATUS_ERROR_NOT_ENOUGH_MEMORY ((tinywot_status_t)-12)

/*!
  \brief Something is missing.

  This can happen when a requested target and operation type cannot
  match any handler registered in a Thing.
*/
#define TINYWOT_STATUS_ERROR_NOT_FOUND         ((tinywot_status_t)-2)

/*!
  \brief The requested operation is not allowed.

  This can happen when a target can be matched, but does not accept the
  operation type requested.
*/
#define TINYWOT_STATUS_ERROR_NOT_ALLOWED       ((tinywot_status_t)-1)

/*!
  \brief The operation is successful.

  This is not an error.
*/
#define TINYWOT_STATUS_SUCCESS                 ((tinywot_status_t)0)

/*!
  \brief Check if a status code indicates an error.

  Any `tinywot_status_t` less than `0` is an error. This is just a
  conveninet macro (implemented as an inline function) for checking
  this.

  \param[in] sc A status code returned by the library.
  \return `true` if `sc` is an error, `false` if it is a success.
*/
static inline bool tinywot_status_is_error(tinywot_status_t sc) {
  return sc < ((tinywot_status_t)0);
}

/*!
  \brief Check if a status code indicates a success.

  Any `tinywot_status_t` greater than or equal to `0` is an success.
  This is just a conveninet macro (implemented as an inline function)
  for checking this.

  \param[in] sc A status code returned by the library.
  \return `true` if `sc` is an success, `false` if it is an error.
*/
static inline bool tinywot_status_is_success(tinywot_status_t sc) {
  return sc >= ((tinywot_status_t)0);
}

/*! @} */ /* status_codes */

/*!
  \defgroup operation_type Operation Type

  An operation type is a "semantic intention" of a request, according to
  the [Thing Description][td] specification. It is a protocol-neutral
  way to specify an action of a request. For example,
  `TINYWOT_OPERATION_TYPE_READPROPERTY` corresponds to HTTP `GET`.

  Operation types can be combined together with bitwise OR (`|`). For
  example, to specify a read-write property, write:

  \code{.c}
  tinywot_operation_type_t rw = TINYWOT_OPERATION_TYPE_READPROPERTY | TINYWOT_OPERATION_TYPE_WRITEPROPERTY;
  \endcode

  A full list of operation types can be found at [&sect; 5.3.4.2 Form,
  the Web of Things (WoT) Thing Description][td-5.3.4.2]. So far,
  TinyWoT only supports a few of them.

  In addition, TinyWoT defines two special operation types:

  - `::TINYWOT_OPERATION_TYPE_UNKNOWN` indicates an invalid operation
    type variable. This can be used to initialize an (`auto`) operation
    type variable to a known state.
  - `::TINYWOT_OPERATION_TYPE_ALL` indicates all operation types (by
    masking all bits). This can be used in a [Form] to create a target
    that accepts all operation types.

  [Form]: \ref tinywot_form
  [td-5.3.4.2]: https://www.w3.org/TR/2020/REC-wot-thing-description-20200409/#form

  @{
*/

/*!
  \brief The data type of operation types.
*/
typedef uint_fast32_t tinywot_operation_type_t;

/*!
  \brief An unknown or uninitialized opeartion type.
*/
#define TINYWOT_OPERATION_TYPE_UNKNOWN \
  ((tinywot_operation_type_t)0)

/*!
  \brief "Identifies the read operation on Property Affordances to
  retrieve the corresponding data."
*/
#define TINYWOT_OPERATION_TYPE_READPROPERTY \
  ((tinywot_operation_type_t)1)

/*!
  \brief "Identifies the write operation on Property Affordances to
  update the corresponding data."
*/
#define TINYWOT_OPERATION_TYPE_WRITEPROPERTY \
  (((tinywot_operation_type_t)1) << 1)

/*!
  \brief "Identifies the observe operation on Property Affordances to be
  notified with the new data when the Property is updated."
*/
#define TINYWOT_OPERATION_TYPE_OBSERVEPROPERTY \
  (((tinywot_operation_type_t)1) << 2)

/*!
  \brief "Identifies the unobserve operation on Property Affordances to
  stop the corresponding notifications."
*/
#define TINYWOT_OPERATION_TYPE_UNOBSERVEPROPERTY \
  (((tinywot_operation_type_t)1) << 3)

/*!
  \brief "Identifies the invoke operation on Action Affordances to
  perform the corresponding action."
*/
#define TINYWOT_OPERATION_TYPE_INVOKEACTION \
  (((tinywot_operation_type_t)1) << 4)

/*!
  \brief "Identifies the querying operation on Action Affordances to get
  the status of the corresponding action."
*/
#define TINYWOT_OPERATION_TYPE_QUERYACTION \
  (((tinywot_operation_type_t)1) << 5)

/*!
  \brief "Identifies the cancel operation on Action Affordances to
  cancel the ongoing corresponding action."
*/
#define TINYWOT_OPERATION_TYPE_CANCELACTION \
  (((tinywot_operation_type_t)1) << 6)

/*!
  \brief "Identifies the subscribe operation on Event Affordances to be
  notified by the Thing when the event occurs."
*/
#define TINYWOT_OPERATION_TYPE_SUBSCRIBEEVENT \
  (((tinywot_operation_type_t)1) << 7)

/*!
  \brief "Identifies the unsubscribe operation on Event Affordances to
  stop the corresponding notifications."
*/
#define TINYWOT_OPERATION_TYPE_UNSUBSCRIBEEVENT \
  (((tinywot_operation_type_t)1) << 8)

/*!
  \brief "Identifies the readallproperties operation on a Thing to
  retrieve the data of all Properties in a single interaction."
*/
#define TINYWOT_OPERATION_TYPE_READALLPROPERTIES \
  (((tinywot_operation_type_t)1) << 9)

/*!
  \brief "Identifies the writeallproperties operation on a Thing to
  update the data of all writable Properties in a single interaction."
*/
#define TINYWOT_OPERATION_TYPE_WRITEALLPROPERTIES \
  (((tinywot_operation_type_t)1) << 10)

/*!
  \brief "Identifies the readmultipleproperties operation on a Thing to
  retrieve the data of selected Properties in a single interaction."
*/
#define TINYWOT_OPERATION_TYPE_READMULTIPLEPROPERTIES \
  (((tinywot_operation_type_t)1) << 11)

/*!
  \brief "Identifies the writemultipleproperties operation on a Thing to
  update the data of selected writable Properties in a single
  interaction."
*/
#define TINYWOT_OPERATION_TYPE_WRITEMULTIPLEPROPERTIES \
  (((tinywot_operation_type_t)1) << 12)

/*!
  \brief "Identifies the observeallproperties operation on Properties to
  be notified with new data when any Property is updated."
*/
#define TINYWOT_OPERATION_TYPE_OBSERVEALLPROPERTIES \
  (((tinywot_operation_type_t)1) << 13)

/*!
  \brief "Identifies the unobserveallproperties operation on Properties
  to stop notifications from all Properties in a single interaction."
*/
#define TINYWOT_OPERATION_TYPE_UNOBSERVEALLPROPERTIES \
  (((tinywot_operation_type_t)1) << 14)

/*!
  \brief "Identifies the queryallactions operation on a Thing to get the
  status of all Actions in a single interaction."
*/
#define TINYWOT_OPERATION_TYPE_QUERYALLACTIONS \
  (((tinywot_operation_type_t)1) << 15)

/*!
  \brief "Identifies the subscribeallevents operation on Events to
  subscribe to notifications from all Events in a single interaction."
*/
#define TINYWOT_OPERATION_TYPE_SUBSCRIBEALLEVENTS \
  (((tinywot_operation_type_t)1) << 16)

/*!
  \brief "Identifies the unsubscribeallevents operation on Events to
  unsubscribe from notifications from all Events in a single
  interaction."
*/
#define TINYWOT_OPERATION_TYPE_UNSUBSCRIBEALLEVENTS \
  (((tinywot_operation_type_t)1) << 17)

/*!
  \brief All of the operation types defined in the library.
*/
#define TINYWOT_OPERATION_TYPE_ALL \
  (~((tinywot_operation_type_t)0))

/*! @} */ /* operation_type */

/*!
  \defgroup request_and_response Request and Response

  @{

  \defgroup response_status Response Status

  This is a limited set of \ref status_codes that can be further mapped
  into a protocol-specific status. For example,
  `::TINYWOT_STATUS_ERROR_NOT_ENOUGH_MEMORY` should be returned as a
  `::TINYWOT_RESPONSE_STATUS_INTERNAL_ERROR`. See the helper function
  `tinywot_response_status_from_status()`.

  @{
*/

/*!
  \brief The data type of a response status.

  Same as `tinywot_status_t`, this is for optimization of 8-bit
  architectures.
*/
typedef uint_fast8_t tinywot_response_status_t;

/*!
  \brief The requested action has not been implemented.
*/
#define TINYWOT_RESPONSE_STATUS_NOT_SUPPORTED ((tinywot_response_status_t)14)

/*!
  \brief Cannot finish a request due to an unspecified and unexpected
  error.
*/
#define TINYWOT_RESPONSE_STATUS_INTERNAL_ERROR ((tinywot_response_status_t)5)

/*!
  \brief Something could not be found.
*/
#define TINYWOT_RESPONSE_STATUS_NOT_FOUND ((tinywot_response_status_t)2)

/*!
  \brief The requested action is not allowed on the target.
*/
#define TINYWOT_RESPONSE_STATUS_NOT_ALLOWED ((tinywot_response_status_t)1)

/*!
  \brief All good.
*/
#define TINYWOT_RESPONSE_STATUS_OK ((tinywot_response_status_t)0)

/*!
  \brief Convert from a `tinywot_status_t` to a
  `tinywot_response_status_t`.

  \param[in] status A `tinywot_status_t`.
  \return A `tinywot_response_status_t`.
*/
tinywot_response_status_t tinywot_response_status_from_status(
  tinywot_status_t const status
);

/*! @} */ /* response_status */

/*!
  \defgroup payload Payload

  This just provides a way to group the memory pointer and a few
  metadata fields about the data in the pointed memory.

  @{
*/

/*!
  \brief A metadata structure about data.
*/
struct tinywot_payload {
  /*!
    \brief A pointer to the buffer holding the actual data.
    \public
  */
  void *content;

  /*!
    \brief The size of buffer pointed by `content` in byte.
    \public
  */
  size_t content_buffer_size_byte;

  /*!
    \brief The valid data length of `content`.
    \public
  */
  size_t content_length_byte;

  /*!
    \brief An indicator of the type of `content`.
    \public
  */
  uint_fast16_t content_type;
};

/*!
  \brief Append the memory content pointed by `data` to a
  `tinywot_payload`.
  \memberof tinywot_payload
  \public

  This copies the memory pointed by `data` to the end of
  `tinywot_payload::content`.

  \param[in] self An instance of `tinywot_payload`.
  \param[in] data A pointer to a memory region containing data.
  \param[in] data_size_byte How long is the data, in bytes.
  \return \ref status_codes
*/
tinywot_status_t tinywot_payload_append(
  struct tinywot_payload *self,
  void const *data,
  size_t data_size_byte
);

/*! @} */ /* payload */

/*!
  \defgroup request Request

  @{
*/

/*!
  \brief The size of buffer reserved for `tinywot_request::target`.
*/
#ifndef TINYWOT_REQUEST_TARGET_BUFFER_SIZE_BYTE
#define TINYWOT_REQUEST_TARGET_BUFFER_SIZE_BYTE 32
#endif

/*!
  \brief An incoming request.
  \extends tinywot_payload
*/
struct tinywot_request {
  /*!
    \brief The content carried with the request.
    \public
  */
  struct tinywot_payload payload;

  /*!
    \brief The intended operation type extracted from the request.
    \public
  */
  tinywot_operation_type_t operation_type;

  /*!
    \brief The intended submision target extracted from the request.
    \public
  */
  char target[TINYWOT_REQUEST_TARGET_BUFFER_SIZE_BYTE];
};

/*! @} */ /* request */

/*!
  \defgroup response Response

  @{
*/

/*!
  \brief An outgoing response.
  \extends tinywot_payload
*/
struct tinywot_response {
  /*!
    \brief The content to send with the response.
    \public
  */
  struct tinywot_payload payload;

  /*!
    \brief A status code for the response.
    \public

    This is normally calculated from the \ref status_codes returned by
    functions.
  */
  tinywot_response_status_t status;
};

/*! @} */ /* response */
/*! @} */ /* request_and_response */

/*!
  \defgroup form_and_thing Behavior Implementation (Form and Thing)

  @{

  \defgroup form Form

  @{
*/

/*!
  \brief The signature of a handler function implementing a form (the
  behavior of a submission target).

  TinyWoT ensures that `response` and `request` are not `NULL`. Should
  the implementation wants to return something, `response` should be
  filled in.

  \param[out] response A pointer to a new `tinywot_response`.
  \param[in] request A pointer to an incoming `tinywot_request`.
  \param[inout] context The context information set in the corresponding
  `tinywot_form`.
  \return \ref status_codes
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
    \public

    A [Thing] will compare this field with the `target` field in a
    [Response] to figure out a registered form.

    The exact way to specify a submission target is undefined. This
    largely depends on the protocol binding implementation, who is
    responsible for extracting this information from the request
    message. Often, for an IRI specified in a Thing Description
    document, this should be the path name of it. For example, for the
    following IRI:

        http://thing.example.com/action/update

    Its path name is `/action/update`, so this field should be
    `/action/update`.

    [Thing]: \ref static_thing
    [Response]: \ref tinywot_response
  */
  char const *target;

  /*!
    \brief The allowed operation types on this form.
    \public

    Multiple operation types can be bitwise-ORed together to specify
    multiple allowed operation types. `::TINYWOT_OPERATION_TYPE_ALL` can
    also be used to indicate that all operations are allowed on this
    form.

    See \ref operation_type for more details about opeartion types.
  */
  tinywot_operation_type_t allowed_operation_types;

  /*!
    \brief A function pointer to the actual implementation of the form.
    \public
  */
  tinywot_form_handler_t *handler;

  /*!
    \brief Arbitrary data to pass to `handler` when it is called.
    \public
  */
  void *context;
};

/*! @} */ /* form */

/*!
  \defgroup base_thing Base Thing

  The [Base Thing] is extended by the [Static Thing] and the
  [Dynamic Thing] to implement different behaviors of different Thing
  configurations in a typed and object-oriented way.

  \attention You should never use this type directly. Use either
  `tinywot_thing_static` or `tinywot_thing_dynamic`.

  [Base Thing]: \ref tinywot_thing_base
  [Static Thing]: \ref tinywot_thing_static
  [Dynamic Thing]: \ref tinywot_thing_dynamic

  @{
*/

/*!
  \brief The base type of a Thing.
  \attention You should never use this type directly.
  \sa `tinywot_thing_static`, `tinywot_thing_dynamic`
*/
struct tinywot_thing_base {
  /*!
    \brief A list of forms describing behaviors of this Thing.
    \protected
  */
  struct tinywot_form *forms;

  /*!
    \brief The number of registered forms in `forms`.
    \protected
  */
  size_t forms_count_n;
};

/*! @} */ /* base_thing */

/*!
  \defgroup static_thing Static Thing

  A [Static Thing] is backed by a statically defined series of [Form]s.
  Most often, it stays in the data (ROM) memory region, so it is read-
  only. Methods implemented on [Static Thing] do not alter the object.
  To configure the Thing during run-time, use a [Dynamic Thing] instead.

  [Static Thing]: \ref tinywot_thing_static
  [Form]: \ref tinywot_form
  [Dynamic Thing]: \ref dynamic_thing

  @{
*/

/*!
  \brief A Thing that is statically configured.
  \extends tinywot_thing_base
*/
struct tinywot_thing_static {
  /*! \private */
  struct tinywot_thing_base const base;
};

/*!
  \brief Create a new `tinywot_thing_static`.

  This is a convenient macro to statically initialize a
  `tinywot_thing_static`. Use it by assigning it to a
  `tinywot_thing_static`:

  \code{.c}
  static const struct tinywot_thing_static thing = TINYWOT_THING_STATIC_NEW(forms, 6);
  \endcode

  The macro is then replaced with structure initialization code during
  compile-time.

  \param[in] forms `struct tinywot_form const *`: A pointer to a
  `tinywot_form` array.
  \param[in] forms_count_n `size_t`: The number of `tinywot_form`s in
  the memory pointed by `forms`.
*/
#define TINYWOT_THING_STATIC_NEW(forms, forms_count_n) \
  { { (forms), (forms_count_n) } }

/*!
  \brief Get the `forms` member of `tinywot_thing_static`.
  \memberof tinywot_thing_static
  \public

  \param[in] self A `tinywot_thing_static` instance.
  \return `tinywot_thing_static::forms`.
*/
struct tinywot_form const *tinywot_thing_static_get_forms(
  struct tinywot_thing_static const *self
);

/*!
  \brief Get the `forms_count_n` member of `tinywot_thing_static`.
  \memberof tinywot_thing_static
  \public

  \param[in] self A `tinywot_thing_static` instance.
  \return `tinywot_thing_static::forms_count_n`.
*/
size_t tinywot_thing_static_get_forms_count(
  struct tinywot_thing_static const *self
);

/*!
  \brief Find a `tinywot_form` in a `tinywot_thing_static`.
  \memberof tinywot_thing_static
  \public

  \param[in]  self A `tinywot_thing_static` instance.
  \param[out] form For returning a pointer pointing to the found
  `tinywot_form`.
  \param[in]  target The submission target string to find.
  \param[in]  operation_types The allowed operation types to match.
  \return \ref status_codes
*/
tinywot_status_t tinywot_thing_static_find_form(
  struct tinywot_thing_static const *self,
  struct tinywot_form const **form,
  char const *target,
  tinywot_operation_type_t operation_types
);

/*!
  \brief Convert a `tinywot_request` to a `tinywot_response` with the
  help of a `tinywot_thing_static`.
  \memberof tinywot_thing_static
  \public

  \param[in]  self A `tinywot_thing_static` instance.
  \param[out] response An instance of `tinywot_response`, the fields of
  which would be finished by the found handler function.
  \param[in]  request An incoming `tinywot_request`.
  \return \ref status_codes
*/
tinywot_status_t tinywot_thing_static_process_request(
  struct tinywot_thing_static const *self,
  struct tinywot_response *response,
  struct tinywot_request *request
);

/*! @} */ /* static_thing */

/*!
  \defgroup dynamic_thing Dynamic Thing

  @{
*/

/*!
  \brief A Thing that supports runtime configuration.
  \extends tinywot_thing_base
*/
struct tinywot_thing_dynamic {
  /*! \private */
  struct tinywot_thing_base base;
  /*!
    \brief The maximum number of `tinywot_form`s that the supporting
    memory can contain.
    \private
  */
  size_t forms_max_n;
};

/*!
  \brief Get the `forms` member of `tinywot_thing_dynamic`.
  \memberof tinywot_thing_dynamic
  \public

  \param[in] self A `tinywot_thing_dynamic` instance.
  \return `tinywot_thing_dynamic::forms`.
*/
struct tinywot_form const *tinywot_thing_dynamic_get_forms(
  struct tinywot_thing_dynamic const *self
);

/*!
  \brief Get the `forms_count_n` member of `tinywot_thing_dynamic`.
  \memberof tinywot_thing_dynamic
  \public

  \param[in] self A `tinywot_thing_dynamic` instance.
  \return `tinywot_thing_dynamic::forms_count_n`.
*/
size_t tinywot_thing_dynamic_get_forms_count(
  struct tinywot_thing_dynamic const *self
);

/*!
  \brief Get the `forms_max_n` member of `tinywot_thing_dynamic`.
  \memberof tinywot_thing_dynamic
  \public

  \param[in] self A `tinywot_thing_dynamic` instance.
  \return `tinywot_thing_dynamic::forms_max_n`.
*/
size_t tinywot_thing_dynamic_get_forms_max(
  struct tinywot_thing_dynamic const *self
);

/*!
  \brief Set a memory buffer backing the forms array of a
  `tinywot_thing_dynamic` instance.
  \memberof tinywot_thing_dynamic
  \public

  \param[in] self A `tinywot_thing_dynamic` instance.
  \param[in] buffer A pointer to the backing memory.
  \param[in] buffer_size_byte The size of `buffer` in bytes.
*/
void tinywot_thing_dynamic_set_forms_buffer(
  struct tinywot_thing_dynamic *self,
  void *buffer,
  size_t buffer_size_byte
);

/*!
  \brief Find a `tinywot_form` in a `tinywot_thing_dynamic`.
  \memberof tinywot_thing_dynamic
  \public

  \param[in]  self A `tinywot_thing_dynamic` instance.
  \param[out] form For returning a pointer pointing to the found
  `tinywot_form`.
  \param[in]  target The submission target string to find.
  \param[in]  operation_types The allowed operation types to match.
  \return \ref status_codes
*/
tinywot_status_t tinywot_thing_dynamic_find_form(
  struct tinywot_thing_dynamic const *self,
  struct tinywot_form const **form,
  char const *target,
  tinywot_operation_type_t operation_types
);

/*!
  \brief Add a `tinywot_form` to a `tinywot_thing_dynamic`.
  \memberof tinywot_thing_dynamic
  \public

  \param[in] self A `tinywot_thing_dynamic` instance.
  \param[in] form A pointer to a `tinywot_form`.
  \return \ref status_codes
*/
tinywot_status_t tinywot_thing_dynamic_add_form(
  struct tinywot_thing_dynamic *self, struct tinywot_form const *form
);

/*!
  \brief Find and replace a `tinywot_form` in a `tinywot_thing_dynamic`.
  \memberof tinywot_thing_dynamic
  \public

  \param[in] self A `tinywot_thing_dynamic` instance.
  \param[in] target The submission target string to find.
  \param[in] operation_types The allowed operation types to match.
  \param[in] form A pointer to a `tinywot_form`.
  \return \ref status_codes
*/
tinywot_status_t tinywot_thing_dynamic_change_form(
  struct tinywot_thing_dynamic *self,
  char const *target,
  tinywot_operation_type_t operation_types,
  struct tinywot_form const *form
);

/*!
  \brief Find and delete a `tinywot_form` in a `tinywot_thing_dynamic`.
  \memberof tinywot_thing_dynamic
  \public

  \param[in] self A `tinywot_thing_dynamic` instance.
  \param[in] target The submission target string to find.
  \param[in] operation_types The allowed operation types to match.
  \return \ref status_codes
*/
tinywot_status_t tinywot_thing_dynamic_remove_form(
  struct tinywot_thing_dynamic *self,
  char const *target,
  tinywot_operation_type_t allowed_operation_types
);

/*!
  \brief Convert a `tinywot_request` to a `tinywot_response` with the
  help of a `tinywot_thing_dynamic`.
  \memberof tinywot_thing_dynamic
  \public

  \param[in]  self A `tinywot_thing_dynamic` instance.
  \param[out] response An instance of `tinywot_response`, the fields of
  which would be finished by the found handler function.
  \param[in]  request An incoming `tinywot_request`.
  \return \ref status_codes
*/
tinywot_status_t tinywot_thing_dynamic_process_request(
  struct tinywot_thing_dynamic const *self,
  struct tinywot_response *response,
  struct tinywot_request *request
);

/*! @} */ /* dynamic_thing */
/*! @} */ /* form_and_thing */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TINYWOT_CORE_H */
