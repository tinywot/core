# TinyWoT

TinyWoT is a [Web Thing][wt] library specifically designed for resource-constrained devices.

This project is still at its very early stage, so please stay tuned!

[wt]: https://www.w3.org/TR/wot-thing-description/

## (Proposed) Features

- **Designed for tiny chips.** The library tries to utilize the [Harvard Architecture][harvard] special commands to accomodate long strings so that even controllers with 2KiB RAM can become a Web Thing.
- **Platform-agnostic.** The library does not contain any platform-specific code; instead you must wire them by hand.
- **Runs everywhere.** The code can even be run and tested on your development host.

[harvard]: https://en.wikipedia.org/wiki/Harvard_architecture

## License

This software is licensed under the MIT license. See [COPYING](COPYING) for details.
