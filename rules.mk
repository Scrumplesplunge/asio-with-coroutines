CXXFLAGS += -Wall -Wextra -Isrc -Igen

.PHONY: default
default: all

# Build with optimized configuration.
.PHONY: opt
opt: all

# Build with profiling instrumentation.
.PHONY: prof
prof: all

# Build with debug configuration.
.PHONY: debug
debug: all

# Delete all generated files.
.PHONY: clean
clean:
	@echo Removing generated files
	@rm -rf bin dep gen obj

# Pattern rule for generating a static library.
bin/%.a:
	@echo Archiving $*
	@mkdir -p $(dir $@)
	@${AR} cr $@ $^

# Pattern rule for generating a binary.
bin/%:
	@echo Linking $*
	@mkdir -p $(dir $@)
	@${CXX} ${LDFLAGS} $^ -o $@ ${LDLIBS}

# Pattern rule for generated files.
.PRECIOUS: gen/%.cc gen/%.h
gen/%.cc gen/%.h: src/gen_%.sh src/%.txt
	@echo Generating $*
	@mkdir -p gen/$(dir $*)
	@$^

GENERATORS = $(shell find src -name 'gen_*.sh')
GENERATED = $(patsubst src/gen_%.sh, gen/%.h, ${GENERATORS})

# Pattern rule for compiling a cc file into an o file.
obj/%.o: src/%.cc $(wildcard src/%.h) | ${GENERATED}
	@echo Compiling $*
	@mkdir -p {obj,dep}/$(dir $*)
	@${CXX} ${CXXFLAGS} ${CPPFLAGS} -MMD -MF dep/$*.d $< -c -o obj/$*.o

# Pattern rule for compiling a generated cc file into an o file.
obj/%.o: gen/%.cc $(wildcard gen/%.h)
	@echo Compiling $*
	@mkdir -p {obj,dep}/$(dir $*)
	@${CXX} ${CXXFLAGS} ${CPPFLAGS} -MMD -MF dep/$*.d $< -c -o obj/$*.o

DEPENDS = $(shell [[ -d dep ]] && find dep -name '*.d')
