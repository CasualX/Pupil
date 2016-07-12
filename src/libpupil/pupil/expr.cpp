#include "expr.h"
#include "token.h"

namespace pupil {

const char* ERROR_DESC[9] = {
	"success",
	"expected an operator",
	"not an expression",
	"internal corruption",
	"unfinished expression",
	"unexpected eos",
	"invalid token",
	"bad argument",
	"unknown symbol",
};

bool vals_push(struct ValStack* vals, value_t val) {
	// Check and realloc as needed
	if (vals->len == vals->cap) {
		size_t new_cap = vals->cap ? vals->cap * 3 / 2 : 20;
		vals->stack = (value_t*)realloc(vals->stack, new_cap * sizeof(value_t));
		if (!vals->stack) {
			return false;
		}
		vals->cap = new_cap;
	}
	// Push the value
	vals->stack[vals->len++] = val;
	return true;
}
bool fns_push(struct FnStack* fns, BuiltinFn pfn, int8_t pre, size_t nargs) {
	// Check and realloc as needed
	if (fns->len == fns->cap) {
		size_t new_cap = fns->cap ? fns->cap * 3 / 2 : 20;
		fns->stack = (BuiltinFn*)realloc(fns->stack, new_cap * sizeof(BuiltinFn));
		fns->nargs = (size_t*)realloc(fns->nargs, new_cap * sizeof(size_t));
		fns->pres = (int8_t*)realloc(fns->pres, new_cap * sizeof(int8_t));
		if (!fns->stack || !fns->nargs || !fns->pres) {
			return false;
		}
		fns->cap = new_cap;
	}
	// Push the values
	size_t n = fns->len++;
	fns->stack[n] = pfn;
	fns->nargs[n] = nargs;
	fns->pres[n] = pre;
	return true;
}

bool eval_apply(struct Expr* expr) {
	if (expr->fns.len > 0) {
		// Pop fn
		size_t fn = --expr->fns.len;
		// Find its arguments
		size_t nargs = expr->fns.nargs[fn];
		if (nargs > expr->vals.len) {
			expr->err = Expr::Error::InternalCorruption;
			return false;
		}
		value_t* vals = expr->vals.stack + expr->vals.len - nargs;
		// Apply the fn
		value_t result;
		if (!expr->fns.stack[fn](expr->env, vals, nargs, &result)) {
			expr->err = Expr::Error::BadArgument;
			return false;
		}
		// Pop vals and push result
		expr->vals.len -= nargs;
		if (!vals_push(&expr->vals, result)) {
			expr->err = Expr::Error::InternalCorruption;
			return false;
		}
		return true;
	}
	else {
		// No more fns remaining!
		expr->err = Expr::Error::BadArgument;
		return false;
	}
}
bool eval_while(struct Expr* expr, int8_t pre) {
	// Apply fns with a higher precedence
	while (expr->fns.len > 0 && expr->fns.pres[expr->fns.len - 1] >= pre) {
		if (!eval_apply(expr)) {
			return false;
		}
	}
	return true;
}
#define NEXT_OP true
#define NEXT_VAL false
bool eval_val(struct Expr* expr, struct Token* tok);
bool eval_op(struct Expr* expr, struct Token* tok) {
	// Expect an infix operator-like thing
	switch (tok->type) {
	case Token::Type::Operator:
		// Evaulate all lower precedence fns
		if (!eval_while(expr, tok->arg.pre)) {
			return false;
		}
		// Push operator as a function
		if (!fns_push(&expr->fns, tok->arg.fn, tok->arg.pre, 2)) {
			expr->err = Expr::Error::InternalCorruption;
			return false;
		}
		// Followed by a value
		expr->next = NEXT_VAL;
		return true;
	case Token::Type::FnComma:
		// Eval everything up until a function barrier
		if (!eval_while(expr, 2)) {
			return false;
		}
		// Increment the nargs for that function
		if (expr->fns.len == 0) {
			expr->err = Expr::Error::BadArgument;
			return false;
		}
		expr->fns.nargs[expr->fns.len - 1] += 1;
		// Followed by a value
		expr->next = NEXT_VAL;
		return true;
	case Token::Type::Variable:
		// Implicit multiplication? why not!
		if (!eval_op(expr, &token_implicit_mul()) || !eval_val(expr, tok)) {
			return false;
		}
		// Followed by an operator
		expr->next = NEXT_OP;
		return true;
	case Token::Type::FnBegin:
		// Implicit multiplication? why not!
		if (!eval_op(expr, &token_implicit_mul()) || !eval_val(expr, tok)) {
			return false;
		}
		// Followed by a value
		expr->next = NEXT_VAL;
		return true;
	case Token::Type::FnEnd:
		// Eval everything and push past the barrier
		if (!eval_while(expr, 2) || !eval_apply(expr)) {
			return false;
		}
		// Followed by an operator
		expr->next = NEXT_OP;
		return true;
	case Token::Type::Unknown:
		expr->err = Expr::Error::InvalidToken;
		return false;
	default:
		// Anything else is an error
		expr->err = Expr::Error::ExpectedOperator;
		return false;
	}
}
bool eval_val(struct Expr* expr, struct Token* tok) {
	// Expect something that becomes a value eventually
	switch (tok->type) {
	case Token::Type::Literal:
		if (!vals_push(&expr->vals, tok->arg.val)) {
			expr->err = Expr::Error::InternalCorruption;
			return false;
		}
		// Followed by an operator
		expr->next = NEXT_OP;
		return true;
	case Token::Type::Operator:
		// Unary operators have high precedence
		if (!fns_push(&expr->fns, tok->arg.fn, 7, 1)) {
			expr->err = Expr::Error::InternalCorruption;
			return false;
		}
		// Followed by a value
		expr->next = NEXT_VAL;
		return true;
	case Token::Type::Variable: {
		// Error handling, unknown symbol
		if (!tok->arg.fn) {
			expr->err = Expr::Error::UnknownSymbol;
			return false;
		}
		// Just evaluate them now
		value_t result;
		if (!tok->arg.fn(expr->env, nullptr, 0, &result)) {
			expr->err = Expr::Error::BadArgument;
			return false;
		}
		// And push result as a value
		if (!vals_push(&expr->vals, result)) {
			expr->err = Expr::Error::InternalCorruption;
			return false;
		}
		// Followed by an operator
		expr->next = NEXT_OP;
		return true;
	}
	case Token::Type::FnBegin:
		// Error handling, unknown symbol
		if (!tok->arg.fn) {
			expr->err = Expr::Error::UnknownSymbol;
			return false;
		}
		// Low precedence acts as a barrier
		if (!fns_push(&expr->fns, tok->arg.fn, 1, 1)) {
			expr->err = Expr::Error::InternalCorruption;
			return false;
		}
		// Followed by more values
		expr->next = NEXT_VAL;
		return true;
	case Token::Type::FnEnd:
		expr->err = Expr::Error::BadArgument;
		return false;
	case Token::Type::Unknown:
		expr->err = Expr::Error::InvalidToken;
		return false;
	default:
		// Anything else is an error
		expr->err = Expr::Error::NaExpression;
		return false;
	}
}
bool parse(struct Expr* expr, const char* in) {
	// Iterate over the tokens in the in stream
	struct Token tok;
	while (token_lex(expr->env, in, &tok)) {
		// Eval as operator or value
		if (!(expr->next ? eval_op : eval_val)(expr, &tok)) {
			return false;
		}
		// Advance the input
		in += tok.len;
	}
	return true;
}
bool eval(struct Expr* expr, value_t* result) {
	// Expected more input
	if (!expr->next) {
		expr->err = Expr::Error::UnexpectedEos;
		return false;
	}
	// Evaluate all pending fns
	if (!eval_while(expr, 2)) {
		return false;
	}
	// Expect exactly one result
	if (expr->vals.len != 1 || expr->fns.len != 0) {
		expr->err = Expr::Error::UnfinishedExpression;
		return false;
	}
	// Return the result
	*result = expr->vals.stack[0];
	return true;
}

}
