CXX = clang++
CXXFLAGS += -std=c++2a -Wall -Wextra -pedantic -fcoroutines-ts -stdlib=libc++
opt: CXXFLAGS += -ffunction-sections -fdata-sections -flto -Ofast -march=native
prof: CXXFLAGS += -g -Ofast -march=native
debug: CXXFLAGS += -O0 -g

LDFLAGS += -fuse-ld=gold -stdlib=libc++
opt: LDFLAGS += -s -Wl,--gc-sections -flto -Ofast

LDLIBS = -lpthread -lboost_system
