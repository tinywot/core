/*
  SPDX-FileCopyrightText: 2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

/*!
  \file
  \brief Unit tests for `tinywot_thing_process_request()`.
*/

#include <stddef.h>
#include <string.h>
#include <tinywot/core.h>
#include <tinywot-test.h>
#include <unity.h>

static char const target_status_str[] = "/status";
static char const target_lorem_str[] = "/lorem";
static char const target_overheating_str[] = "/oh";

void tinywot_process_request_should_succeed(void) {
  enum tinywot_status status = TINYWOT_STATUS_ERROR_GENERIC;
  struct tinywot_thing *thing = tinywot_test_thing_new_example();

  struct tinywot_request request = {0};
  struct tinywot_response response = {0};

  /* Memory backing payload.content in the request and the response. */
  unsigned char *content_in = tinywot_test_mallocd(
    TINYWOT_TEST_MEMORY_SIZE_BYTE
  );
  unsigned char *content_out = tinywot_test_mallocd(
    TINYWOT_TEST_MEMORY_SIZE_BYTE
  );

  /* Pretend that we have an incoming request. */
  request.payload.content = content_in;
  request.payload.content_buffer_size_byte = TINYWOT_TEST_MEMORY_SIZE_BYTE;
  request.op = TINYWOT_OPERATION_TYPE_READPROPERTY;
  memcpy(request.target, target_status_str, sizeof(target_status_str));

  /* Prepare the response payload content memory. */
  response.payload.content = content_out;
  response.payload.content_buffer_size_byte = TINYWOT_TEST_MEMORY_SIZE_BYTE;

  status = tinywot_thing_process_request(thing, &response, &request);

  TEST_ASSERT_EQUAL(TINYWOT_STATUS_SUCCESS, status);
  TEST_ASSERT_EQUAL(TINYWOT_RESPONSE_STATUS_OK, response.status);
  TEST_ASSERT_EQUAL_STRING("false", response.payload.content); /* should be */
  TEST_ASSERT_EQUAL_UINT(
    TINYWOT_TEST_MEMORY_SIZE_BYTE, response.payload.content_buffer_size_byte
  );
  TEST_ASSERT_EQUAL_UINT(6, response.payload.content_length_byte);
  TEST_ASSERT_EQUAL_UINT(50, response.payload.content_type);

  tinywot_test_free(content_out);
  tinywot_test_free(content_in);
  tinywot_test_thing_delete(thing);
}

void tinywot_process_request_should_succeed_when_op_mismatch(void) {
  enum tinywot_status status = TINYWOT_STATUS_ERROR_GENERIC;
  struct tinywot_thing *thing = tinywot_test_thing_new_example();

  struct tinywot_request request = {0};
  struct tinywot_response response = {0};

  /* Memory backing payload.content in the request and the response. */
  unsigned char *content_in = tinywot_test_mallocd(
    TINYWOT_TEST_MEMORY_SIZE_BYTE
  );
  unsigned char *content_out = tinywot_test_mallocd(
    TINYWOT_TEST_MEMORY_SIZE_BYTE
  );

  /* Pretend that we have an incoming request, but op mismatch. */
  request.payload.content = content_in;
  request.payload.content_buffer_size_byte = TINYWOT_TEST_MEMORY_SIZE_BYTE;
  request.op = TINYWOT_OPERATION_TYPE_INVOKEACTION;
  memcpy(request.target, target_status_str, sizeof(target_status_str));

  /* Prepare the response payload content memory. */
  response.payload.content = content_out;
  response.payload.content_buffer_size_byte = TINYWOT_TEST_MEMORY_SIZE_BYTE;

  status = tinywot_thing_process_request(thing, &response, &request);

  /* `tinywot_thing_process_request()` always return
     `TINYWOT_STATUS_SUCCESS`. Any error has been mapped to a response
     status stored in `response.status`. */
  TEST_ASSERT_EQUAL(TINYWOT_STATUS_SUCCESS, status);
  TEST_ASSERT_EQUAL(TINYWOT_RESPONSE_STATUS_NOT_ALLOWED, response.status);
  /* Don't care about response.payload.content. */
  TEST_ASSERT_EQUAL_UINT(
    TINYWOT_TEST_MEMORY_SIZE_BYTE, response.payload.content_buffer_size_byte
  );
  TEST_ASSERT_EQUAL_UINT(0, response.payload.content_length_byte);
  TEST_ASSERT_EQUAL_UINT(0, response.payload.content_type);

  tinywot_test_free(content_out);
  tinywot_test_free(content_in);
  tinywot_test_thing_delete(thing);
}

void tinywot_process_request_should_succeed_when_not_found(void) {
  enum tinywot_status status = TINYWOT_STATUS_ERROR_GENERIC;
  struct tinywot_thing *thing = tinywot_test_thing_new_example();

  struct tinywot_request request = {0};
  struct tinywot_response response = {0};

  /* Memory backing payload.content in the request and the response. */
  unsigned char *content_in = tinywot_test_mallocd(
    TINYWOT_TEST_MEMORY_SIZE_BYTE
  );
  unsigned char *content_out = tinywot_test_mallocd(
    TINYWOT_TEST_MEMORY_SIZE_BYTE
  );

  /* Pretend that we have an incoming request with a non-existent
     target. */
  request.payload.content = content_in;
  request.payload.content_buffer_size_byte = TINYWOT_TEST_MEMORY_SIZE_BYTE;
  request.op = TINYWOT_OPERATION_TYPE_READPROPERTY;
  memcpy(request.target, target_lorem_str, sizeof(target_lorem_str));

  /* Prepare the response payload content memory. */
  response.payload.content = content_out;
  response.payload.content_buffer_size_byte = TINYWOT_TEST_MEMORY_SIZE_BYTE;

  status = tinywot_thing_process_request(thing, &response, &request);

  /* `tinywot_thing_process_request()` always return
     `TINYWOT_STATUS_SUCCESS`. Any error has been mapped to a response
     status stored in `response.status`. */
  TEST_ASSERT_EQUAL(TINYWOT_STATUS_SUCCESS, status);
  TEST_ASSERT_EQUAL(TINYWOT_RESPONSE_STATUS_NOT_FOUND, response.status);
  /* Don't care about response.payload.content. */
  TEST_ASSERT_EQUAL_UINT(
    TINYWOT_TEST_MEMORY_SIZE_BYTE, response.payload.content_buffer_size_byte
  );
  TEST_ASSERT_EQUAL_UINT(0, response.payload.content_length_byte);
  TEST_ASSERT_EQUAL_UINT(0, response.payload.content_type);

  tinywot_test_free(content_out);
  tinywot_test_free(content_in);
  tinywot_test_thing_delete(thing);
}

void tinywot_process_request_should_succeed_when_not_implemented(void) {
  enum tinywot_status status = TINYWOT_STATUS_ERROR_GENERIC;
  struct tinywot_thing *thing = tinywot_test_thing_new_example();

  struct tinywot_request request = {0};
  struct tinywot_response response = {0};

  /* Memory backing payload.content in the request and the response. */
  unsigned char *content_in = tinywot_test_mallocd(
    TINYWOT_TEST_MEMORY_SIZE_BYTE
  );
  unsigned char *content_out = tinywot_test_mallocd(
    TINYWOT_TEST_MEMORY_SIZE_BYTE
  );

  /* Pretend that we have an incoming request to a form with no
     handler. */
  request.payload.content = content_in;
  request.payload.content_buffer_size_byte = TINYWOT_TEST_MEMORY_SIZE_BYTE;
  request.op = TINYWOT_OPERATION_TYPE_SUBSCRIBEEVENT;
  memcpy(
    request.target, target_overheating_str, sizeof(target_overheating_str)
  );

  /* Prepare the response payload content memory. */
  response.payload.content = content_out;
  response.payload.content_buffer_size_byte = TINYWOT_TEST_MEMORY_SIZE_BYTE;

  status = tinywot_thing_process_request(thing, &response, &request);

  /* `tinywot_thing_process_request()` always return
     `TINYWOT_STATUS_SUCCESS`. Any error has been mapped to a response
     status stored in `response.status`. */
  TEST_ASSERT_EQUAL(TINYWOT_STATUS_SUCCESS, status);
  TEST_ASSERT_EQUAL(TINYWOT_RESPONSE_STATUS_NOT_SUPPORTED, response.status);
  /* Don't care about response.payload.content. */
  TEST_ASSERT_EQUAL_UINT(
    TINYWOT_TEST_MEMORY_SIZE_BYTE, response.payload.content_buffer_size_byte
  );
  TEST_ASSERT_EQUAL_UINT(0, response.payload.content_length_byte);
  TEST_ASSERT_EQUAL_UINT(0, response.payload.content_type);

  tinywot_test_free(content_out);
  tinywot_test_free(content_in);
  tinywot_test_thing_delete(thing);
}

void setUp(void) {}
void tearDown(void) {}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(tinywot_process_request_should_succeed);
  RUN_TEST(tinywot_process_request_should_succeed_when_op_mismatch);
  RUN_TEST(tinywot_process_request_should_succeed_when_not_found);
  RUN_TEST(tinywot_process_request_should_succeed_when_not_implemented);

  return UNITY_END();
}
