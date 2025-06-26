CFLAGS=-std=c2x -g -Wall -Wextra -Wunused-result
SRC_DIR=dev
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
# Only include objects for modules that have corresponding source files
objs = $(sort $(foreach m,$1,$(if $(wildcard ${SRC_DIR}/$(m).c),${OBJ_DIR}/$(m).o)))

## deps NAMES...
deps = $(sort $(foreach thing,$1,${$(thing)_deps}))

## add NAMES...
add = $(sort $1 $(call deps,$1))

types_deps =
allocator_deps = $(call add,types)
cstr_deps = $(call add,types)
strbuf_deps = $(call add,allocator cstr)
list_deps = $(call add,allocator)
array_deps = $(call add,allocator)
hashmap_deps = $(call add,allocator cstr)
path_deps = strbuf

allc_deps = $(call add,types allocator cstr strbuf list array path)

define PROGRAM_build =
build_$1: ${OBJ_DIR}/$1.o build_deps_for_$1
build_deps_for_$1: $(addprefix build_,$(call deps,$1))
build_test_for_$1: ${BIN_DIR}/test_$1
endef

$(foreach dep,${allc_deps},$(eval $(call PROGRAM_build,${dep})))
$(eval $(call PROGRAM_build,tests))

${OBJ_DIR}/%.o: ${SRC_DIR}/%.c ${SRC_DIR}/%.h | ${OBJ_DIR}
	cc ${CFLAGS} $< -c -o $@

${BIN_DIR}/test_%: ${TESTS_DIR}/%.c $(call objs,$(call deps,$*)) | ${BIN_DIR}
	cc ${CFLAGS} $< $(call objs,$(call deps,$*)) -o $@

test_%: ${BIN_DIR}/test_% ; $<

clean:
	if [ -d ${CACHE_DIR} ]; then rm -r ${CACHE_DIR}; fi

