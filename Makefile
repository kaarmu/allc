CFLAGS=-std=c99 -g -Wall -Wextra -Wunused-result
SRC_DIR=src
TESTS_DIR=tests

PROJ_DIR=${shell pwd}
CACHE_DIR=.cache
OBJ_DIR=${CACHE_DIR}/o
BIN_DIR=${CACHE_DIR}/bin

${OBJ_DIR}:
	mkdir -p $@
${BIN_DIR}:
	mkdir -p $@

## sandwich PREFIX NAMES... SUFFIX
sandwich = $(addprefix $1,$(addsuffix $3,$2))

## objs NAMES...
objs = $(sort $(call sandwich,${OBJ_DIR}/,$1,.o))

## deps NAMES...
deps = $(sort $(foreach thing,$1,${$(thing)_deps}))

## add NAMES...
add = $(sort $1 $(call deps,$1))

allocator_deps =
cstr_deps =
strbuf_deps = $(call add,allocator cstr)
path_deps = strbuf

allc_deps = $(call add,allocator cstr strbuf path)
tests_deps = $(call add,cstr strbuf)

define PROGRAM_build =
build_$1: ${OBJ_DIR}/$1.o build_deps_for_$1
build_deps_for_$1: $(addprefix build_,$(call deps,$1))
build_test_for_$1: ${BIN_DIR}/test_$1
endef

$(foreach dep,${allc_deps},$(eval $(call PROGRAM_build,${dep})))
$(eval $(call PROGRAM_build,tests))

${OBJ_DIR}/%.o: ${SRC_DIR}/%.c ${SRC_DIR}/%.h | ${OBJ_DIR}
	cc ${CFLAGS} $< -c -o $@

${BIN_DIR}/test_%: ${TESTS_DIR}/%.c build_% build_deps_for_tests | ${BIN_DIR}
	cc ${CFLAGS} $< $(call objs,$* $(call deps,$* tests)) -o $@

test_%: ${BIN_DIR}/test_% ; $<

.PHONY: clean
clean:
	if [ -d ${CACHE_DIR} ]; then rm -r ${CACHE_DIR}; fi

