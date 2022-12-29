<!--
  SPDX-FileCopyrightText: 2019, 2021-2022 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: CC0-1.0
-->

# TinyWoT

TinyWoT is a library for implementing [W3C Web of Things (WoT)](https://www.w3.org/WoT/) on embedded devices.

It consists of a set of _([WoTA 1.1] &sect; 8.8.1) Native WoT APIs_ in C, a _([WoTA 1.1] &sect; 8.2) WoT Runtime_, ~~a few _([WoTA 1.1] &sect; 8.6) Protocol Stack Implementations_, and a high-level _([WoTA 1.1] &sect; 6.9) Servient_ framework~~. (The strike-through parts aren't implemented yet.)

Version 0.3 of TinyWoT is a work-in-progress. It's a complete rewrite comparing to the previous versions.

[WoTA 1.1]: https://www.w3.org/TR/wot-architecture11/

## License

TinyWoT is licensed under the MIT license; see [`LICENSE`](LICENSE) for a copy of the license.

This project is [REUSE 3.0](https://reuse.software/) compliant: every file carries its own copyright and licensing information in either its comment header or a corresponding `.license` file. See [`LICENSES/`](LICENSES/) for copies of license used across the project.
