#include "builtins.h"

namespace pupil {

#define PUPIL_BUILTIN_0(N, C) PUPIL_FN(N) { if (nargs == 0) { *result = C; return true; } return false; }
#define PUPIL_BUILTIN_1(N, F) PUPIL_FN(N) { if (nargs == 1) { *result = (F)(vals[0]); return true; } return false; }
#define PUPIL_BUILTIN_2(N, F) PUPIL_FN(N) { if (nargs == 2) { *result = (F)(vals[0], vals[1]); return true; } return false; }

PUPIL_FN(id) {
	if (nargs == 1) {
		*result = vals[0];
		return true;
	}
	return false;
}
PUPIL_BUILTIN_0(ans, env->ans);
PUPIL_FN(add) {
	if (nargs > 0) {
		value_t tmp = vals[0];
		for (size_t i = 1; i < nargs; ++i) {
			tmp += vals[i];
		}
		*result = tmp;
		return true;
	}
	return false;
}
PUPIL_FN(sub) {
	if (nargs == 1) {
		*result = -vals[0];
		return true;
	}
	if (nargs == 2) {
		*result = vals[0] - vals[1];
		return true;
	}
	return false;
}
PUPIL_FN(mul) {
	if (nargs >= 2) {
		value_t tmp = vals[0];
		for (size_t i = 1; i < nargs; ++i) {
			tmp *= vals[i];
		}
		*result = tmp;
		return true;
	}
	return false;
}
PUPIL_FN(div) {
	if (nargs == 2) {
		*result = vals[0] / vals[1];
		return true;
	}
	return false;
}
PUPIL_BUILTIN_2(rem, ::fmod)
PUPIL_BUILTIN_2(pow, ::pow)

PUPIL_BUILTIN_1(floor, ::floor)
PUPIL_BUILTIN_1(ceil, ::ceil)
PUPIL_BUILTIN_1(round, ::round)
PUPIL_BUILTIN_1(abs, ::abs)
PUPIL_FN(sqr) {
	if (nargs == 1) {
		*result = vals[0] * vals[0];
		return true;
	}
	return false;
}
PUPIL_FN(cube) {
	if (nargs == 1) {
		*result = vals[0] * vals[0] * vals[0];
		return true;
	}
	return false;
}
PUPIL_BUILTIN_1(sqrt, ::sqrt)
PUPIL_BUILTIN_1(cbrt, ::cbrt)
PUPIL_BUILTIN_1(gamma, ::tgamma)
PUPIL_FN(fac) {
	if (nargs == 1) {
		value_t val = vals[0];
		// Get exact result for certain values
		int ival = (int)val;
		if (val > 0 && val < 44 && abs(val - (value_t)ival) < (value_t)0.00001) {
			value_t acc = (value_t)ival;
			value_t it = (value_t)ival;
			while (--ival > 1) {
				it -= 1.0;
				acc *= it;
			}
			*result = acc;
		}
		// Otherwise approximate using gamma function
		else {
			*result = ::tgamma(val + 1);
		}
		return true;
	}
	return false;
}
PUPIL_BUILTIN_1(exp, ::exp)
PUPIL_BUILTIN_1(exp2, ::exp2)
PUPIL_BUILTIN_1(expm1, ::expm1)
PUPIL_BUILTIN_1(ln, ::log)
PUPIL_FN(log) {
	if (nargs == 2) {
		*result = ::log(vals[0]) / ::log(vals[1]);
		return true;
	}
	return false;
}
PUPIL_BUILTIN_1(log2, ::logb)
PUPIL_BUILTIN_1(log10, ::log10)
PUPIL_BUILTIN_1(ln1p, ::log1p)
PUPIL_BUILTIN_0(e, (value_t)M_E)
PUPIL_FN(min) {
	if (nargs > 0) {
		value_t tmp = vals[0];
		for (size_t i = 1; i < nargs; ++i) {
			tmp = ::fmin(tmp, vals[i]);
		}
		*result = tmp;
		return true;
	}
	return false;
}
PUPIL_FN(max) {
	if (nargs > 0) {
		value_t tmp = vals[0];
		for (size_t i = 1; i < nargs; ++i) {
			tmp = ::fmax(tmp, vals[i]);
		}
		*result = tmp;
		return true;
	}
	return false;
}
PUPIL_FN(mean) {
	if (builtin_add(env, vals, nargs, result)) {
		*result /= (value_t)nargs;
		return true;
	}
	return false;
}
PUPIL_FN(median) {
	if (nargs > 0) {
		// Need sorted arguments...
		qsort(vals, nargs, sizeof(value_t), [](const void* _a, const void* _b) {
			const value_t* lhs = (const value_t*)_a;
			const value_t* rhs = (const value_t*)_b;
			if (*lhs < *rhs) {
				return -1;
			}
			else if (*lhs > *rhs) {
				return 1;
			}
			else {
				return 0;
			}
		});
		// Odd nargs
		if (nargs & 1) {
			*result = vals[nargs >> 1];
		}
		// Even nargs
		else {
			*result = (vals[nargs >> 1] + vals[(nargs >> 1) - 1]) / 2;
		}
		return true;
	}
	return false;
}
PUPIL_FN(range) {
	if (nargs > 0) {
		value_t min = vals[0], max = vals[0];
		for (size_t i = 1; i < nargs; ++i) {
			if (vals[i] < min) {
				min = vals[i];
			}
			else if (vals[i] > max) {
				max = vals[i];
			}
		}
		*result = max - min;
		return true;
	}
	return false;
}
PUPIL_FN(var) {
	value_t mean;
	if (builtin_mean(env, vals, nargs, &mean)) {
		value_t var = 0.;
		for (size_t i = 1; i < nargs; ++i) {
			value_t dev = vals[i] - mean;
			var += dev * dev;
		}
		*result = var;
		return true;
	}
	return false;
}
PUPIL_FN(stdev) {
	if (builtin_var(env, vals, nargs, result)) {
		*result = sqrt(*result);
		return true;
	}
	return false;
}
PUPIL_FN(deg) {
	if (nargs == 1) {
		*result = vals[0] * (value_t)(180.0 / M_PI);
		return true;
	}
	return false;
}
PUPIL_FN(rad) {
	if (nargs == 1) {
		*result = vals[0] * (value_t)(M_PI / 180.0);
		return true;
	}
	return false;
}
PUPIL_BUILTIN_0(pi, (value_t)M_PI)
PUPIL_BUILTIN_0(tau, (value_t)(M_PI + M_PI))
PUPIL_BUILTIN_1(sin, ::sin)
PUPIL_BUILTIN_1(cos, ::cos)
PUPIL_BUILTIN_1(tan, ::tan)
PUPIL_BUILTIN_1(asin, ::asin)
PUPIL_BUILTIN_1(acos, ::acos)
PUPIL_BUILTIN_1(atan, ::atan)
PUPIL_BUILTIN_2(atan2, ::atan2)
PUPIL_BUILTIN_1(sinh, ::sinh)
PUPIL_BUILTIN_1(cosh, ::cosh)
PUPIL_BUILTIN_1(tanh, ::tanh)
PUPIL_BUILTIN_1(asinh, ::asinh)
PUPIL_BUILTIN_1(acosh, ::acosh)
PUPIL_BUILTIN_1(atanh, ::atanh)

}
