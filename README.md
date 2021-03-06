# Martin Language
This is a statically typed language with a focus on memory safety. Support for
calling C and C++ functions are also, as well as creating functions that can be
called from C and C++, is supported with externs. It also features a package
manager and a strict versioning system for libraries. Ease of development is also
a big goal with support for being compiled, JIT compiled, and interpreted.
Lastly, it supports zero cost returns of any sized type.

## Currently implemented
* Parser
* Tokenizer
* Unicode conversion
* Verifier

## Currently working on
* Project manager
* Package manager

## Future goals
* Interpreter
* Code generation using LLVM
* Standard library
* Code simplification of hi level abstractions to low level concepts