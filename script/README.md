# Useful Scriptlets During Development

- [`dev.containerfile`](./dev.containerfile): Recipe for building the standard development environment for TinyWoT Core.

## Development Container

### Build

```sh
podman build -t ghcr.io/tinywot/core/dev:latest -f - < dev.containerfile
```

### Run

```sh
podman run --volume /path/to/repo:core --device /dev/ttyACM0 --userns keep-id:uid=1000,gid=1000 --tty --interactive ghcr.io/tinywot/core/dev:latest
```
