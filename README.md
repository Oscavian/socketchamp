# SocketChamp

In this repo I'm experimenting with socket programming in C.
You can probably find better boilerplate code.

**Contents:**

- `webclient` - Send simple HTTP GET Requests to any webserver
- `kekclient` and `kekserver` - client & server communicating over a simple custom protocol

# Build & Run

Make targets exists for every program.

```bash
make webclient
./bin/webclient
```

# Usage

## webclient

Performs a HTTP GET / Request to Port 80 at \<ip\>

`./bin/webclient <ip>`

## kekclient

## kekserver