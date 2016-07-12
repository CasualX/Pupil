#include "stdafx.h"

#include "libpupil/pupil/pupil.h"
using namespace pupil;

int main(int argc, char* args[])
{
#ifdef _MSC_VER
	bool con = _isatty(_fileno(stdin)) != 0;
#else
	bool con = isatty(stdin) != 0;
#endif
	if (con) {
		printf("Welcome to Pupil, the expression evaluator!\n");
		if (argc <= 1) {
			printf("\n\
Enter an expression, eg. 2 + 3, and press enter.\n\
Press ctrl-C to exit.\n\
\n\
Built-in functions:\n\
  +-*/^   : Operators with correct precedence.\n\
  (expr)  : Group expression with parentheses.\n\
  ans     : Use answer from previous expression.\n\
  pi, tau : Trigonometric constants.\n\
  e       : Euler's number.\n\
  add, sub, mul, div, rem, pow, floor, ceil, round,\n\
  abs, sqr, cube, sqrt, cbrt, min, max, gamma, fac,\n\
  exp, expm1, ln, log, log2, log10, ln1p,\n\
  mean, median, range, var, stdev,\n\
  deg, rad, sin, cos, tan, asin, acos, atan, atan2,\n\
  sinh, cosh, tanh, asinh, acosh, atanh\n\
          : Use parens to provide arguments.\n\
\n");
		}
	}

	// Initialize the environment
	struct Env env = {};
	env_init(&env);

	if (argc > 1) {
		// Eval args from command line
		struct Expr expr = { &env };
		for (int i = 1; i < argc; ++i) {
			if (!parse(&expr, args[i])) {
				goto error;
			}
		}
		if (eval(&expr, &env.ans)) {
			printf("Ok: %.*g\n", 15, env.ans);
			return 0;
		}
		else {
		error:
			fprintf(stderr, "Err: %s!\n", ERROR_DESC[expr.err]);
			return expr.err;
		}
	}
	else {
		char buf[2048];
		while (true) {
			// If user is at a console, print a nice REPL
			if (con) {
				printf(">>> ");
			}
			// Get string from stdin, 2K ought to be enough for anybody
			if (fgets(buf, sizeof(buf), stdin) != buf) {
				return 0;
			}
			// Evaluate the expression
			struct Expr expr = { &env };
			if (parse(&expr, buf) && eval(&expr, &env.ans)) {
				printf("%.*g\n", 15, env.ans);
			}
			else {
				fprintf(stderr, "Err: %s!\n", ERROR_DESC[expr.err]);
				// Fail fast when not from a console
				if (!con) {
					return expr.err;
				}
			}
		}
	}
}
