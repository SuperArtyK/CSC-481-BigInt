# BigInteger — Software Requirements Specification

Numbered requirements define the CSC 481 BigInteger project. **Required** features form the core deliverable;
**optional** features are extensions.

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

**Terminology.** A *limb* represents one base-`10^18` magnitude digit, indexed from zero at the least-significant limb.
A *normalized* BigInt has at least one limb, no redundant most-significant zero limbs, and a nonnegative sign if its
magnitude is zero. *Canonical zero* contains exactly one zero limb and has a nonnegative sign. A *non-normalized*
representation does not meet these conditions; it may be retained indefinitely. Empty storage is a valid C++ object
state but not a normalized numerical representation. The operations permitted on these states are defined in §3.6. `ans`
is a predefined calculator result variable, initially zero, with a case-insensitive name.

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
| `FR-BI-005` | BigInt shall provide `toStringDec()` to return the complete, exact decimal representation of its mathematical value, with no scientific notation, truncation, rounding, or unnecessary leading zeroes. Zero shall be represented as `0`. |
| `FR-BI-006` | BigInt shall provide `toString()` with automatic numeric display formatting and shall support `operator<<` for standard C++ output streams using the same default display result. Magnitudes with at most 20 decimal digits shall use complete exact plain decimal text; magnitudes with more than 20 digits shall use the compact scientific format of FR-BI-032 with default maximum 20 significant digits. Default display shall not change the stored mathematical value. Specialized numeric stream manipulators (including `std::hex`, `std::scientific`, and `std::setprecision`) are not required to select or customize BigInt numeric formatting. |
| `FR-BI-007` | Nonempty malformed decimal numeric strings shall be rejected in full with `std::invalid_argument`; they shall not be silently partially parsed. Empty and boundary-whitespace-only strings are the explicit zero-conversion exception in FR-BI-033, not valid numeric tokens. An invalid string assignment shall leave the receiving BigInt unchanged, in accordance with FR-BI-040. |
| `FR-BI-008` | Normalized mathematical zero shall have exactly one zero limb and a nonnegative sign. Construction and assignment from accepted non-BigInt numeric forms of zero (including `-0`) shall produce that canonical zero representation; copying from another BigInt instead preserves its stored representation under FR-BI-022. A direct sign change through `setNegative(true)` may produce negative zero; this is permitted as a non-normalized representation, and `isNegative()` shall report the stored sign even when the magnitude is zero. `normalise()` shall clear the negative sign for zero. Mathematical zero comparisons, conversions, formatting and negation retain their specified zero behavior when their normalized-input precondition is satisfied (FR-BI-042). |

**Decimal text syntax.** Valid numeric text permits an optional sign, leading zeroes, one optional decimal point, and
leading/trailing ASCII spaces or tabs. The input is interpreted as an integer by truncating the fractional part toward
zero. For example, `" -00123.45 "` converts to `-123`, `.5` converts to `0`, and `5.` converts to `5`. Embedded
whitespace, commas, exponent syntax, multiple decimal points, and sign-only tokens are rejected. Nonempty input must be
validated in full.

Empty or ASCII-whitespace-only strings are invalid *numeric tokens*, but direct string construction and assignment are
special-cased to produce zero. Stream extraction requires an actual whitespace-delimited token; missing or invalid
tokens set the stream failure state and preserve the destination.

**Display versus exact conversion.** `toString()` and the default `operator<<` are for human-readable display and may
emit rounded scientific notation for values exceeding 20 decimal digits. Such display text is not guaranteed to be
accepted by the current numeric-string parser or to preserve the original value if parsed. `toStringDec()` always
produces the exact, lossless decimal representation; for every **normalized** BigInt value `x`, constructing a BigInt
from `x.toStringDec()` shall reconstruct the same mathematical value. The current parser **does not accept
scientific/exponent notation**.

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

**Shift example.** `-3 >> 1 = -2` because right shift uses floor division.

### 3.4 Value semantics

| ID | Requirement |
|---|---|
| `FR-BI-022` | Copy construction and copy assignment from a BigInt shall reproduce the source's current stored limb sequence and sign flag exactly, including non-normalized and empty limb sequences, without implicit normalization. Copying a valid moved-from object is permitted, and copies the state actually remaining in that object; no particular post-move limb sequence is assumed. The source and destination shall have independent storage. Self-copy assignment shall preserve the existing representation. Every copied non-normalized or empty destination remains subject to FR-BI-042. |
| `FR-BI-023` | Assigning a new value to an existing BigInt shall replace its previous stored value. Assignment from another BigInt shall preserve the source's limb sequence and sign flag as required by FR-BI-022; assignment from accepted built-in numeric values or decimal numeric strings shall produce a normalized destination (subject to input-error behavior in FR-BI-040). |
| `FR-BI-024` | A default-constructed BigInt shall have mathematical value zero. |

### 3.5 Additional operations, formatting, and limb access

| ID | Requirement |
|---|---|
| `FR-BI-025` | BigInt shall support prefix and postfix increment (`++`) and decrement (`--`), each changing the represented value by exactly one. A prefix operation shall yield the updated object as a modifiable lvalue; a postfix operation shall yield the independent previous value while leaving the operand incremented/decremented. Behavior shall remain exact for zero, negative values, and decimal-limb boundary transitions, without fixed-width overflow. |
| `FR-BI-026` | Unary plus shall yield the unchanged integer value. |
| `FR-BI-027` | Input-stream extraction shall follow conventional formatted whitespace-delimited token extraction, allowing multiple adjacent tokens to be read separately (e.g. `12 34` as two successive BigInt values). Each extracted token shall use the same valid numeric syntax and integer conversion rules as nonempty decimal-string construction. When no token can be obtained or a token is malformed, extraction shall set the stream failure state (`failbit`) without changing the destination BigInt; an empty stream shall not be interpreted as the zero-valued empty-string conversion exception. Input may have been consumed. A stream configured to throw on failure may do so according to standard C++ stream semantics. |
| `FR-BI-028` | An existing BigInt shall be assignable from supported integer, floating-point and decimal-string values as well as another BigInt, preserving documented conversion behavior. Assignment from accepted non-BigInt numeric values shall produce a normalized representation; assignment from a BigInt shall copy its stored representation exactly under FR-BI-022. Rejection of invalid input shall preserve the original destination representation and sign flag; other exceptional failures follow FR-BI-040. |
| `FR-BI-029` | For ordinary moves, move construction and move assignment shall transfer the source's pre-move limb sequence and sign flag to the destination without normalizing them, including non-normalized or empty representations. After a successful move, the source shall remain a valid, destructible, assignable, and copyable C++ object, but its remaining limb sequence and mathematical value are unspecified; an empty sequence is permitted. Raw queries may inspect the remaining state under their own preconditions. `setToZero()` and `normalise()` may restore a canonical representation upon success, and a supported assignment may replace the state. Self-move assignment (`a = std::move(a)`) shall be a nonthrowing, complete no-op preserving the exact limb sequence, sign flag, and normalization state, including empty storage. Copy/self-copy behavior follows FR-BI-022; in-place arithmetic with aliased normalized operands (`a += a`, `a *= a`) shall preserve the specified value semantics. |
| `FR-BI-030` | The integer result of converting a finite built-in floating-point number shall be the mathematical truncation toward zero **of the floating-point value actually received**. |
| `FR-BI-031` | Construction or assignment from NaN or positive/negative infinity shall throw `std::domain_error`, not produce a BigInt value. A failed assignment shall preserve the previous value under FR-BI-040. |
| `FR-BI-032` | BigInt shall provide `toStringSci(maxSignificantDigits = 20, pad = false)` to produce scientific-format output with caller-selected positive maximum significant-digit precision `p` and rounding to nearest, with exact ties away from zero. This method shall use scientific notation even for values that the automatic `toString()` format would display in plain decimal. In compact mode (`pad = false`), nonzero values shall retain every original significant digit when `digitCount <= p` (including original trailing zeroes); otherwise the number shall be rounded to `p` digits and trailing mantissa zeroes removed. In forced-width mode (`pad = true`), the mantissa shall contain exactly `p` digits, including zero padding if required. Zero shall use the agreed scientific zero representation. The notation shall use lowercase `e`, an explicitly signed exponent with no leading zeroes, and a leading minus sign for negative values. Formatting shall not modify the stored mathematical value. `p` denotes significant digits, not digits after the decimal point. |
| `FR-BI-033` | Construction and assignment from an empty or ASCII-space/tab-only string shall produce zero as a special conversion case, although such text is not a syntactically valid numeric token. Strings with `.5` and `5.` are valid numeric representations, while comma-grouped strings are rejected. Empty or whitespace-only strings used as numeric comparison operands are invalid under FR-BI-036. |
| `FR-BI-034` | BigInt values shall be comparable by numerical value with supported built-in signed and unsigned integer types, with either operand on the left, without narrowing or rounding the BigInt value. This includes unsigned values larger than the maximum signed value of their width. |
| `FR-BI-035` | BigInt shall provide equality and numerical ordering against supported built-in floating-point values with either operand on the left, without discarding a nonzero fractional part. For NaN, equality and all four ordering operators return false while inequality returns true in either order; positive infinity is greater than every BigInt; negative infinity is less than every BigInt. |
| `FR-BI-036` | BigInt shall provide equality and numerical ordering against valid decimal numeric strings with either operand on the left, considering any nonzero fractional digits in the string. When either operand is an invalid decimal numeric string (including empty or ASCII-whitespace-only text), `==` shall return `false`, `!=` shall return `true`, and each of `<`, `<=`, `>`, `>=` shall return `false` without throwing due to invalid numeric syntax. Such an operand is treated as unordered. Construction and assignment from a malformed nonempty string still throw under FR-BI-007; empty/whitespace-only construction and assignment still produce zero under FR-BI-033. |
| `FR-BI-037` | BigInt shall expose read-only inspection of the number of *stored* limbs and their indexed values, without requiring normalization. Index `0` denotes the least-significant base-`10^18` magnitude limb; each stored limb value shall lie in `[0, 10^18)`. Indexed `getLimb(index)` shall return `0` for out-of-range indices even when storage is empty. The count includes redundant leading zero limbs, and `getLimbCount()` may return `0`. `getFirstLimb()` and `getLastLimb()` require nonempty storage (`getLimbCount() > 0`); calling either on empty storage has undefined behavior, with no required runtime check or exception. Normalized zero has exactly one stored zero limb. |
| `FR-BI-038` | BigInt shall provide `bool setLimb(index, value)` for validated, normalizing magnitude-limb writes. A value outside `[0, 10^18)` shall cause `false` to be returned without mutation or normalization; a valid value shall return `true` unless an exception is propagated. A zero write to an index at or beyond the existing stored limb count shall be a complete no-op, without allocation, normalization, or sign changes, and shall return `true`. For any other successful write, storage shall extend as needed with zero-valued intervening limbs and the result shall be normalized. The existing sign flag shall be preserved if the resulting magnitude is nonzero, including when the prior magnitude was zero or the prior state was negative zero; normalization shall clear a negative sign if the resulting magnitude is zero. Extension shall prevent overflow when converting an index to the required storage size; other storage-size and allocation failures may be propagated as exceptions under FR-BI-040. |
| `FR-BI-039` | For a normalized BigInt, `getDigitCount()` shall report the exact count of decimal digits in its magnitude where representable, reporting one digit for zero and zero as a sentinel when the count overflows the public count type. Behavior on non-normalized representations is not guaranteed by this requirement (FR-BI-042). |
| `FR-BI-040` | Invalid-value rejections and documented input/domain errors (including malformed numeric text, nonfinite floating-point conversion, division/remainder by zero, and negative shift counts) shall provide the strong guarantee for an existing destination: its exact stored limb sequence and sign flag are unchanged. An invalid magnitude-limb value in `setLimb()` or `setLimbRaw()` shall instead be reported by returning `false`, still without mutation. Storage-capacity, allocation, and other resource exceptions may propagate, including during `setToZero()` or `normalise()`; such failures provide at least the basic guarantee: an existing destination remains a valid, destructible, assignable C++ object, but its prior value, limb sequence, sign, and normalization status need not be preserved. In particular, a failed `setToZero()` may leave an empty limb sequence. Restoration after a resource failure is the caller's responsibility before normalization-dependent operations and may itself allocate or throw. A failed constructor yields no usable destination object. No universal rollback buffer or automatic recovery is required. |
| `FR-BI-041` | BigInt shall provide public `bool setLimbRaw(index, value)` for validated magnitude-limb writes without implicit normalization. An invalid limb value outside `[0, 10^18)` shall return `false` without modifying storage or sign. A valid write shall return `true` unless a storage exception propagates, and shall preserve the existing sign flag. When `index` is outside the stored range, this operation shall extend storage up to that index, with zero-initialized intervening limbs, **including when `value` is zero**. The operation shall not remove redundant leading zero limbs or clear negative zero. It shall prevent arithmetic overflow in the index-to-required-size calculation before resize/storage access; other capacity and allocation checks may be delegated to the underlying container and their exceptions propagated under FR-BI-040. The caller is responsible for later normalization when required by FR-BI-042. |
| `FR-BI-042` | BigInt shall distinguish normalization-independent storage operations, normalization-dependent numerical operations, and normalization-restoring operations in §3.6. Non-normalized representations may be retained indefinitely, and empty storage is a permitted non-numerical object state. Every BigInt operand of a normalization-dependent operation shall be normalized before invocation; this is the caller's precondition. Implementations need not check or repair such inputs, and neither a result nor safe execution is guaranteed if the precondition is violated. A precondition violation does not *automatically* imply C++ language-level undefined behavior; particular library operations (e.g. `getFirstLimb()`/`getLastLimb()` on empty storage) can have explicitly documented undefined behavior. Normalization-independent operations shall accept non-normalized objects subject to any separate nonempty-storage preconditions; restoration is guaranteed only after successful completion. Public API documentation shall identify applicable preconditions. |

**Mixed-comparison examples.** `BigInt(12) < "12.9"` is true, `BigInt(12) == "12.9"` is false, and
`BigInt(0) == "+0.000"` is true.

**Formatting and round trips.** `toString()` and default `operator<<` are intended for display; scientific output may be
rounded and is not accepted by the current decimal-input grammar (which excludes exponent notation). Code that must
serialize a BigInt without losing information or parse its output back into BigInt must use `toStringDec()`. A
`toStringSci()` call always requests scientific notation; its precision is a maximum number of significant digits, not a
maximum number of digits following the decimal point.

**Scientific-format examples.**

| Value | Precision | Compact | Forced-width |
|---|---:|---|---|
| `20000` | 10 | `2.0000e+4` | `2.000000000e+4` |
| `20000` | 3 | `2e+4` | `2.00e+4` |
| `999950` | 4 | `1e+6` | `1.000e+6` |
| `0` | 3 | `0.00e+0` | `0.00e+0` |

**Scale demonstration.** Parsing, lossless decimal output, and addition must be demonstrated using operands of at least
**5,000 decimal digits**. This is a verification target, not a guaranteed maximum capacity or performance bound.

Bitwise AND, OR, XOR, and NOT (`&`, `|`, `^`, `~`) are excluded from mandatory functionality. Binary shifts are
required.

### 3.6 Normalization contract and API classification

A valid BigInt C++ object may have a normalized representation, a non-normalized representation, or **no stored limbs**.
Empty storage is not canonical zero and has no guaranteed numerical interpretation. The library need not normalize a
non-normalized representation on any timetable. A violated normalization precondition leaves numerical behavior outside
the contract; it is not by itself a claim that every such call causes C++ undefined behavior. In particular, numerical
operations are not required to check or report missing normalization.

| Category | Public operations | Preconditions and guarantees |
|---|---|---|
| **A — Normalization-independent** | `getLimbCount()`, `getLimb(index)`, `isNegative()`, `isNormalised()`, `setNegative(bool)`, `setLimbRaw(index, value)`, BigInt copy construction and copy assignment | Inspect, modify, or preserve the current stored representation without implicit normalization. Copying a valid moved-from object (including one with empty storage) is permitted; the actual remaining limb sequence and sign are copied exactly. `getLimb(index)` returns `0` beyond the stored range, including for empty storage; `isNormalised()` returns `false` for empty storage. |
| **A — Nonempty-storage precondition** | `getFirstLimb()`, `getLastLimb()` | Require `getLimbCount() > 0`; a call on empty storage has **undefined behavior** and no mandatory runtime check. Normalization otherwise is not required. |
| **A — Move operations** | Move construction and move assignment | Transfer the source's pre-move representation to the destination without normalization. The moved-from source remains valid but has unspecified remaining limb contents; self-move assignment is a nonthrowing *complete no-op* regardless of representation. |
| **B — Normalization-dependent numerical queries and display** | `isZero()`, `getDigitCount()`, `toStringDec()`, `toString()`, `toStringSci()`, output `operator<<`, value-to-`bool` conversion | Require normalized input. `isZero()` may test canonical zero by checking the single stored zero limb; it need not recognize raw-created multi-zero-limb representations. |
| **B — Normalization-dependent numerical operations** | All supported arithmetic, comparisons, shifts, unary `+`/`-`, compound arithmetic/shift assignments, and prefix/postfix `++`/`--`; optional math functions if implemented | Every BigInt operand, including a BigInt-valued shift count, must be normalized before invocation. No automatic validation or repair is required. |
| **C — Normalization-restoring** | `normalise()`, `setToZero()`, accepted construction/assignment from built-in numeric values or decimal strings, successful input-stream extraction, `setLimb()` after an actual write | Accept non-normalized destination storage and produce a normalized representation **upon successful completion**. `setLimb()` out-of-range zero writes, and rejected invalid limb values, preserve the existing state instead. Failed extraction/domain validation preserves the destination; resource exceptions follow FR-BI-040. |

**Negative zero and raw storage.** Directly setting the sign of zero, or raw limb manipulation, may produce negative
zero. `isNegative()` reports the actual sign flag; `isNormalised()` reports `false`. Normalizing zero clears the sign. A
nonzero actual `setLimb()` write preserves the prior sign, including from negative zero, while an out-of-range zero
no-op does not modify anything. `setLimbRaw()` never implicitly normalizes and may preserve redundant zero limbs
indefinitely.

**Moved-from and empty representations.** After a successful ordinary move, the destination preserves the transferred
pre-move limb sequence and sign. The moved-from source remains valid, and its current state can be inspected using the
Category A operations whose preconditions are met. A copy of that source preserves exactly the state that remains, even
when the limb sequence is empty; copying empty storage is **not** undefined behavior. `getFirstLimb()` and
`getLastLimb()` always require nonempty storage. `normalise()` restores empty storage to canonical zero upon success;
for nonempty storage it normalizes the remaining magnitude rather than guaranteeing that a moved-from source becomes
zero. `setToZero()` restores canonical zero upon success. Restoration may fail due to resource exhaustion.

**Self-assignment and failures.** Self-copy assignment preserves the representation. Self-move assignment is a
nonthrowing, complete no-op even for non-normalized, negative-zero or empty states. Invalid input and domain errors must
preserve the existing representation; resource exceptions need preserve only a valid C++ object, potentially with empty
storage. After such a failure, callers must establish the relevant preconditions before numerical use. Neither
successful restoration nor memory allocation is guaranteed under resource exhaustion.

**Boundary behavior.** `setLimb()` and `setLimbRaw()` shall prevent overflow when calculating the storage size needed
for an extension. An out-of-range zero write through `setLimb()` remains a successful no-op, including at the largest
representable index. `getDigitCount()` shall report `0` when the exact digit count is not representable, as specified by
FR-BI-039.

## 4. EvaluatorDemo requirements

### 4.1 Expressions and assignments

| ID | Requirement |
|---|---|
| `FR-DEMO-001` | The program shall accept user input and print results in an interactive text-based session. |
| `FR-DEMO-002` | It shall evaluate one optionally signed integer literal or variable reference, or two such operands connected by exactly one supported binary operator. Each operand may have at most one leading unary `+` or `-`, immediately adjacent to its literal or identifier; a sign at the start of an operand is unary, whereas an operator separating two operands is binary. |
| `FR-DEMO-003` | Integer literal operands shall consist of one or more ASCII decimal digits (`0`–`9`), optionally preceded by one immediately adjacent unary `+` or `-`. Leading zeroes shall be accepted. Decimal points, exponent notation, internal whitespace and chained unary signs shall not be accepted as mandatory integer operands. |
| `FR-DEMO-004` | It shall support the operators `+`, `-`, `*`, `/`, `%`, `<<`, and `>>` using the corresponding BigInt semantics. |
| `FR-DEMO-005` | It shall display results using the required automatic formatting policy, including compact scientific notation for large values. Display rounding shall not change the exact stored result. Successful unassigned calculations shall display `ans = <value>`. |
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
| `FR-DEMO-020` | Automatic calculator display shall use `BigInt::toString()` as the shared default formatting policy: exact plain decimal for magnitudes of at most 20 decimal digits, and compact scientific notation with at most 20 significant digits for larger magnitudes. The calculator shall not implement a separate automatic format-selection or rounding algorithm. Formatting shall not change stored variables or `ans`. |
| `FR-DEMO-021` | Successful assignments shall print the identifier and result (`name = value`) using the same exact/scientific display policy as ordinary results. |
| `FR-DEMO-023` | Evaluating an existing bare variable name, including any case variation of `ans`, shall display `<name> = <value>` and copy the referenced value into `ans`. An undefined variable name shall produce an error without changing state. Bare `ans` shall use the canonical display label `ans`. |
| `FR-DEMO-028` | Successful unassigned binary expressions, unary expressions, and standalone numeric literals shall display `ans = <value>`. Successful assignments and bare variable-name evaluations shall display `<name> = <value>` using the automatic numeric-display policy. All successful unassigned evaluations update `ans`. |

### 4.4 Commands

The command reference below summarizes commands specified by `FR-DEMO-019`, `FR-DEMO-022`, `FR-DEMO-026`, `FR-DEMO-027`,
and `FR-DEMO-030`.

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
| `FR-DEMO-022` | `@showdigits <identifier>` shall display the complete unrounded exact decimal value of an existing variable or `ans` using the BigInt exact-decimal formatting behavior (`toStringDec()`), irrespective of magnitude, without modifying any stored value or `ans`. |
| `FR-DEMO-024` | Every calculator command shall start with `@` immediately followed by a case-insensitive command name; ordinary variable identifiers remain case-sensitive except for `ans`. The unprefixed names `show`, `showdigits`, `showscientific`, `dumpvars`, and `exit` remain available as ordinary variable identifiers. |
| `FR-DEMO-026` | `@show <identifier>` shall display a previously defined variable or `ans` using the default automatic numeric formatting without changing stored state. |
| `FR-DEMO-027` | `@showscientific <identifier> [precision] [pad]` shall display an existing variable or `ans` using BigInt scientific formatting (`toStringSci()`) with default maximum significant-digit precision 20 or a supplied precision. A precision token shall contain one or more unsigned ASCII decimal digits (`0`–`9`), without any `+`/`-` sign, decimal point, or exponent; leading zeroes are permitted. Its numerical value shall be greater than zero and representable by the supported precision parameter type. There shall be no arbitrary 20-digit cap on the input token or precision value; 20 is the default precision, not a maximum. The case-insensitive `pad` option may appear without a precision argument (`@showscientific <identifier> pad`), in which case precision defaults to 20. Without `pad`, compact formatting applies; with it, exactly `p` mantissa digits are displayed. The command shall not change stored state. |
| `FR-DEMO-029` | Variable-targeted display commands `@show`, `@showdigits`, and `@showscientific` shall accept only an existing variable identifier (including any case variant of `ans`), not numeric literals or expressions; invalid names and malformed command arguments shall produce errors without modifying state. |
| `FR-DEMO-030` | `@dumpvars` shall accept no arguments and display the predefined `ans` first, followed by every currently defined ordinary variable in deterministic alphabetical order. Each line shall use the original variable name and automatic numeric formatting (`name = value`). The command shall not modify stored values or `ans`; a fresh session shall display `ans = 0`. |
| `FR-DEMO-031` | Commands shall accept leading/trailing ASCII spaces or tabs and separate arguments with one or more ASCII spaces or tabs. The command prefix `@` must be immediately adjacent to its name; command names and `pad` shall be case-insensitive. Missing, extra or unrecognized arguments, invalid variable references, unsupported flags, malformed precision tokens (including explicit signs, fractions and exponents), nonpositive or numerically unrepresentable precision values, and unknown commands shall produce recoverable errors without state changes. A precision token shall not be rejected solely because its digit sequence is long when its numeric value is representable. |

### 4.5 Input handling and error recovery

| ID | Requirement |
|---|---|
| `FR-DEMO-013` | Malformed expressions shall be rejected with an understandable diagnostic without silently producing an incorrect result. |
| `FR-DEMO-014` | An undefined variable reference shall be reported as an error. |
| `FR-DEMO-015` | An arithmetic failure (such as division by zero) shall be reported and shall not terminate the interactive session. |
| `FR-DEMO-016` | Following a recoverable input or evaluation error, the application shall accept another input. |
| `FR-DEMO-025` | An empty or whitespace-only input line shall be ignored without output or state change. |

### 4.6 Expression grammar

A statement evaluates one optionally signed operand, optionally followed by **one** binary operator and a second
optionally signed operand. An assignment stores the result of either form. Expressions are parsed left to right and must
consume the entire input line, apart from permitted boundary whitespace. Commands use the separate argument rules in
§4.4.

```text
statement              := ws (assignment | evaluation | command) ws
assignment             := assignable_identifier ws '=' ws evaluation
evaluation             := signed_operand [ws binary_operator ws signed_operand]
signed_operand         := [unary_sign] primary
unary_sign             := '+' | '-'
primary                := integer_literal | identifier
integer_literal        := digit+
digit                  := '0'..'9'
binary_operator        := '+' | '-' | '*' | '/' | '%' | '<<' | '>>'
identifier             := [A-Za-z_][A-Za-z0-9_]*
assignable_identifier  := identifier except any case spelling of 'ans'
ws                     := (' ' | '\t')*
command                := a command form defined in §4.4
```

**Unary/binary disambiguation.** At the beginning of an operand (the beginning of an evaluation or immediately following
a binary operator), a single `+` or `-` is a unary sign **only when directly attached** to a literal or identifier.
Between two operands, `+` or `-` is binary. Spaces and tabs are permitted around binary operators and `=`, but **not**
between a unary sign and its operand. For example, `a-b`, `a - b`, `a - -b`, `-a-b`, `-a--b`, `-a + -b`, `a++b` and
`a+-b` are syntactically valid. In contrast, `- a`, `a - - b`, `--a`, and `a---b` are invalid. Chained binary
expressions, operator precedence, parentheses (including `-(a+b)`), and fractional/dotted or exponent-form integer
operands are outside the mandatory grammar. A syntactically valid expression can still fail semantically (for example,
`a << -3` uses a negative shift count). An empty or whitespace-only line follows FR-DEMO-025 rather than this grammar.

**Grammar separation.** This is intentionally narrower than BigInt's decimal-string constructor syntax: the library may
accept fractional text and truncate its value, but the mandatory calculator does not accept decimal-point literals.
Similarly, an `@showscientific` precision argument is an unsigned **digit token** representing a positive in-range
integer, not a signed expression, fractional value, or arbitrary 20-digit-limited field.

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
