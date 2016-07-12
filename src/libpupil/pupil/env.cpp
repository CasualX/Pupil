#include "env.h"
#include "builtins.h"

namespace pupil {

uint32_t hash(const char* str, size_t len) {
	uint32_t acc = 5381;
	for (size_t i = 0; i < len; ++i) {
		unsigned char chr = str[i];
		if (chr >= 'A' && chr <= 'Z') {
			chr = chr - 'A' + 'a';
		}
		acc = (acc * 33) ^ (uint32_t)chr;
	}
	return acc;
}

bool env_add(struct Env* env, const char* name, BuiltinFn pfn) {
	uint32_t h = hash(name, strlen(name));
	for (size_t i = 0; i < env->len; ++i) {
		// Already exists, overwrite it
		if (env->names[i] == h) {
			env->pfns[i] = pfn;
			return true;
		}
	}
	// Check and realloc as needed
	if (env->len == env->cap) {
		size_t new_cap = env->cap ? env->cap * 3 / 2 : 40;
		env->names = (uint32_t*)realloc(env->names, new_cap * sizeof(uint32_t));
		env->pfns = (BuiltinFn*)realloc(env->pfns, new_cap * sizeof(BuiltinFn));
		if (!env->names || !env->pfns) {
			return false;
		}
		env->cap = new_cap;
	}
	// Add a new entry
	size_t n = env->len++;
	env->names[n] = h;
	env->pfns[n] = pfn;
	return true;
}

BuiltinFn env_find(struct Env* env, const char* name, size_t len) {
	uint32_t h = hash(name, len);
	for (size_t i = 0; i < env->len; ++i) {
		if (env->names[i] == h) {
			return env->pfns[i];
		}
	}
	return nullptr;
}

void env_init(struct Env* env) {
	PUPIL_BUILTINS(PUPIL_INIT);
}

}
