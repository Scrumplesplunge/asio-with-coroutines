# Boost ASIO with the Coroutines TS

This project is an experiment of mine to see how difficult it is to wrap
existing asynchronous APIs using the new [Coroutines TS][1]. The project
implements a simple asynchronous echo server in two ways. The first way is by
using ASIO directly, and is very similar to the [echo server example][2]
provided in the boost documentation. The second way is by writing a small amount
of boilerplate to wrap the ASIO interface, and then simply writing some
coroutines for the echo server.

[1]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/n4680.pdf
[2]: http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/example/cpp11/echo/async_tcp_echo_server.cpp

## Requirements

To compile this code, you will need clang 5, the corresponding libc++, and
boost. If you are using archlinux and pacaur, you can get all three of these
with:

    pacaur -S clang boost libc++

The project can then be built with `make`, `make opt` or `make debug`.

## Project Layout

The conventional implementation using asio is in `classic_asio.cc`. The
coroutines implementation is broken into two parts: `coroutine_wrappers.{h,cc}`
contain the boilerplate for converting the API to coroutines, and
`coroutines_asio.cc` contains the echo server.

The boilerplate code in `coroutine_wrappers` does not cater specifically to the
echo server use case, which is why I have separated it from the echo server
implementation.
