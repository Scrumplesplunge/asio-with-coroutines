include config.mk
include rules.mk

.PHONY: all

BINARIES =  \
	classic_asio  \
	coroutines_asio
all: $(patsubst %, bin/%, ${BINARIES})

CLASSIC_ASIO_DEPS =  \
	classic_asio
bin/classic_asio: $(patsubst %, obj/%.o, ${CLASSIC_ASIO_DEPS})

COROUTINES_ASIO_DEPS =  \
	coroutines_asio  \
	coroutine_wrappers
bin/coroutines_asio: $(patsubst %, obj/%.o, ${COROUTINES_ASIO_DEPS})

-include ${DEPENDS}
