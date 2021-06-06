/*
 * An example Web Thing exposing a LED based on TinyWoT and Arduino.
 *
 * Since TinyWoT is not associated with any protocol yet, in this example a
 * simple text-based protocol is used. Each line is an instruction:
 *
 * - `PR <path>`: Read property at path
 * - `PW <path> <value>`: Write property at path
 * - `AC <path>`: Invoke action at path
 *
 * The protocol is manually parsed in this example, so it may look complicated.
 * In the future there will be protocol binding interfaces that transforms
 * network requests into TinyWoTRequest.
 *
 * The following resources are available in this example:
 *
 * - `/led`: read-write property: the Arduino built-in LED
 * - `/toggle`: action: flip the status of LED
 *
 * For example, to flip the status of LED, enter the following line and press
 * \[Enter\]:
 *
 * ```
 * AC /toggle
 * ```
 *
 * SPDX-FileCopyrightText: 2021 Junde Yhi <junde@yhi.moe>
 * SPDX-License-Identifier: MIT
 */

#include <Arduino.h>
#include <tinywot.h>

/*
 * The LED pin number that this Thing is operating.
 */
#define LED LED_BUILTIN

/*
 * LED handler, forward declaration. The implementation is at the bottom of the
 * source file.
 */
static TinyWoTResponse hLED(const TinyWoTRequest *const request, void *ctx);

/*
 * The path strings to the resources that this Thing offers. To put the string
 * into the (AVR) program space, declaration like this is required. Otherwise,
 * this could be directly put into the handlers list (below).
 */
static const char path_led[] PROGMEM = "/led";
static const char path_toggle[] PROGMEM = "/toggle";

/*
 * Handlers used in this Thing. This list MUST end with #TINYWOT_HANDLER_END.
 * Note that in this example, a few special techniques are used:
 *
 * - `/led` can accept both read and write request since the two
 *   TinyWoTOperationType are OR-ed.
 * - `/led` and `/toggle` both points to a same handler. As long as the handler
 *   expects this, it's okay.
 */
static const TinyWoTHandler handlers[] = {
  {path_led,
   TINYWOT_OPERATION_TYPE_READ_PROPERTY | TINYWOT_OPERATION_TYPE_WRITE_PROPERTY,
   hLED, NULL},
  {path_toggle, TINYWOT_OPERATION_TYPE_INVOKE_ACTION, hLED, NULL},
  TINYWOT_HANDLER_END};

/*
 * This Thing. At this moment it only contains a list of handlers.
 */
static const TinyWoTThing thing = {
  .handlers = handlers,
};

/*
 * TinyWoT configuration object. At this moment it only contains a few function
 * pointers.
 *
 * Note the use of a set of different function pointers -- these are special
 * avr-libc provided functions that work with program memory data. If an
 * implementor makes use of such feature, they must use this set of different
 * functions, otherwise the program will crash, since the RAM and the program
 * space don't use a same address space.
 */
static const TinyWoTConfig config = {
#ifdef __AVR_ARCH__
  .strcmp = strcmp_P,
  .strlen = strlen_P,
  .memcpy = memcpy_P,
#else
  .strcmp = strcmp,
  .strlen = strlen,
  .memcpy = memcpy,
#endif
};

void setup() {
  /* Setting up the LED */
  pinMode(LED, OUTPUT);

  /* Setting up the serial console */
  Serial.begin(9600);
  while (!Serial) {}
  Serial.setTimeout(30000);

  Serial.println(F(R"%(
TinyWoT simple example - Arduino built-in LED over serial

A simple protocol is used here. Each line is an instruction:

- `PR <path>`: Read property at path
- `PW <path> <value>`: Write property at path
- `AC <path>`: Invoke action at path

In this example, the following resources are available:

- `/led` [RW]: The Arduino built-in LED
- `/toggle` [AC]: Flip the status of the LED
)%"));
}

void loop() {
  Serial.print(F("> "));
  while (!Serial.available()) {}

  String in = Serial.readStringUntil('\n');
  int iPath = in.indexOf(' ');
  int iValue = in.indexOf(' ', iPath + 1);

  if (iValue < 0) {
    iValue = in.length() - 1;
  }

  String path = in.substring(iPath + 1, iValue);
  String opt = in.substring(iValue + 1);
  path.trim();
  opt.trim();

  /*
   * Constructing a TinyWoTRequest. Note that normally this step should be done
   * by a protocol binding library. At this moment we just manually parse the
   * console input and DIY.
   */

  TinyWoTRequest req;
  req.path = path.c_str();

  if (in.startsWith("PR")) {
    req.op = TINYWOT_OPERATION_TYPE_READ_PROPERTY;
  } else if (in.startsWith("PW")) {
    req.op = TINYWOT_OPERATION_TYPE_WRITE_PROPERTY;
    req.content_type = TINYWOT_CONTENT_TYPE_TEXT_PLAIN;
    req.content_length = opt.length();
    req.content = (const uint8_t *)opt.c_str();
  } else if (in.startsWith("AC")) {
    req.op = TINYWOT_OPERATION_TYPE_INVOKE_ACTION;
  } else {
    Serial.println(F("Unsupported instruction!"));
    return;
  }

  /*
   * Calling the main dispatcher. This function is quite simple: it just finds
   * the appropriate handler in TinyWoTThing::handlers and executes it. It's
   * pure (functionally) so it's stateless.
   */

  TinyWoTResponse resp = tinywot_process_request(&config, &thing, &req);

  /*
   * The code below are for printing the TinyWoTResponse returned. In the
   * future, a protocol binding implementation will be able to send this out
   * as a network message, but now we can only do this to demonstrate TinyWoT.
   */

  switch (resp.status) {
    case TINYWOT_RESPONSE_STATUS_OK:
      Serial.print(F("OK"));
      break;
    case TINYWOT_RESPONSE_STATUS_BAD_REQUEST:
      Serial.print(F("Bad Request"));
      break;
    case TINYWOT_RESPONSE_STATUS_UNSUPPORTED:
      Serial.print(F("Unsupported"));
      break;
    case TINYWOT_RESPONSE_STATUS_METHOD_NOT_ALLOWED:
      Serial.print(F("Method Not Allowed"));
      break;
    case TINYWOT_RESPONSE_STATUS_ERROR:
      Serial.print(F("Internal Error"));
      break;
    case TINYWOT_RESPONSE_STATUS_NOT_IMPLEMENTED:
      Serial.print(F("Not Implemented"));
      break;
    case TINYWOT_RESPONSE_STATUS_UNKNOWN: // fall through
    default:
      Serial.print(F("Unknown"));
      break;
  }

  switch (resp.content_type) {
    case TINYWOT_CONTENT_TYPE_TEXT_PLAIN:
      Serial.print(F(" text/plain"));
      break;
    case TINYWOT_CONTENT_TYPE_OCTET_STREAM:
      Serial.print(F(" application/octet-stream"));
      break;
    case TINYWOT_CONTENT_TYPE_JSON:
      Serial.print(F(" application/json"));
      break;
    case TINYWOT_CONTENT_TYPE_TD_JSON:
      Serial.print(F(" application/td+json"));
      break;
    default:
      break;
  }

  if (resp.content) {
    Serial.print(F(" "));
    Serial.print((const char *)resp.content);
  }

  Serial.println();

  return;
}

/*
 * Handler for LED related requests.
 */
static TinyWoTResponse hLED(const TinyWoTRequest *const request, void *ctx) {
  (void)ctx;

  static const char str_true[] PROGMEM = "true";
  static const char str_false[] PROGMEM = "false";

  int led = digitalRead(LED);
  TinyWoTResponse response;

  switch (request->op) {
    case TINYWOT_OPERATION_TYPE_READ_PROPERTY:
      response.status = TINYWOT_RESPONSE_STATUS_OK;
      response.content_type = TINYWOT_CONTENT_TYPE_TEXT_PLAIN;

      if (led) {
        response.content_length = strlen(str_true);
        response.content = (uint8_t *)str_true;
      } else {
        response.content_length = strlen(str_false);
        response.content = (uint8_t *)str_false;
      }

      break;

    case TINYWOT_OPERATION_TYPE_WRITE_PROPERTY:
      if (strcmp_P((const char *)request->content, str_true) == 0) {
        digitalWrite(LED, HIGH);
        response.status = TINYWOT_RESPONSE_STATUS_OK;
        response.content_type = TINYWOT_CONTENT_TYPE_TEXT_PLAIN;
        response.content_length = strlen(str_true);
        response.content = (uint8_t *)str_true;
      } else if (strcmp_P((const char *)request->content, str_false) == 0) {
        digitalWrite(LED, LOW);
        response.status = TINYWOT_RESPONSE_STATUS_OK;
        response.content_type = TINYWOT_CONTENT_TYPE_TEXT_PLAIN;
        response.content_length = strlen(str_false);
        response.content = (uint8_t *)str_false;
      } else {
        response.status = TINYWOT_RESPONSE_STATUS_BAD_REQUEST;
      }

      break;

    case TINYWOT_OPERATION_TYPE_INVOKE_ACTION:
      if (led) {
        digitalWrite(LED, LOW);
      } else {
        digitalWrite(LED, HIGH);
      }

      response.status = TINYWOT_RESPONSE_STATUS_OK;

      break;

    default:

      /*
       * This should not happen, since TinyWoT has prevented such thing from
       * happening. The dead code is purely for completeness (to some extents).
       */

      response.status = TINYWOT_RESPONSE_STATUS_METHOD_NOT_ALLOWED;

      break;
  }

  return response;
}
