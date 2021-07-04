<!--
SPDX-FileCopyrightText: 2019, 2021 Junde Yhi <junde@yhi.moe>
SPDX-License-Identifier: CC0-1.0
-->

# TinyWoT

TinyWoT is a simple, light-weight [W3C Web of Things (WoT)][wot] [runtime][wot-rt] library providing [native WoT APIs][wot-napi] for resource-constrained devices.

[wot]: https://www.w3.org/WoT/
[wot-rt]: https://www.w3.org/TR/wot-architecture11/#wot-runtime
[wot-napi]: https://www.w3.org/TR/wot-architecture11/#native-impl

## Installation

- [PlatformIO][pio]: `pio lib install tinywot`

Usually you use this library along with a [protocol binding][wot-pb] library; see below.

[pio]: https://platformio.org/

## Usage

Use TinyWoT with its [protocol binding][wot-pb] implementations:

- [Simple HTTP](https://github.com/lmy441900/tinywot-http-simple)

They convert network requests into `TinyWoTRequest`s and `TinyWoTResponse`s into network responses. It's completely fine to implement your own.

To implement a Web Thing:

1. Write handlers.
2. Construct a list of handler, with its bound path, its acceptable affordances, and a context (user data) pointer (if needed).
3. Construct a Thing, with the list of handler and its size included.
4. Receive a request from a [protocol binding][wot-pb] library, pass the Thing and a request into `tinywot_process`, and send the returned response using the [protocol binding][wot-pb] library.

```c
// A handler implementing a resource
static TinyWoTRequest handler_test(TinyWoTRequest *req, void *ctx);

// A path to a resource, stored in the program space. Remember to define
// TINYWOT_USE_PROGMEM if PROGMEM string is used.
static const char path_test[] PROGMEM = "/test";

// A list of handlers, with path, acceptable optype, the function, and context
// (user data).
static TinyWoTHandler handlers[] = {
  {path_test, WOT_OPERATION_TYPE_READ_PROPERTY, handler_test, NULL},
};

// A Thing representation; only handlers are present.
static TinyWoTThing thing = {
  .handlers = handlers,
  .handlers_size = sizeof(handlers) / sizeof(TinyWoTHandler),
};

// Receive a TinyWoTRequest using a protocol binding library. Here
// TinyWoT-HTTP-Simple is used.
if (tinywot_http_simple_recv(&cfg, &req)) {
  // Malformed request
}

// Process the request
resp = tinywot_process(&thing, &req);

// Send a HTTP request back.
if (tinywot_http_simple_send(&cfg, &req)) {
  // Error
}
```

### Configuration

- `TINYWOT_USE_PROGMEM`: tell TinyWoT to use AVR program space (flash memory) special functions. Define this if you use `PROGMEM` / `PSTR` in your AVR-targeted application. When this preprocessor variable is defined, `strcmp_P` will be used instead of `strcmp`. This requires `TinyWoTHandler::path` to be a program space pointer.

[wot-pb]: https://www.w3.org/TR/wot-binding-templates/
[wot-td]: https://www.w3.org/TR/wot-thing-description11/

## License

This project is [REUSE 3.0][reuse] compliant: every file is carried with its own copyright and licensing information in the comment headers or the corresponding `.license` files. In general:

- Source files are licensed under the MIT license.
- Various data files are (un)licensed under the CC0 license.

See the [LICENSES](LICENSES) directory for copies of licenses used across this project. The [LICENSE](LICENSE) file also contains a MIT license for non-REUSE practices.

[reuse]: https://reuse.software/spec/
