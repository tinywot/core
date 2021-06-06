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
- More support to come...

[pio]: https://platformio.org/

## Usage

TinyWoT itself is only a simple dispatcher. Various [protocol binding template][wot-bt] implementations designed for TinyWoT need to be used to convert incoming network requests to a `TinyWoTRequest`, but they do not exist at this moment. If we assume there is one, then a Thing implementor needs to do the following:

1. Write handlers that implement behaviors of the Thing. The behaviors are described in a [WoT Thing Description][wot-td].
2. Create a `TinyWoTHandler []` list, including metadata about the handlers (e.g. the matching paths, the acceptable interaction affordances).
3. Create a `TinyWoTThing` object, including the handler list above.
4. Create a `TinyWoTConfig` object. This passes several platform-specific and implementation-specific resources that TinyWoT needs to use.
5. Upon incomping request, the application first converts the request into a `TinyWoTRequest`, and then passes it into `tinywot_process_request`, along with the `TinyWoTThing` and the `TinyWoTConfig` created above. The `TinyWoTResponse`returned can then be converted back to a network request.

```c
TinyWoTConfig config = {/* ... */};
TinyWoTThing thing = {/* ... */};

TinyWoTRequest request = tinywot_request_from_xxx(/* ... */);
response = tinywot_process_request(&config, &thing, &request);
tinywot_response_to_xxx(/* ... */);
```

[wot-td]: https://www.w3.org/TR/wot-thing-description11/
[wot-bt]: https://www.w3.org/TR/wot-binding-templates/

## License

This project is [REUSE 3.0][reuse] compliant: every file is carried with its own copyright and licensing information in the comment headers or the corresponding `.license` files. In general:

- Source files are licensed under the MIT license.
- Various data files are (un)licensed under the CC0 license.

See the [LICENSES](LICENSES) directory for copies of licenses used across this project. The [LICENSE](LICENSE) file also contains a MIT license for non-REUSE practices.

[reuse]: https://reuse.software/spec/
