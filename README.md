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

[pio]: https://platformio.org/

## Usage

Use TinyWoT with its [protocol binding][wot-pb] implementations:

- [Simple HTTP](https://github.com/lmy441900/tinywot-http-simple)

They convert network requests into `TinyWoTRequest`s and `TinyWoTResponse`s into network responses. It's completely fine to implement your own.

[wot-pb]: https://www.w3.org/TR/wot-binding-templates/
[wot-td]: https://www.w3.org/TR/wot-thing-description11/

## License

This project is [REUSE 3.0][reuse] compliant: every file is carried with its own copyright and licensing information in the comment headers or the corresponding `.license` files. In general:

- Source files are licensed under the MIT license.
- Various data files are (un)licensed under the CC0 license.

See the [LICENSES](LICENSES) directory for copies of licenses used across this project. The [LICENSE](LICENSE) file also contains a MIT license for non-REUSE practices.

[reuse]: https://reuse.software/spec/
