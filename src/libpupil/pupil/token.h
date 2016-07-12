#pragma once

// Token lexing.

#include "env.h"

namespace pupil {

struct Token {
	enum Type {
		Unknown,
		Literal,
		Operator,
		Variable,
		FnBegin,
		FnComma,
		FnEnd,
		Count,
	} type;
	union {
		value_t val;
		struct {
			BuiltinFn fn;
			int8_t pre;
		};
	} arg;
	size_t len;
};

// Lex a token.
extern bool token_lex(struct Env* env, const char* in, struct Token* tok);

// Create an implicit multiplication token.
extern struct Token token_implicit_mul();

}
