# BigInteger — Software Requirements Specification

This document defines the proposed behavior and scope of the CSC 481 BigInteger project for team discussion. Numbered
requirements identify the intended product behavior. **Required** features form the core deliverable; **optional**
features are extensions.

## 1. Overview

C++ built-in integer types have fixed ranges. This project provides a reusable arbitrary-precision signed integer
library, `BigInt`, and a command-line calculator, `EvaluatorDemo`, that demonstrates its operations.

| Component | Scope | Purpose |
|---|---|---|
| `BigInt` | **Required** | Signed arbitrary-precision integers, arithmetic, conversions, comparisons, and formatting. |
| `EvaluatorDemo` | **Required** | Interactive integer calculator with named variables and display commands. |
| `BigInt_Math` | **Optional** | Additional integer mathematical functions and calculator integration. |
| Calculator fractional-literal mode | **Optional** | Limited floating-point intermediate arithmetic with integer results. |

The core `BigInt` library must be independently usable. The evaluator and optional math module depend on it. A GUI,
persistent calculator variables, a full expression parser with precedence or parentheses, and an arbitrary-precision
fractional type are outside the required scope.

**Terminology.** A *limb* represents one base-`10^18` magnitude digit, indexed from zero at the least significant limb.
*Canonical zero* is nonnegative and has exactly one zero limb. `ans` is a predefined calculator result variable,
initially zero, with a case-insensitive name.

## 2. User requirements

| ID | Requirement |
|---|---|
| `UR-001` | A developer shall be able to represent positive, negative, and zero integers larger than the limits of ordinary built-in integer types. |
| `UR-002` | A developer shall be able to perform familiar exact integer calculations and comparisons using these values. |
| `UR-003` | A developer shall be able to convert between human-readable decimal representations and BigInt values. |
| `UR-004` | If `BigInt_Math` is delivered, a developer shall be able to use additional integer mathematical functions through a separate module. |
| `UR-005` | A calculator user shall be able to evaluate a single signed integer literal, a variable reference, or a simple arithmetic expression with up to two operands and at most one supported binary operator. |
| `UR-006` | A calculator user shall be able to save a result in a named variable and use that variable in later expressions. |
| `UR-007` | A calculator user shall be able to use the predefined, case-insensitive `ans` variable as an operand or standalone reference, but not as an assignment or mutation target. |
| `UR-008` | A calculator user shall receive an understandable error and continue using the calculator after invalid input or arithmetic failure. |

## 3. BigInt requirements

### 3.1 Values, construction, and decimal representation

| ID | Requirement |
|---|---|
| `FR-BI-001` | BigInt shall represent nonnegative and negative integers and zero without a fixed application-defined digit limit, subject to resource availability. |
| `FR-BI-002` | BigInt shall preserve exact integer values during supported calculations, without silent overflow of the mathematical result. |
| `FR-BI-003` | BigInt shall be constructible from standard signed and unsigned integral values that its public interface supports. |
| `FR-BI-004` | BigInt shall convert an accepted decimal numeric string to its integer part by truncating its fractional part toward zero, preserving the signed whole-number part exactly. |
| `FR-BI-005` | Decimal output shall represent the exact mathematical value without scientific notation or leading zeroes; zero shall be written as `0`. |
| `FR-BI-006` | BigInt values shall be printable to a standard output stream in decimal form. |
| `FR-BI-007` | Nonempty malformed decimal numeric strings shall be rejected in full with `std::invalid_argument`; they shall not be silently partially parsed. Empty and boundary-whitespace-only strings are the explicit zero-conversion exception in FR-BI-033, not valid numeric tokens. An invalid string assignment shall leave the receiving BigInt unchanged, in accordance with FR-BI-040. |
| `FR-BI-008` | Mathematical zero shall have the same observable value under comparison, conversion, and negation regardless of the form used to construct it (including `-0`). |

**Decimal text syntax.** Valid numeric text permits an optional sign, leading zeroes, one optional decimal point, and
leading/trailing ASCII spaces or tabs. The input is interpreted as an integer by truncating the fractional part toward
zero. For example, `" -00123.45 "` converts to `-123`, `.5` converts to `0`, and `5.` converts to `5`. Embedded
whitespace, commas, exponent syntax, multiple decimal points, and sign-only tokens are rejected. Nonempty input must be
validated in full.

Empty or ASCII-whitespace-only strings are invalid *numeric tokens*, but direct string construction and assignment are
special-cased to produce zero. Stream extraction requires an actual whitespace-delimited token; missing or invalid
tokens set the stream failure state and preserve the destination.

### 3.2 Arithmetic and comparisons

| ID | Requirement |
|---|---|
| `FR-BI-009` | BigInt shall support exact binary addition. |
| `FR-BI-010` | BigInt shall support exact binary subtraction. |
| `FR-BI-011` | BigInt shall support exact binary multiplication. |
| `FR-BI-012` | BigInt shall support integer quotient calculation for a nonzero divisor. |
| `FR-BI-013` | BigInt shall support integer remainder calculation for a nonzero divisor. |
| `FR-BI-014` | BigInt shall support all six numerical comparisons (`==`, `!=`, `<`, `<=`, `>`, `>=`). For BigInt-to-BigInt comparisons and each supported built-in integer, floating-point, and decimal-string operand category, expressions shall be supported with the BigInt on either side of the operator. Reversing the operand order shall preserve the same numerical relation (including unordered cases), with relational direction applied correctly. |
| `FR-BI-015` | BigInt shall support unary arithmetic negation. |
| `FR-BI-016` | BigInt shall support a boolean interpretation of its value, false exactly for zero and true otherwise. |
| `FR-BI-017` | BigInt shall support updating an existing value through compound arithmetic (`+=`, `-=`, `*=`, `/=`, `%=`) with BigInt operands. The same operators shall be usable with supported built-in integral operands, consistent with the integer-conversion behavior, without requiring a particular direct-overload implementation. |
| `FR-BI-018` | Division or remainder by zero shall throw a documented exception rather than produce a numeric result. If the operation modifies a BigInt, its prior value shall be preserved on failure under FR-BI-040. |

**Signed division.** The quotient truncates toward zero; the nonzero remainder has the sign of the dividend. Thus
`-17 / 5 = -3`, `-17 % 5 = -2`, and `a = (a / b) * b + (a % b)` for nonzero `b`.

### 3.3 Shifts

| ID | Requirement |
|---|---|
| `FR-BI-019` | BigInt shall support left shifts by nonnegative counts supplied as supported built-in integral values or BigInt values, producing the exact mathematical result of multiplication by `2^n` whenever sufficient system resources are available. Counts shall not be restricted by the width of a built-in integer or underlying limb. BigInt shall add no application-defined maximum shift count or result size; underlying resource/container failures may propagate. A zero value shifted left by any valid count remains zero. |
| `FR-BI-020` | BigInt shall support right shifts by nonnegative counts supplied as supported built-in integral values or BigInt values. The result shall be exact floor division by `2^n`, including negative values, without fixed-width shift-count undefined behavior. Counts beyond a built-in type's range remain valid; for sufficiently large `n`, nonnegative values yield `0` and negative values yield `-1`. BigInt shall not reject an otherwise valid right shift merely because the count cannot fit into a built-in integer or container index type. |
| `FR-BI-021` | BigInt shall support compound left- and right-shift assignments with the same count categories, mathematical results, and input-error behavior as the nonmutating shifts. A negative count shall throw a documented exception without modifying the destination under FR-BI-040. |

A shift count measures **binary bit positions**, not decimal limbs: `a << n` is multiplication by `2^n`, and `a >> n` is
floor division by `2^n`; for example, `-3 >> 1 = -2`. Counts and results have no additional library-defined size limits.
Extremely large right-shift counts remain valid even when they exceed native integer ranges.

### 3.4 Value semantics

| ID | Requirement |
|---|---|
| `FR-BI-022` | Copying a BigInt shall produce an independently usable value with the same mathematical value as the source. |
| `FR-BI-023` | Assigning a new value to an existing BigInt shall replace its previously represented value. |
| `FR-BI-024` | A default-constructed BigInt shall have mathematical value zero. |

### 3.5 Additional operations, formatting, and limb access

| ID | Requirement |
|---|---|
| `FR-BI-025` | BigInt shall support prefix and postfix increment (`++`) and decrement (`--`), each changing the represented value by exactly one. A prefix operation shall yield the updated object as a modifiable lvalue; a postfix operation shall yield the independent previous value while leaving the operand incremented/decremented. Behavior shall remain exact for zero, negative values, and decimal-limb boundary transitions, without fixed-width overflow. |
| `FR-BI-026` | Unary plus shall yield the unchanged integer value. |
| `FR-BI-027` | Input-stream extraction shall follow conventional formatted whitespace-delimited token extraction, allowing multiple adjacent tokens to be read separately (e.g. `12 34` as two successive BigInt values). Each extracted token shall use the same valid numeric syntax and integer conversion rules as nonempty decimal-string construction. When no token can be obtained or a token is malformed, extraction shall set the stream failure state (`failbit`) without changing the destination BigInt; an empty stream shall not be interpreted as the zero-valued empty-string conversion exception. Input may have been consumed. A stream configured to throw on failure may do so according to standard C++ stream semantics. |
| `FR-BI-028` | An existing BigInt shall be assignable from supported integer, floating-point and decimal-string values as well as another BigInt, preserving documented conversion behavior. Rejection of invalid input shall preserve its original value; other exceptional failures follow FR-BI-040. |
| `FR-BI-029` | Copying, moving, and reassigning a BigInt shall preserve externally correct value behavior, including self-assignment and in-place arithmetic with aliased operands (`a += a`, `a *= a`). |
| `FR-BI-030` | The integer result of converting a finite built-in floating-point number shall be the mathematical truncation toward zero **of the floating-point value actually received**. |
| `FR-BI-031` | Construction or assignment from NaN or positive/negative infinity shall throw `std::domain_error`, not produce a BigInt value. A failed assignment shall preserve the previous value under FR-BI-040. |
| `FR-BI-032` | BigInt shall provide scientific-format output with caller-selected positive maximum significant-digit precision `p` (calculator default 20), rounding to nearest with ties away from zero. Compact output for nonzero values shall preserve all original digits when `digitCount <= p`, without padding, or otherwise round to `p` digits and remove trailing mantissa zeroes after rounding. A separate forced-width mode shall instead output exactly `p` mantissa digits (padding with zeroes if necessary). Formatting shall use lowercase `e`, an explicitly signed exponent without leading zeroes, and a leading minus sign for negative values. Formatting shall never modify the stored integer. |
| `FR-BI-033` | Construction and assignment from an empty or ASCII-space/tab-only string shall produce zero as a special conversion case, although such text is not a syntactically valid numeric token. Strings with `.5` and `5.` are valid numeric representations, while comma-grouped strings are rejected. Empty or whitespace-only strings used as numeric comparison operands are invalid under FR-BI-036. |
| `FR-BI-034` | BigInt values shall be comparable by numerical value with supported built-in signed and unsigned integer types, with either operand on the left, without narrowing or rounding the BigInt value. This includes unsigned values larger than the maximum signed value of their width. |
| `FR-BI-035` | BigInt shall provide equality and numerical ordering against supported built-in floating-point values with either operand on the left, without discarding a nonzero fractional part. For NaN, equality and all four ordering operators return false while inequality returns true in either order; positive infinity is greater than every BigInt; negative infinity is less than every BigInt. |
| `FR-BI-036` | BigInt shall provide equality and numerical ordering against valid decimal numeric strings with either operand on the left, considering any nonzero fractional digits in the string. When either operand is an invalid decimal numeric string (including empty or ASCII-whitespace-only text), `==` shall return `false`, `!=` shall return `true`, and each of `<`, `<=`, `>`, `>=` shall return `false` without throwing due to invalid numeric syntax. Such an operand is treated as unordered. Construction and assignment from a malformed nonempty string still throw under FR-BI-007; empty/whitespace-only construction and assignment still produce zero under FR-BI-033. |
| `FR-BI-037` | BigInt shall expose indexed read-only limb inspection and the count of currently stored limbs. Limb index `0` shall represent the least significant base-`10^18` magnitude digit; each limb value shall be within `[0, 10^18)`. Reading at or beyond the current limb count shall return `0` without modifying the BigInt. A zero BigInt shall report exactly one stored limb, with value `0`. |
| `FR-BI-038` | BigInt shall permit indexed modification of magnitude limbs with values in `[0, 10^18)`, throwing a documented exception for a value outside this range without modifying the original BigInt. Setting a limb beyond the stored extent to `0` shall be a no-op; setting it to a nonzero valid value shall extend the magnitude with zero-valued intervening limbs. After each successful modification, redundant most-significant zero limbs shall be removed. A nonzero resulting magnitude shall preserve the prior sign, except that a BigInt whose previous magnitude was zero shall become positive when a nonzero limb is set. A zero resulting magnitude shall have exactly one zero limb and a nonnegative sign, including when its only limb was changed to `0`. |
| `FR-BI-039` | BigInt shall report the exact count of decimal digits in its magnitude where that count is representable, reporting one digit for zero and zero as a sentinel when the count overflows the public count type. |
| `FR-BI-040` | When a BigInt operation rejects an invalid operand or violates a documented input/domain precondition (such as malformed numeric text, nonfinite floating-point conversion, division/remainder by zero, a negative shift count, or an invalid limb value), it shall report the documented error without changing an existing destination BigInt. For other exceptions, including allocation/resource failures during a mutating operation, the destination shall remain valid, destructible, assignable, and in its canonical representation, but its prior mathematical value is not guaranteed. A failed constructor produces no usable object. This requirement specifies observable guarantees, not a requirement for defensive copies, rollback buffers, or any particular algorithm. |

**Mixed comparisons.** BigInt compares numerically with supported integral values, floating values, and decimal strings
in either operand order. A fractional comparison operand is *not* truncated: `BigInt(12) < "12.9"` is true, and
`BigInt(12) == "12.9"` is false. A valid string such as `"+0.000"` compares equal to zero. An invalid string, including
`""` or `"   "`, compares as unordered: `==` false, `!=` true, and `<`, `<=`, `>`, `>=` false. Floating NaN is likewise
unordered. Positive infinity is greater than any BigInt; negative infinity is less. Converting either infinity or NaN
*into* BigInt remains invalid.

**Public limbs.** Limb indices begin at zero for the least significant base-`10^18` magnitude digit. A limb setter
changes the magnitude only. A sign is retained if the old and new magnitudes are nonzero; reaching zero clears the
negative sign, and extending a zero value with a nonzero limb makes it positive. Setting a nonzero limb beyond the
current extent inserts zeros between limbs; setting zero beyond the extent does nothing.

**Scientific formatting.** A positive precision `p` limits significant digits; the calculator defaults to 20. Rounding
is to nearest, with exact ties away from zero. In compact mode, all original digits are retained when the number has at
most `p` digits, including its original trailing zeroes. Otherwise, the rounded mantissa has its redundant terminal
zeroes removed. Forced-width mode (`pad`) always outputs exactly `p` mantissa digits.

| Value | Precision | Compact | Forced-width |
|---|---:|---|---|
| `20000` | 10 | `2.0000e+4` | `2.000000000e+4` |
| `20000` | 3 | `2e+4` | `2.00e+4` |
| `999950` | 4 | `1e+6` | `1.000e+6` |
| `0` | 3 | `0.00e+0` | `0.00e+0` |

Scientific notation uses lowercase `e`, an explicit exponent sign, and no exponent leading zeroes. Formatting never
changes the stored integer; exact decimal output never rounds.

**Error guarantees.** Invalid arguments and domain errors are rejected before changing an existing destination. During
unexpected errors, including allocation failures, the object remains canonical, destructible, and reusable, but its
prior numeric value is not guaranteed. Resource exceptions propagate; BigInt adds no artificial memory, digit, or shift
limits.

**Scale demonstration.** Parsing, lossless decimal output, and addition must be demonstrated using operands of at least
**5,000 decimal digits**. This is a verification target, not a guaranteed maximum capacity or performance bound.

Bitwise AND, OR, XOR, and NOT (`&`, `|`, `^`, `~`) are excluded from mandatory functionality. Binary shifts are
required.

## 4. EvaluatorDemo requirements

### 4.1 Expressions and assignments

| ID | Requirement |
|---|---|
| `FR-DEMO-001` | The program shall accept user input and print results in an interactive text-based session. |
| `FR-DEMO-002` | It shall evaluate one signed literal or variable reference, or two signed operands connected by exactly one supported binary operator; at most one unary sign shall be accepted per operand. |
| `FR-DEMO-003` | It shall accept signed decimal integer literals as operands. |
| `FR-DEMO-004` | It shall support the operators `+`, `-`, `*`, `/`, `%`, `<<`, and `>>` using the corresponding BigInt semantics. |
| `FR-DEMO-005` | It shall display a faithful representation of the exact stored result, using the required compact-output policy for large values; display rounding shall not change the stored result, and successful unassigned calculations shall display `ans = <value>`. |
| `FR-DEMO-006` | It shall permit an operand to reference a previously defined named variable. |
| `FR-DEMO-007` | It shall permit an ordinary variable to receive the value of any supported evaluation, including a standalone integer literal, a previously defined variable reference, or an optionally signed single operand, without requiring a binary operator. It shall reject assignment to any case variation of `ans`. |

### 4.2 Variables and session state

| ID | Requirement |
|---|---|
| `FR-DEMO-008` | A variable assigned successfully shall be available to subsequent expressions within the same calculator session and may be reassigned. Ordinary variable names are case-sensitive. |
| `FR-DEMO-009` | `ans` shall be a predefined variable initialized to zero and referenced without regard to letter case. It shall be updated with the value of each successful evaluation that has no assignment target, whether the evaluated input is binary, unary, one literal, or one variable reference. |
| `FR-DEMO-010` | A successfully assigned expression shall not update the value of `ans`. |
| `FR-DEMO-011` | A rejected expression shall not modify existing named variables or `ans`. |
| `FR-DEMO-017` | At session start, `ans` shall hold zero and shall remain unchanged by assignments or failed evaluations. |
| `FR-DEMO-018` | Ordinary variable identifiers shall begin with ASCII A–Z, a–z, or `_` and continue with ASCII letters, decimal digits, or `_`; ordinary names are case-sensitive. All letter-case variants of `ans` identify the same predefined variable and shall be prohibited as targets of user-directed assignment or mutation. |

### 4.3 Output and formatting

| ID | Requirement |
|---|---|
| `FR-DEMO-020` | Automatic display shall use exact plain decimal for integer magnitudes having at most 20 decimal digits and scientific notation with a maximum of 20 significant digits for magnitudes having more than 20 digits; rounding shall be nearest with halfway cases away from zero. Presentation shall not modify stored variables or `ans`. |
| `FR-DEMO-021` | Successful assignments shall print the identifier and result (`name = value`) using the same exact/scientific display policy as ordinary results. |
| `FR-DEMO-023` | Evaluating an existing bare variable name, including any case variation of `ans`, shall display `<name> = <value>` and copy the referenced value into `ans`. An undefined variable name shall produce an error without changing state. Bare `ans` shall use the canonical display label `ans`. |
| `FR-DEMO-028` | Successful unassigned binary expressions, unary expressions, and standalone numeric literals shall display `ans = <value>`. Successful assignments and bare variable-name evaluations shall display `<name> = <value>` using the automatic numeric-display policy. All successful unassigned evaluations update `ans`. |

### 4.4 Commands

Commands start with `@` immediately followed by a case-insensitive name. They accept leading/trailing ASCII spaces or
tabs and whitespace-separated arguments. The `pad` flag is case-insensitive.

| Command | Behavior |
|---|---|
| `@exit` | End the session; end-of-file also exits. |
| `@show name` | Display an existing variable with automatic formatting. |
| `@showdigits name` | Display its exact unrounded decimal value. |
| `@showscientific name [precision] [pad]` | Show scientific notation, default precision 20; `pad` can be used without a precision. |
| `@dumpvars` | Display `ans` first, then ordinary variables alphabetically. |

**Detailed requirements**

| ID | Requirement |
|---|---|
| `FR-DEMO-012` | The program shall support prefixed session commands separate from arithmetic expression syntax, including read-only inspection and session termination. |
| `FR-DEMO-019` | `@exit`, with any capitalization of the command name and no arguments, or end-of-file shall terminate the interactive calculator session; the bare identifier `exit` shall not be interpreted as a command. |
| `FR-DEMO-022` | `@showdigits <identifier>` shall display the complete unrounded decimal value of an existing variable or `ans`, irrespective of magnitude, without modifying any stored value or `ans`. |
| `FR-DEMO-024` | Every calculator command shall start with `@` immediately followed by a case-insensitive command name; ordinary variable identifiers remain case-sensitive except for `ans`. The unprefixed names `show`, `showdigits`, `showscientific`, `dumpvars`, and `exit` remain available as ordinary variable identifiers. |
| `FR-DEMO-026` | `@show <identifier>` shall display a previously defined variable or `ans` using the default automatic numeric formatting without changing stored state. |
| `FR-DEMO-027` | `@showscientific <identifier> [precision] [pad]` shall display an existing variable or `ans` in scientific notation with default maximum significant-digit precision 20 or a specified positive precision. The case-insensitive `pad` option may also appear without a precision argument (`@showscientific <identifier> pad`), in which case precision defaults to 20. Without `pad`, use compact formatting under FR-BI-032; with it, use exactly `p` mantissa digits. The command shall not change any stored state. |
| `FR-DEMO-029` | Variable-targeted display commands `@show`, `@showdigits`, and `@showscientific` shall accept only an existing variable identifier (including any case variant of `ans`), not numeric literals or expressions; invalid names and malformed command arguments shall produce errors without modifying state. |
| `FR-DEMO-030` | `@dumpvars` shall accept no arguments and display the predefined `ans` first, followed by every currently defined ordinary variable in deterministic alphabetical order. Each line shall use the original variable name and automatic numeric formatting (`name = value`). The command shall not modify stored values or `ans`; a fresh session shall display `ans = 0`. |
| `FR-DEMO-031` | Commands shall accept leading/trailing ASCII spaces or tabs and separate arguments with one or more ASCII spaces or tabs. The command prefix `@` must be immediately adjacent to its name; command names and `pad` shall be case-insensitive. Missing arguments, extra or unrecognized arguments, invalid variable references, unsupported flags, nonpositive or unrepresentable precision values, and unknown commands shall be reported as errors without state changes; the session shall continue. |

### 4.5 Input handling and error recovery

| ID | Requirement |
|---|---|
| `FR-DEMO-013` | Malformed expressions shall be rejected with an understandable diagnostic without silently producing an incorrect result. |
| `FR-DEMO-014` | An undefined variable reference shall be reported as an error. |
| `FR-DEMO-015` | An arithmetic failure (such as division by zero) shall be reported and shall not terminate the interactive session. |
| `FR-DEMO-016` | Following a recoverable input or evaluation error, the application shall accept another input. |
| `FR-DEMO-025` | An empty or whitespace-only input line shall be ignored without output or state change. |

### 4.6 Expression grammar

Each statement can evaluate one signed operand, optionally followed by one binary operator and a second signed operand.
An assignment stores any such result in a named variable; a binary operator is not required.

```text
statement              := evaluation | assignment | command
assignment             := assignable_identifier '=' evaluation
evaluation             := signed_operand [binary_operator signed_operand]
signed_operand         := ('+' | '-')? primary
primary                := integer_literal | identifier
binary_operator        := '+' | '-' | '*' | '/' | '%' | '<<' | '>>'
identifier             := [A-Za-z_][A-Za-z0-9_]*
assignable_identifier  := identifier except any case spelling of 'ans'
```

Chained expressions, precedence, parentheses, multiple unary signs on one operand, and fractional/dotted operands are
excluded from the mandatory integer-only evaluator. Exponentiation is not a mandatory operator.

### 4.7 Example session

```text
> 123
ans = 123
> a = 456
a = 456
> b = a
b = 456
> c = -b
c = -456
> ans
ans = 123
> a + b
ans = 912
> @showdigits c
c = -456
> @dumpvars
ans = 912
a = 456
b = 456
c = -456
> @exit
```

## 5. Optional extensions

These features do not form part of the required BigInt or integer-only evaluator deliverables.

### 5.1 BigInt_Math

| Function | Proposed integer-domain behavior |
|---|---|
| `pow` | Integer exponentiation for nonnegative exponents. A convention for `0^0` and negative exponents would be needed. |
| `isqrt` | Integer square root: `floor(sqrt(n))` for nonnegative `n`. |
| `iroot` | Integer n-th root for positive order; for example, `iroot(-9, 3) = -2`. Negative even roots are invalid. |
| `gamma` | Integer-domain gamma: `Gamma(n) = (n-1)!` for positive integers. |
| `factorial` | Nonnegative integer factorial, `n!`. |
| `gcd` | Greatest common divisor, returned nonnegative; `gcd(0,0)` requires a convention. |

If implemented, these functions belong to a separate module based on BigInt. Demonstrating them through the calculator
is optional and would need a defined invocation syntax.

### 5.2 Fractional-literal calculator mode

The optional evaluator extension supports selected decimal-dot literal calculations using `long double` intermediates
and converts their result to BigInt by truncation toward zero. This does not introduce persistent fractional variables
or arbitrary-precision fractional storage.

| Operands | Calculation mode |
|---|---|
| Integer literals and/or BigInt variables | Exact BigInt arithmetic. |
| One BigInt variable and one decimal-dot literal | Reject. |
| Two decimal-dot literals | `long double`. |
| A decimal-dot literal and integer literal within finite `long double` range | `long double`, allowing precision loss. |
| A decimal-dot literal and integer literal outside finite `long double` range | Reject. |

Floating-mode operations would be `+`, `-`, `*`, `/`, and `%` (remainder via `std::fmod`), but not shifts. Nonfinite
values and zero divisors are rejected. Platform-dependent `long double` precision may affect integer results near
truncation boundaries. The handling of nonzero decimal text underflowing to floating zero remains to be specified if
this feature is selected.

## 6. Nonfunctional and project requirements

| ID | Requirement |
|---|---|
| `NFR-001` | Production components shall be implemented in standard C++23. |
| `NFR-002` | Running production libraries or the CLI shall not require third-party runtime dependencies beyond the chosen platform's standard C++ runtime and other ordinary system prerequisites. |
| `NFR-003` | The project shall be configurable and buildable with CMake. |
| `NFR-004` | Test cases shall be executable through CTest, with Google Test permissible as a **development/test dependency**, not a production runtime dependency. |
| `NFR-005` | The team shall validate intended compiler/platform configurations on GCC and Clang for Linux and MSVC for Windows, or document any unsupported configurations honestly. |
| `NFR-006` | Core integer computations shall not silently truncate or wrap mathematical results due to exceeding a fixed built-in integer width. |
| `NFR-007` | Each delivered component shall have documented responsibilities, with `BigInt` independently reusable; `BigInt_Math` remains optional. |
| `NFR-008` | A successful test run shall cover specified arithmetic edge cases, input failures, and calculator session behavior. |
| `NFR-009` | The team shall record work and reviews using GitHub issues and pull requests. |
| `NFR-010` | Each of the five members shall have a clear, substantial and reviewable project contribution. |
| `NFR-011` | Final deliverables shall include a functioning `BigInt`-based calculator, comprehensive documented report, and presentation/demo conforming to templates when published; optional mathematical extensions are not required for baseline acceptance. |
| `NFR-012` | The development team shall maintain an SRS revision history explaining significant changes and record architectural/design rationales separately. |

The project targets C++23 and CMake with Google Test/CTest for testing. No fixed speed, memory-consumption, or
maximum-digit benchmark is required.

## 7. Verification

The requirements will be verified through automated tests and representative calculator sessions, supplemented by
artifact inspection and documented reviews where appropriate. Detailed test cases, traceability, and execution evidence
are kept separate from this specification.
