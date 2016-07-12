Pupil
=====

Pupil is a pretty simple command line calculator written in C-ish C++.

Compare with its [Rust implementation](https://github.com/CasualX/pupil-rs).

Written for a contest that is pretty much suspended indefinitely, so publishing it early anyway.

Building
--------

Developed, built and tested on windows, it may or may not compile on posix compatible systems.

Using Visual Studio, open and build the pupil.sln solution.

Any other compiler, feed the files `src/main.cpp src/stdafx.cpp` to a C++ compiler.

Usage
-----

The executable can be invoked in three ways:

* Run without any arguments.

  Prints a description and help instructions and starts a REPL.

  Simply enter expressions and press enter to evaluate them.

  ```text
  C:\Projects\Pupil>x64\Release\pupil
  Welcome to Pupil, the expression evaluator!

  Enter an expression, eg. 2 + 3, and press enter.
  Press ctrl-C to exit.

  >>> 2 - 3*4
  10.000000
  >>> ^C
  ```

* Pass any command line arguments.

  Prints a description, evaluates the arguments as a single expression and prints the result.

  ```text
  C:\Projects\Pupil>x64\Release\pupil 2 - 3*4
  Welcome to Pupil, the expression evaluator!
  Ok: -10.000000
  ```

* Pipe input.

  Every line in the input is treated as an expression and evaluated.

  Prints just the answers for every line of input without any fancy formatting or introductions.

  When an error is encountered, the evaluation stops and a non-zero error code is returned.

  ```text
  C:\Projects\Pupil>echo 2 - 3*4 | x64\Release\pupil
  -10.000000
  ```

Calculator
----------

The calculator supports the operators `+` `-` `*` `/` and `^` with their appropriate precedence rules.

The result of the previous successful expression evaulation can be used with `ans` as a value.
Also available are the constants `e`, `pi` and `tau`.

Implicit multiplication is also supported, its precedence lies between `/` and `^`
such that `1/2ans` is equivalent to `1/(2ans)`. Regular multiplication is unaffected.

Group expressions with `(` and `)`, this is actually an invocation of the identity function.
Functions start with an identifier followed by parentheses, arguments are separated by `,`.

The following is a list of all functions available within the pupil program:

`add`, `sub`, `mul`, `div`, `rem`, `pow`, `floor`, `ceil`, `round`,
`abs`, `sqr`, `cube`, `sqrt`, `cbrt`, `min`, `max`, `gamma`, `fac`
`exp`, `expm1`, `ln`, `log`, `log2`, `log10`, `ln1p`,
`mean`, `median`, `range`, `var`, `stdev`,
`deg`, `rad`, `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `atan2`,
`sinh`, `cosh`, `tanh`, `asinh`, `acosh`, `atanh`.

License
-------

MIT - see license.txt
