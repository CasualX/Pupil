#pragma once

#include <cstdint>

namespace pupil {

// Underlying type used for arithmetic
typedef double value_t;

// Every builtin fn signature.
typedef bool (*BuiltinFn)(struct Env* env, value_t* vals, size_t nargs, value_t* result);

// Persistent environment for expressions, maps names to builtins.
struct Env {
	uint32_t* names;
	BuiltinFn* pfns;
	size_t len;
	size_t cap;
	value_t ans;
};

// Add a builtin fn to the environment.
extern bool env_add(struct Env* env, const char* name, BuiltinFn pfn);
// Find a builtin fn from the environment.
extern BuiltinFn env_find(struct Env* env, const char* name, size_t len);
// Initialize the environment with the default builtins.
extern void env_init(struct Env* env);

}
