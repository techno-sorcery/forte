# Forte

Forte is an stack-based programming language implemented in C++. It is inspired by Forth, but it uses its own type system, syntax, module system, runtime model, and planned bytecode compilation + execution.

The project is currently under active development.

## Overview

Forte programs are made of whitespace-separated tokens. Literal values are pushed onto a stack, and words operate on values from that stack.

Example:

```forte
1 2 + .#
```

This pushes `1` and `2`, adds them, and prints the result as a number.

Forte currently supports:

* Stack-based execution
* Native C++ modules loaded with `dlopen`
* Runtime symbol scopes
* Variables backed by runtime data memory
* Prefix words such as `def`, `if`, `while`, `for`, `rep`, `var`, `cast`, and `import`
* User-defined words
* Basic arithmetic, bitwise, comparison, stack, data, and I/O words

## Primitive Types

Forte natively supports the following primitive types which, for ease of manipulation, take up 8 bytes each on the stack.

| Marker | Name | Meaning       |
| ------ | ---- | ------------- |
| `#`    | num  | double number |
| `$`    | ptr  | pointer       |
| `%`    | word | raw bitfield  |
| `@`    | char | character     |

Each type has its own marker, which is used in casts, documentation, and stack-effect comments.

### Variables

```forte
var x
10 x <-
x -> .#
```

Variables evaluate to pointers. Use `<-` to store and `->` to fetch.

### Characters and Strings

```forte
'a' .@
"hello" print
```

Strings are represented as stack strings terminated by a null character.

### User-defined Words

```forte
def square
    dup *
done

5 square .#
```

User functions are defined using the "def" prefix.

### Repetition

```forte
3 rep
    "hi" print \n
done
```

### If / Else

```forte
1 if
    "true" print
else
    "false" print
done
```

### While

```forte
while
    condition
do
    body
done
```

### For

```forte
for
    init
cond
    condition
step
    update
do
    body
done
```

## Comments

Forte supports line comments and parenthesized comments.

```forte
; this is a line comment

( this is an inline comment )
```

## Core Words

### Arithmetic

```forte
+ - * /
```

### Bitwise

```forte
& | ^ ~ << >>
```

### Comparison and Logic

```forte
> < = != <= >=
&& || !
```

### Stack Manipulation

| Word    | Effect                                    |
| ------- | ----------------------------------------- |
| `drop`  | remove the top value                      |
| `dup`   | duplicate the top value                   |
| `<>`    | swap the top two values                   |
| `over`  | copy the second value to the top          |
| `rot`   | rotate the top three values               |
| `nip`   | remove the second value                   |
| `tuck`  | copy the top value under the second value |
| `drop2` | drop the top two values                   |
| `dup2`  | duplicate the top two values              |

### Data

| Word | Effect                       |
| ---- | ---------------------------- |
| `<-` | store a value at a pointer   |
| `->` | fetch a value from a pointer |

### I/O

| Word      | Effect                           |
| --------- | -------------------------------- |
| `in`      | read one character               |
| `.#`      | print a number                   |
| `.@`      | print a character                |
| `.$`      | print a pointer                  |
| `.%`      | print a word / bit value         |
| `print`   | print a stack string             |
| `println` | print a stack string and newline |
| `\n`      | print newline                    |
| `\t`      | print tab                        |
| `cat`     | concatenate stack strings        |
| `flush`   | flush standard output            |

## Architecture

Forte is split into interpreter components and native modules.

### `State`

`State` coordinates execution. It owns or references:

* a `Runtime`
* a `Scope`
* parent state information
* prefix parsing context
* local data-base cleanup information

`State` handles token evaluation, symbol resolution, user function calls, prefix handling, and interaction with the runtime stack/data.

### `Runtime`

`Runtime` owns execution data:

* stack
* data memory
* loaded module handles

It provides stack operations, data allocation, pointer validation, and module-handle cleanup.

### `Scope`

`Scope` stores symbols and supports parent-scope lookup.

Symbols may refer to:

* native functions
* prefix functions
* data pointers
* user-defined token bodies

`Scope` also tracks block prefixes that consume tokens until `done`.

### `Loader`

The loader handles:

* Forte source file loading (libraries)
* native module loading through `dlopen`
* calling module initialization functions with `dlsym`

### Helpers

Helpers currently include tokenization and type conversion utilities.

The tokenizer supports:

* whitespace-separated tokens
* quoted strings
* quoted characters
* escape sequences
* `;` line comments
* `( ... )` comments

## Modules

Forte loads native modules from shared objects.

Current modules include:

```text
modules/prefix.so
modules/core.so
modules/io.so
```

### Core Module

The core module registers arithmetic, bitwise, comparison, logic, stack, control, and data words.

Example native registration:

```cpp
extern "C" void forte_init_module(forte::State* state) {
    state->newEntry("+", add);
    state->newEntry("-", sub);
}
```

### Prefix Module

The prefix module registers prefix words such as:

```forte
for
while
rep
if
cast
var
def
import
```

Block prefixes are registered so nested `done` depth can be tracked correctly.

### I/O Module

The I/O module registers input and output words such as:

```forte
in
.#
.@
.$
.%
print
println
\n
\t
cat
flush
```

## Running

Interactive mode:

```sh
./forte
```

Run a file:

```sh
./forte program.frte
```
