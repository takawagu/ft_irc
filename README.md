*This project has been created as part of the 42 curriculum by tochi, takawagu.*

# ft_irc

## Description

`ft_irc` is an Internet Relay Chat (IRC) server written from scratch in **C++98**.
The goal of the project is to implement a subset of the IRC protocol so that a
real, unmodified IRC client can connect to our server, authenticate, exchange
messages, and interact through channels — exactly as it would with any standard
IRC server.

The server:

- Listens on a TCP port (IPv4/IPv6) and accepts multiple clients simultaneously.
- Uses a **single** `poll()` (or equivalent) loop to multiplex **all** I/O
  operations (accept, recv, send) in **non-blocking** mode. No forking, no
  threads.
- Aggregates partial TCP packets and rebuilds complete IRC commands before
  executing them.
- Implements authentication (`PASS`), nickname/username registration (`NICK`,
  `USER`), private messages (`PRIVMSG`), channel join (`JOIN`), and channel
  broadcasting.
- Distinguishes between **regular users** and **channel operators**, and
  supports the operator-only commands `KICK`, `INVITE`, `TOPIC`, and `MODE`
  (`i`, `t`, `k`, `o`, `l`).

Server-to-server communication and the IRC client itself are explicitly out of
scope.

## Instructions

### Requirements

- A C++ compiler supporting the **C++98** standard (`c++` / `clang++` / `g++`).
- `make`.
- `irssi` — the reference IRC client used during evaluation (see
  "Connecting with the reference IRC client" below).

### Build

From the repository root:

```sh
make         # build the ircserv binary
make clean   # remove object files
make fclean  # remove object files and the binary
make re      # fclean + make
```

The build uses the flags `-Wall -Wextra -Werror -std=c++98`.

### Run

```sh
./ircserv <port> <password>
```

- `port` — the TCP port the server listens on (e.g. `6667`).
- `password` — the connection password required by any client that wants to
  connect to the server.

### Quick smoke test with `nc`

The server must correctly reassemble commands delivered in several TCP
segments. This can be verified with:

```sh
nc -C 127.0.0.1 6667
```

Then type a command in several parts, using `Ctrl+D` between parts (for
example `com`, `man`, `d\n`). The server must process the aggregated command
as a single IRC message.

### Connecting with the reference IRC client

The reference client for this project is **[irssi](https://irssi.org/)** — a
terminal-based IRC client. It is the client used during evaluation, and every
feature of `ircserv` is verified against it.

Install it with:

```sh
brew install irssi          # macOS
sudo apt install irssi      # Debian / Ubuntu
```

Connect to a running server:

```sh
irssi -c 127.0.0.1 -p 6667 -w <password> -n <nickname>
```

Once connected, classic IRC commands can be used:

```
/join #channel
/msg <nick> hello
/topic #channel new topic
/mode #channel +i
/kick #channel <nick>
/invite <nick> #channel
```

## Resources

### IRC protocol references

- RFC 1459 — Internet Relay Chat Protocol
  <https://datatracker.ietf.org/doc/html/rfc1459>
- RFC 2810 — IRC: Architecture
- RFC 2811 — IRC: Channel Management
- RFC 2812 — IRC: Client Protocol
- RFC 2813 — IRC: Server Protocol
- Modern IRC client protocol documentation: <https://modern.ircdocs.horse/>

### Network programming

- Beej's Guide to Network Programming:
  <https://beej.us/guide/bgnet/>
- `man 2 poll`, `man 2 socket`, `man 2 getaddrinfo`, `man 7 ip`, `man 7 tcp`.

### Use of AI

- **Protocol exploration**: questions about specific parts of the IRC RFCs
  (numeric replies, message grammar, channel mode semantics) to cross-check
  the interpretation of edge cases.
- **Brainstorming design**: discussing trade-offs for the command dispatcher,
  the client buffering strategy, and the channel/user data structures before
  implementing them.
- **Debugging support**: explaining `poll()` / `recv()` behaviors and helping
  narrow down bugs from reproductions.
- **Documentation**: drafting and polishing this `README.md`.
