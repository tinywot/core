<!--
  SPDX-FileCopyrightText: 2019, 2021-2023 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: CC0-1.0
-->

# TinyWoT

TinyWoT is a library for implementing a [W3C Web of Things (WoT)](https://www.w3.org/WoT/) compliant _[Web Thing](https://www.w3.org/TR/2020/REC-wot-architecture-20200409/#sec-web-thing)_ on an embedded device.

It consists of:

- A set of _Native WoT APIs ([WoTA] &sect; 8.8.1)_ in C.
- A _WoT Runtime ([WoTA] &sect; 8.2)_.
- ~~A few _Protocol Stack Implementations ([WoTA] &sect; 8.6)_.~~
- ~~A high-level _Servient framework ([WoTA] &sect; 6.7)_~~.

The striked-through parts aren't implemented yet.

Version 0.3 of TinyWoT is a work-in-progress. It's a complete rewrite comparing to the previous versions.

[WoTA]: https://www.w3.org/TR/2020/REC-wot-architecture-20200409/

## License

TinyWoT is licensed under the MIT license; see [`LICENSE`](LICENSE) for a copy of the license.

This project is [REUSE 3.0](https://reuse.software/) compliant: every file carries its own copyright and licensing information in either its comment header or a corresponding `.license` file. See [`LICENSES/`](LICENSES/) for copies of license used across the project.
