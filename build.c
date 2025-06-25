/*

./build [COMMAND] [COMMAND OPTIONS]

COMMAND: build
--------------

    ./build build [OPTIONS]

    OPTIONS:
        --shared
        --static
        --debug

COMMAND: clean
--------------

    ./build clean

COMMAND: test
--------------

    ./build test [OPTIONS]

    OPTIONS:
        --debug
        --release

*/

#define ALLC_IMPL
#include "include/types.h"
#include "include/allocator.h"

// ===================================================================
// hashmap.h

#ifdef ALLC_IMPL
#define ALLC_HASHMAP_IMPL
#endif

// Unmanaged Hash Map {{{1
// =======================

struct allc_hashmap_s {
    bool (*eqfn)(void *, void *);
    size_t (*hashfn)(void *);
    size_t length;
    size_t map_size;
    struct allc_hashmap_item_s {
        void *key;
        void *value;
        struct allc_hashmap_item_s *next;
    } *map[];
};

void allc_hashmap_init(struct allc_hashmap_s *self, size_t size)
{
    self->length = 0;
    self->map_size = size;
    allc_mem_clear(self->map, size * sizeof(void *));
    return self;
}

bool allc_hashmap_contains(struct allc_hashmap_s *self, void *key)
{
    return allc_hashmap_item_get(self, key) != NULL;
}

struct allc_hashmap_item_s* allc_hashmap_item_get(struct allc_hashmap_s *self, void *key)
{
    size_t hash = self->hashfn(key);
    size_t index = hash % self->map_size;
    struct allc_hashmap_item_s *item = self->map[index];
    while (item) {
        if (self->eqfn(item->key, key)) {
            return item;
        }
        item = item->next;
    }
    return NULL;
}

struct allc_hashmap_item_s* allc_hashmap_item_remove(struct allc_hashmap_s *self, void *key) 
{
    size_t hash = self->hashfn(key);
    size_t index = hash % self->map_size;
    struct allc_hashmap_item_s *item = self->map[index];
    struct allc_hashmap_item_s *prev = NULL;
    while (item) {
        if (self->eqfn(item->key, key)) {
            if (prev) {
                prev->next = item->next;
            } else {
                self->map[index] = item->next;
            }
            return item;
        }
        prev = item;
        item = item->next;
    }
    return NULL;
}

struct allc_hashmap_item_s* allc_hashmap_item_insert(struct allc_hashmap_s *self, struct allc_hashmap_item_s *item)
{
    size_t hash = self->hashfn(item->key);
    size_t index = hash % self->map_size;
    struct allc_hashmap_item_s *curr = self->map[index];
    struct allc_hashmap_item_s *prev = NULL;
    while (curr) {
        if (self->eqfn(curr->key, item->key)) {
            break;
        }
        prev = curr;
        curr = curr->next;
    }
    // curr is NULL or curr->key == item->key
    // prev is NULL or prev->next == curr
    if (curr) {
        item->next = curr->next;
    } else {
        item->next = NULL;
    }
    if (prev) {
        prev->next = item;
    } else {
        self->map[index] = item;
    }    
    return curr;
}

allc_f32_t allc_hashmap_load_factor(struct allc_hashmap_s *self) {
    return (float)self->length / self->map_size;
}

// HashMap {{{1
// ============


#ifdef ALLC_HASHMAP_IMPL
typedef struct {
    struct allc_hashmap_s base;
    allc_allocator_t allocator;
} *HashMap;
#else
typedef void *HashMap;
#endif

HashMap allc_hashmap_new(allc_allocator_t allocator) {
    HashMap self = allocator.alloc(sizeof (HashMap) + sizeof(void *));
    self->allocator = allocator;
    allc_hashmap_init(self, 1);
    return self;
}

void allc_hashmap_delete(HashMap self) {
    // TODO: Free items
    self->allocator.free(self);
}

void allc_hashmap_set(HashMap self, void *key, void *value);

allc_hashmap_item_t allc_hasmap_item_new(allc_hashmap_t *owner_p, void *key, void *value) {
    allc_hashmap_item_t self = (*owner_p)->allocator.alloc(sizeof(allc_hashmap_item_t));
    self->key = key;
    self->value = value;
    self->next = NULL;
    return self;
}

void allc_hashmap_item_delete(allc_hashmap_t *owner_p, allc_hashmap_item_t self) {
    (*owner_p)->allocator.free(self);
}


// ===================================================================
// argparser.h


    allc_argparser_s p = {};
    allc_argparser_inherit(&p, b->cli);

    allc_argparser_add_options(&p, &(allc_argparser_option_s[]) {
        { &opt_debug,           allc_types_bool, "-d --debug",  "Compile with debug symbols." },
        { &opt_build_shared,    allc_types_bool, "--shared",    "Compile as shared library." },
        { &opt_build_static,    allc_types_bool, "--static",    "Compile as static library." },
        NULL,
    });

    allc_argparser_parse(&p);


#include "include/cstr.h"

typedef struct {
    int argp;
    int argc;
    char **argv;
} allc_argparser_s;

typedef struct {
    enum {
        ALLC_ARGPARSE_TYPE_flag,
        ALLC_ARGPARSE_TYPE_int,
        ALLC_ARGPARSE_TYPE_float,
        ALLC_ARGPARSE_TYPE_string,
    } type;
    allc_string_t name;
} allc_argparser_arg_s;

allc_argparser_arg_s
allc_argparser_parse_arg(allc_argparser_s *self)
{
    allc_string_t s = self->argv[self->argp];

    if (allc_cstr_is_starting_with(s, "-")) {
        return (allc_argparser_arg_s) {
            .type = ALLC_ARGPARSE_TYPE_flag,
            .name = s,
        };
    }
}



// ===================================================================
// behavior_tree.h

// Unmanaged Behavior Tree {{{1
// ----------------------------

struct allc_bt_s {
    struct allc_tree_s *parent;
    struct allc_tree_s *children[];
};

// ===================================================================
// builder.h

typedef struct {
    struct allc_builder_project_s {
        allc_string_t name;
        allc_string_t version;
        enum {
            allc_version_resolution_custom,
            allc_version_resolution_semver,
        } version_resolution;
        allc_string_t description;
        allc_string_t license;
        allc_string_t author;
        allc_string_t email;
    } project;
    struct allc_builder_cli_s {
        int argc;
        char **argv;
        char **envp;
        struct allc_builder_cli_option_s {
            void *out;
            enum allc_types_e type;
            allc_string_t name;
            allc_string_t description;
        } *options;
        struct allc_builder_cli_command_s {
            char *name;
            void (*fn)(struct allc_builder_s*);
        } *commands;
    } cli;
} allc_builder_s;

void allc_builder_main(allc_builder_s *builder)
{

}


// ===================================================================
// builder/step.h

typedef enum {
    ALLC_BUILDER_STEP_sequence,
    ALLC_BUILDER_STEP_fallback,
    // ALLC_BUILDER_STEP_async_sequence,
    // ALLC_BUILDER_STEP_async_fallback,
    ALLC_BUILDER_STEP_execute,
    ALLC_BUILDER_STEP_NUM,
} allc_builder_step_e;

typedef struct {
    allc_builder_step_e tag; 
} allc_builder_step_s;

int allc_builder_step_run(struct allc_builder_step_s *root) {}

typedef struct {
    allc_builder_step_s base;
    allc_builder_step_s **steps;
} allc_builder_step_sequence_s;

int allc_builder_step_sequence_run(allc_builder_step_sequence_s *root)
{
    allc_builder_step_s *step = ;
    while (root->steps[i++] != NULL) {
        allc_builder_step_run(step);

    }
}

typedef struct {
    allc_builder_step_s base;
    allc_builder_step_s *steps; // Should be null terminated
} allc_builder_step_fallback_s;

int allc_builder_step_fallback_run(struct allc_builder_step_fallback_s *root) {}




typedef enum {
    ALLC_BUILDER_ARTIFACT_shared,
    ALLC_BUILDER_ARTIFACT_static,
    ALLC_BUILDER_ARTIFACT_executable,
} allc_builder_artifact_e;

typedef struct {
    allc_builder_step_s base;
    allc_string_t source;               // source file
    allc_string_t output;               // output file
    allc_string_t target;               // target platform
    allc_builder_artifact_e artifact;   // type of artifact
} allc_builder_step_artifact_s;

allc_builder_step_artifact_s
allc_builder_step_artifact_new(allc_builder_s *builder, allc_string_t source, allc_string_t output)
{
    return (allc_builder_step_artifact_s) {
        .base.tag = ALLC_BUILDER_STEP_artifact,
        .source = source,
        .output = output,
        .target = NULL, // TODO: Determine default from builder
        .artifact = ALLC_BUILDER_ARTIFACT_executable,
    };
}


// ===================================================================
// build.c

static allc_bool_t opt_debug;
static allc_bool_t opt_build_shared;
static allc_bool_t opt_build_static;

void build_cmd(allc_builder_s *b)
{
    allc_argparser_s p = {};
    allc_argparser_inherit(&p, b->cli);
    
    allc_argparser_add_options(&p, &(allc_argparser_option_s[]) {
        { &opt_debug,           allc_types_bool, "-d --debug",  "Compile with debug symbols." },
        { &opt_build_shared,    allc_types_bool, "--shared",    "Compile as shared library." },
        { &opt_build_static,    allc_types_bool, "--static",    "Compile as static library." },
        NULL,
    });

    allc_argparser_parse(&p);


    allc_builder_step_artifact_s step = allc_builder_step_artifact_new(builder, "src/allc.c", "liballc.a");
    step.artifact = 
        opt_build_shared ? ALLC_BUILDER_ARTIFACT_shared :
        opt_build_static ? ALLC_BUILDER_ARTIFACT_static :
                           ALLC_BUILDER_ARTIFACT_executable;

    allc_builder_run(&(allc_builder_step_sequence_s) {
        .base.tag = ALLC_BUILDER_STEP_sequence,
        .steps = &(allc_builder_step_s*[]) {
            &step,
            NULL,
        },
    });
}

int main(int argc, char ** argv, char ** envp) 
{
    allc_allocator_c_s A;
    allc_allocator_c_init(&A);

    allc_builder_main(&(allc_builder_s) {
        // .allocator = &A,
        .project = {
            .name = "allc",
            .version = "0.0.1",
            .version_resolution = allc_version_resolution_semver,
            .description = "A library for C projects.",
            .license = "MIT",
            .author = "Kaj Munhoz Arfvidsson",
            .email = "kajarf@kth.se",
        },
        .cli = {
            .argc = argc, .argv = argv, .envp = envp,
            .options = &(struct allc_builder_cli_option_s[]) {
            },
            .commands = &(struct allc_builder_cli_command_s[]){
                { .name = "build", .fn = &build_cmd },
                NULL,
            },
        },
    });
}

q