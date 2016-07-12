#include "token.h"
#include "builtins.h"

namespace pupil {

bool token_lex_op(const char* in, struct Token* tok) {
	switch (in[0]) {
	case '+':
		tok->arg.pre = 3;
		tok->arg.fn = &builtin_add;
		break;
	case '-':
		tok->arg.pre = 3;
		tok->arg.fn = &builtin_sub;
		break;
	case '*':
		tok->arg.pre = 4;
		tok->arg.fn = &builtin_mul;
		break;
	case '/':
		tok->arg.pre = 4;
		tok->arg.fn = &builtin_div;
		break;
		// Implicit multiply gets precedence 5
		// This makes it clingy to division allowing eg. 1/2ans
	case '^':
		tok->arg.pre = 6;
		tok->arg.fn = &builtin_pow;
		break;
	default:
		return false;
	}
	tok->type = Token::Type::Operator;
	tok->len += 1;
	return true;
}
bool token_lex_lit(const char* in, struct Token* tok) {
	char* end_ptr;
	tok->arg.val = (value_t)strtod(in, &end_ptr);
	if (in != end_ptr) {
		tok->type = Token::Type::Literal;
		tok->len += end_ptr - in;
		return true;
	}
	return false;
}
bool token_lex_fnbegin(struct Env* env, const char* in, struct Token* tok) {
	// Search for an identifier
	size_t len = 0;
	while ((in[len] >= 'a' && in[len] <= 'z') ||
		(in[len] >= 'A' && in[len] <= 'Z') ||
		(in[len] >= '0' && in[len] <= '9')) {
		++len;
	}
	// Find matching symbol in env, nullptr is handled upstream
	BuiltinFn pfn;
	if (len == 0) {
		// Special case for identity fn
		if (in[len] != '(') {
			return false;
		}
		pfn = env_find(env, "id", 2);
	}
	else {
		pfn = env_find(env, in, len);
	}
	// Variable is a function that takes no arguments
	tok->type = in[len] == '(' ? (++len, Token::Type::FnBegin) : Token::Type::Variable;
	tok->len += len;
	tok->arg.fn = pfn;
	return true;
}
bool token_lex_fnend(const char* in, struct Token* tok) {
	if (in[0] == ')') {
		tok->type = Token::Type::FnEnd;
		tok->len += 1;
		return true;
	}
	if (in[0] == ',') {
		tok->type = Token::Type::FnComma;
		tok->len += 1;
		return true;
	}
	return false;
}

bool token_lex(struct Env* env, const char* in, struct Token* tok) {
	// Skip whitespace and check eos
	tok->len = 0;
	while (in[tok->len] == ' ') ++tok->len;
	if (!in[tok->len] || in[tok->len] == '\n') {
		return false;
	}
	in += tok->len;
	// Try parsing as an operator
	if (token_lex_op(in, tok)) {
		return true;
	}
	// Try parsing as a double literal
	if (token_lex_lit(in, tok)) {
		return true;
	}
	// Try parsing as a function end or comma
	if (token_lex_fnend(in, tok)) {
		return true;
	}
	// Try parsing as a function begin
	if (token_lex_fnbegin(env, in, tok)) {
		return true;
	}
	// This is an error handled upstream
	tok->type = Token::Type::Unknown;
	return true;
}

struct Token token_implicit_mul() {
	// Very clingy multiply but lower than pow^
	struct Token tok = { Token::Type::Operator };
	tok.arg.fn = &builtin_mul;
	tok.arg.pre = 5;
	tok.len = 0;
	return tok;
}

}
