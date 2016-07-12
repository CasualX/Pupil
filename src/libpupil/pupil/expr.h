#pragma once

// Expression evaluator.

#include "env.h"

namespace pupil {

struct FnStack {
	BuiltinFn* stack;
	int8_t* pres;
	size_t* nargs;
	size_t len;
	size_t cap;
};
struct ValStack {
	value_t* stack;
	size_t len;
	size_t cap;
};
struct Expr {
	struct Env* env;
	struct FnStack fns;
	struct ValStack vals;
	enum Error {
		Success = 0,
		ExpectedOperator,
		NaExpression,
		InternalCorruption,
		UnfinishedExpression,
		UnexpectedEos,
		InvalidToken,
		BadArgument,
		UnknownSymbol,
		Count,
	} err;
	bool next;
};

// Error descriptions.
extern const char* ERROR_DESC[Expr::Error::Count];

// Feed input to the expression.
extern bool parse(struct Expr* expr, const char* in);
// Evaluate the remainder of the expression.
extern bool eval(struct Expr* expr, value_t* result);

}
