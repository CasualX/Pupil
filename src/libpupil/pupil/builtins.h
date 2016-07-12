#pragma once

// Functions wrapped such that the evaluator can call them.
// Maybe I overdid it a little :)

#include "env.h"

namespace pupil {

#define PUPIL_BUILTINS(λ) λ(id) λ(ans) \
	λ(add) λ(sub) λ(mul) λ(div) λ(rem) λ(pow) λ(floor) λ(ceil) λ(round) \
	λ(abs) λ(sqr) λ(cube) λ(sqrt) λ(cbrt) λ(min) λ(max) λ(gamma) λ(fac) \
	λ(exp) λ(exp2) λ(expm1) λ(ln) λ(log) λ(log2) λ(log10) λ(ln1p) λ(e) \
	λ(mean) λ(median) λ(range) λ(var) λ(stdev) \
	λ(deg) λ(rad) λ(pi) λ(tau) \
	λ(sin) λ(cos) λ(tan) λ(asin) λ(acos) λ(atan) λ(atan2) \
	λ(sinh) λ(cosh) λ(tanh) λ(asinh) λ(acosh) λ(atanh)

#define PUPIL_FN(N) bool builtin_##N(struct Env* env, value_t* vals, size_t nargs, value_t* result)
#define PUPIL_DECL(N) extern PUPIL_FN(N);
#define PUPIL_INIT(N) env_add(env, #N, &builtin_##N);

PUPIL_BUILTINS(PUPIL_DECL)

}
