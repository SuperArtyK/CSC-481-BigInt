# BigInteger — Software Requirements Specification (SRS)

| Document control | Value |
|---|---|
| Document ID | CSC481-BIGINT-SRS |
| Current version | **0.3 - Section 1 review draft** |
| Date | 2026-10-25 |
| Status | **Not approved** - team review in progress |
| Planning horizon | Three weeks from 2026-10-25 (working target: 2026-11-15; not an official deadline) |
| Intended audience | Development team, instructor, reviewers, library users |
| Document owner | Project product owner |
| Document authorship | Team-authored specification in progress; current drafts prepared at the product owner's direction |
| Required reviewers for initial baseline | All five project team members |
| Initial baseline approval policy | Explicit approval from each of the five team members is required before publishing v1.0 |
| Current approval record | No team-wide approval recorded; v0.3 is a working draft |
| Repository | https://github.com/SuperArtyK/CSC-481-BigInt |

> **Reading convention:** A statement labeled **Requested** reflects an intention expressed by the project product owner, not necessarily a team-approved requirement. **Proposed** is an explicit recommendation needing approval. **Blocked** identifies requested behavior with unresolved, test-preventing semantics. All items are draft until the team approves a baseline. The SRS specifies observable behavior and constraints; C++ signatures, algorithms, data layouts, exception classes, and build-target structure belong in the Software Design Document (SDD).

## 1. Preface and revision history

### 1.1 Purpose and audience

This SRS specifies the intended observable behavior of the CSC 481 BigInteger system and the constraints under which it is developed and delivered. It supports implementation planning, acceptance testing, review, and the final course report. Technical and nontechnical team members should be able to understand the user requirements; implementers use the more detailed system requirements below.

### 1.2 Revision history

| Version | Date | Author / drafting provenance | Review / approval status | Rationale and changes |
|---|---|---|---|---|
| 0.1 | 2026-10-25 | initial draft | Superseded; not team-approved | First captured requirements, objectives, module descriptions, and example acceptance tests. |
| 0.2 | 2026-10-25 | audit and revision | Superseded; not team-approved | Corrected scope assumptions; separated requested/proposed/unresolved behavior; added review, decision and traceability records. |
| **0.3** | **2026-10-25** | team check | **Working draft; awaiting five-member approval of eventual baseline** | **Section 1: document ownership and approval metadata, revision provenance, and source authority clarified. No behavioral requirements changed.** |

Preserved drafts: `versions/SRS_v0.1.md`, `versions/SRS_v0.2.md`, and `versions/SRS_v0.3.md`. Detailed change rationales are maintained in `CHANGELOG.md` and in the applicable review records; a revision must explain **why** changes were made rather than only listing edits. Actual team contributors and reviewers shall be credited accurately when their participation is recorded.

**Initial requirements baseline approval (product owner's chosen policy):** Version 1.0 may be marked **Approved** only after all five team members have explicitly approved the same identified revision. Record each person's name or GitHub identifier, approval date, and document version or commit reference in a review/approval record. Silence or lack of objections does not constitute approval. The instructor's announcement calls for meaningful collaboration but does not itself prescribe this unanimous approval rule. Procedures for later post-baseline changes can be agreed by the team when the baseline is established.

### 1.3 Sources and authority

The sources below serve different purposes. An instructor's assignment constraints cannot be overridden by team choices; project behavior not prescribed by the course is defined by the team's **approved** requirements, not by proposals or existing code.

| Source | Role and authority |
|---|---|
| **Instructor's announcement and forthcoming official templates** | Establish course deliverables, collaboration expectations, and required report/presentation constraints. The announced grading weights are tentative; final templates and rubric have not been supplied. |
| **Team-approved BigInteger requirements (once established)** | Define the authoritative behavioral scope and acceptance criteria for the chosen application, subject to course constraints. No baseline is approved yet. |
| **Project product owner's stated intentions** | Input to requirements elicitation: three modules (`BigInt`, `BigInt_Math`, `EvaluatorDemo`), basic operations, calculator behavior including assignment and `ans`, C++23, CMake, CTest/Google Test, and standard-C++ production dependencies. These intentions are not equivalent to all-five-member approval. |
| **CSC 481 lectures (especially 4-1 and 4-2)** | Guide requirements terminology, document organization, analysis, and validation; they do not mandate specific BigInt operators or arithmetic semantics. |
| **GitHub repository and PR #14 (observed 2026-10-25)** | Evidence of implementation and collaboration status at the observed revision, not a source of requirements authority or proof of team approval. Repository status may change. |

The decimal-limb representation is a project product owner design direction to be justified in the Software Design Document (SDD). Implementation choices, C++ signatures, internal storage layout, algorithm selections, and exception types remain outside this behavioral SRS. Proposed and unresolved choices throughout this document remain unapproved until the team explicitly accepts them.

## 2. Introduction

### 2.1 Problem

Built-in C++ integer types have fixed finite ranges, which makes them unsuitable for exact integer computations outside those ranges. The project provides a reusable signed arbitrary-precision integer facility, optional auxiliary mathematics, and an interactive demonstration program that lets users perform and examine exact integer calculations.

### 2.2 Objectives

The system aims to:

- Represent and manipulate signed integers of magnitudes beyond built-in fixed-width types, limited by available resources rather than a chosen fixed digit count.
- Make common integer operations available through familiar C++ expressions and decimal conversions.
- Offer a separate component for mathematical functions whose behavior is defined over integers.
- Demonstrate the library through a simple CLI with variables and a reserved previous-answer identifier.
- Provide test evidence and documentation of requirements, architectural choices, implementation, and team collaboration.

### 2.3 Scope boundary

**In scope:** three named components, decimal integer input/output, requested arithmetic, a small CLI expression grammar, unit testing and course deliverables. Finite floating-point-to-integer conversion is **requested but behaviorally incomplete** (see §6.1.5 and D-01).

**Out of initial scope unless separately approved:** arbitrary-precision fractional values; a complete general-purpose expression language with precedence/parentheses; complex numbers; floating-point mathematical results; GUI or persistent storage; all `cmath` functions; promises of optimal asymptotic or runtime performance.

### 2.4 Stakeholders

- **Library developer:** integrates `BigInt` into other C++ applications.
- **Calculator user / demonstrator:** enters expressions, reuses variables, observes results and errors.
- **Development team (five students):** specifies, implements, tests, reviews, and documents the system.
- **Instructor / project evaluator:** evaluates evidence of the software engineering process and working deliverable.

## 3. Glossary

| Term | Meaning |
|---|---|
| Arbitrary-precision integer | An integer whose capacity can grow with memory/resource availability rather than a fixed built-in width. |
| Limb | A fixed-width element storing part of an integer; an internal design detail, not part of the public behavior contract. |
| Canonical decimal representation | A decimal numeral with no unnecessary leading zeroes and a minus sign only for strictly negative numbers. |
| Exact arithmetic | Integer result equals the mathematically defined operation, without silent wraparound or rounding. |
| User requirement | High-level service described from the user's perspective. |
| System requirement | Specific, verifiable description of behavior or a system constraint. |
| SRS | Software Requirements Specification (what the system must do). |
| SDD | Software Design Document (how interfaces and internals realize the requirements). |
| `ans` | Reserved calculator identifier for the most recent successful expression result not assigned to a named variable, subject to pending initialization details. |
| Acceptance test | Evidence that a stated requirement is fulfilled. |

## 4. User requirements

Each statement is deliberately understandable without knowing the internal integer representation.

| ID | User-level requirement | Origin |
|---|---|---|
| UR-001 | A developer shall be able to represent positive, negative, and zero integers larger than the limits of ordinary built-in integer types. | Requested |
| UR-002 | A developer shall be able to perform familiar exact integer calculations and comparisons using these values. | Requested |
| UR-003 | A developer shall be able to convert between human-readable decimal representations and BigInt values. | Requested |
| UR-004 | A developer shall be able to use additional mathematical integer functions through an independent module. | Requested; function list open |
| UR-005 | A calculator user shall be able to enter and evaluate a simple expression containing two operands and one operator. | Requested |
| UR-006 | A calculator user shall be able to save a result in a named variable and use that variable in later expressions. | Requested |
| UR-007 | A calculator user shall be able to use `ans` case-insensitively to retrieve the previous eligible result. | Requested |
| UR-008 | A calculator user shall receive an understandable error and continue using the calculator after invalid input or arithmetic failure. | Proposed |

## 5. High-level system architecture

The system is divided into three responsibility-based components:

```text
                 EvaluatorDemo (interactive calculator)
                      |                    |
                      v                    v (optional use)
                    BigInt <---------- BigInt_Math
```

**Dependency direction:** `BigInt_Math` depends on `BigInt`; `EvaluatorDemo` depends on `BigInt` and may optionally invoke `BigInt_Math` if the team agrees how math functions will be demonstrated. `BigInt` shall be usable independently of both other components. This diagram is logical, not a specification of CMake targets or source directories.

The detailed API, choice of header-only vs. compiled packaging, decimal limb base, internal invariants, and algorithm selections belong in the SDD and decision register, not this SRS.

## 6. Detailed system requirements

### 6.1 BigInt — core behavior

The rows below are draft candidate requirements. **Requested** indicates scope expressed by the project owner; **Proposed** identifies behavior that still requires explicit acceptance.

#### 6.1.1 Values, construction, and decimal representation

| ID | Requirement | Origin |
|---|---|---|
| FR-BI-001 | BigInt shall represent nonnegative and negative integers and zero without a fixed application-defined digit limit, subject to resource availability. | Requested |
| FR-BI-002 | BigInt shall preserve exact integer values during supported calculations, without silent overflow of the mathematical result. | Requested |
| FR-BI-003 | BigInt shall be constructible from standard signed and unsigned integral values that its public interface supports. | Requested |
| FR-BI-004 | BigInt shall parse a valid optionally signed base-10 integer string into the exact represented value. | Requested |
| FR-BI-005 | Decimal output shall represent the exact mathematical value without scientific notation or leading zeroes; zero shall be written as `0`. | Proposed clarification |
| FR-BI-006 | BigInt values shall be printable to a standard output stream in decimal form. | Requested |
| FR-BI-007 | Invalid integer strings shall be rejected as a whole rather than partially parsed into an unintended valid integer. | Proposed clarification |
| FR-BI-008 | Mathematical zero shall have the same observable value under comparison, conversion, and negation regardless of the form used to construct it (including `-0`). | Proposed clarification |

**String-syntax proposal (D-02):** optional leading `+`/`-` followed by one or more ASCII digits `0`–`9`; initial and trailing whitespace are not accepted by the core parser unless the team adopts a different policy. Leading zeroes are accepted and removed in canonical output. This grammar is **proposed**, not yet ratified.

#### 6.1.2 Arithmetic, comparison, and truth value

| ID | Requirement | Origin |
|---|---|---|
| FR-BI-009 | BigInt shall support exact binary addition. | Requested |
| FR-BI-010 | BigInt shall support exact binary subtraction. | Requested |
| FR-BI-011 | BigInt shall support exact binary multiplication. | Requested |
| FR-BI-012 | BigInt shall support integer quotient calculation for a nonzero divisor. | Requested; sign semantics proposed below |
| FR-BI-013 | BigInt shall support integer remainder calculation for a nonzero divisor. | Requested; sign semantics proposed below |
| FR-BI-014 | BigInt shall support equality and full numerical ordering (`<`, `<=`, `>`, `>=`, `!=`). | Requested |
| FR-BI-015 | BigInt shall support unary arithmetic negation. | Requested |
| FR-BI-016 | BigInt shall support a boolean interpretation of its value, false exactly for zero and true otherwise. | Requested |
| FR-BI-017 | BigInt shall support updating an existing value through the compound forms of its supported arithmetic operators (`+=`, `-=`, `*=`, `/=`, `%=`). | Proposed extension to integer-like use |
| FR-BI-018 | Division or remainder with a zero divisor shall fail in a documented and detectable way rather than produce a numeric result. | Proposed clarification |

**Signed division proposal (D-03):** quotient truncates toward zero. The remainder is defined by `a == (a / b) * b + (a % b)` and has the dividend's sign if nonzero. Example: `-17 / 5 == -3`; `-17 % 5 == -2`. Team approval required.

#### 6.1.3 Shift operations

| ID | Requirement | Origin |
|---|---|---|
| FR-BI-019 | BigInt shall support a left-shift operation corresponding, for a valid nonnegative count `n`, to multiplication by `2^n`. | Requested |
| FR-BI-020 | BigInt shall support right shift by a valid nonnegative count `n`; signed rounding behavior must be defined before approval. | Requested; **Blocked** on D-04 |
| FR-BI-021 | BigInt shall support compound left- and right-shift assignment operations when the underlying shift operations are supported. | Proposed extension |

**Not resolved (D-04):** right shift of a negative value (rounding toward negative infinity vs. toward zero); permitted count types/ranges; behavior of negative or otherwise invalid shift counts. No test should assume these semantics until approved.

#### 6.1.4 Copying, values, and use as an integer-like type

| ID | Requirement | Origin |
|---|---|---|
| FR-BI-022 | Copying a BigInt shall produce an independently usable value with the same mathematical value as the source. | Proposed clarification |
| FR-BI-023 | Assigning a new value to an existing BigInt shall replace its previously represented value. | Proposed clarification |
| FR-BI-024 | A default-constructed BigInt shall have mathematical value zero. | Proposed; needs team approval |

This section specifies value semantics rather than C++ member signatures or memory-management techniques.

#### 6.1.5 Floating-point conversion — requested, not yet testable

| Candidate | Requested capability | Status |
|---|---|---|
| CAND-BI-FLOAT-001 | Construct an integer value from a finite built-in floating-point value through a defined conversion policy. | **Blocked — D-01** |
| CAND-BI-FLOAT-002 | Reject or otherwise explicitly handle infinities and NaNs if the chosen input type can represent them. | **Blocked — D-01** |

**Important limitation:** serializing a floating-point number to decimal text does not recover information already lost when that floating-point value was formed. Scientific notation, fractional values, and large finite values require a deliberate conversion rule. The SDD may describe decimal serialization as an implementation strategy, but the SRS must first define the observable integer result (for example, reject fractions or truncate toward zero). This capability remains requested; it is **not silently discarded**.

### 6.2 BigInt_Math — auxiliary mathematical functions

| ID | Requirement | Origin |
|---|---|---|
| FR-MATH-001 | BigInt_Math shall expose a separately usable collection of integer mathematical operations on BigInt values. | Requested |
| FR-MATH-002 | Each included mathematical operation shall document its valid input domain and expected result/error behavior. | Proposed clarification |
| FR-MATH-003 | Mathematical operations declared exact over integers shall return mathematically exact integer results. | Proposed clarification |

**Function inventory is not approved.** The owner's descriptions mention powers, roots, and factorial/gamma-like capabilities, but do not commit to a precise subset. Candidates for a three-week minimum viable scope are:

| Candidate ID | Function | Proposed behavior | Decision |
|---|---|---|---|
| CAND-MATH-POW | Integer exponentiation | `base^n` for nonnegative integer `n`; define `0^0` behavior. | D-05 |
| CAND-MATH-FACT | Factorial | `n!` for `n >= 0`, with `0! = 1`. | D-05 |
| CAND-MATH-ISQRT | Integer square root | Nonnegative input yields floor of real square root. | D-05 |
| CAND-MATH-GCD | Greatest common divisor | Conventional nonnegative gcd, including zero cases. | Optional candidate, D-05 |

**Do not claim gamma, floating-point transcendental functions, or arbitrary fractional roots** unless the team explicitly accepts an integer-valued definition. The SRS will add individual `FR-MATH-*` requirements with examples after choosing the set.

### 6.3 EvaluatorDemo — CLI behavior

#### 6.3.1 Basic evaluation

| ID | Requirement | Origin |
|---|---|---|
| FR-DEMO-001 | The program shall accept user input and print results in an interactive text-based session. | Requested |
| FR-DEMO-002 | It shall evaluate a well-formed expression containing exactly two operands and one supported binary operator. | Requested |
| FR-DEMO-003 | It shall accept signed decimal integer literals as operands. | Requested |
| FR-DEMO-004 | It shall support the operators `+`, `-`, `*`, `/`, `%`, `<<`, and `>>` to the extent their BigInt semantics have been approved. | Proposed operator inventory from BigInt scope |
| FR-DEMO-005 | It shall display the exact decimal result for a successful evaluation. | Requested |
| FR-DEMO-006 | It shall permit an operand to reference a previously defined named variable. | Requested |
| FR-DEMO-007 | It shall permit the expression result to be stored in a named variable through assignment syntax. | Requested |

#### 6.3.2 Variables, reserved identifiers, and session behavior

| ID | Requirement | Origin |
|---|---|---|
| FR-DEMO-008 | A variable assigned successfully shall be available to subsequent expressions within the same calculator session. | Requested |
| FR-DEMO-009 | `ans`, regardless of letter case, shall refer to the latest successful **unassigned** expression result. | Requested; wording clarified |
| FR-DEMO-010 | A successfully assigned expression shall not update the value of `ans`. | Interpretation of owner wording; team confirmation required |
| FR-DEMO-011 | A rejected expression shall not modify existing named variables or `ans`. | Proposed atomicity rule |
| FR-DEMO-012 | The program shall provide a user-visible way to exit an interactive session. | Proposed clarification |

**Open:** behavior of `ans` before any eligible expression; identifier spelling/case rules for ordinary variables; whether assigning to an existing name overwrites; whether assigning to `ans` is rejected; whitespace acceptance; how end-of-file is handled. See D-06 to D-09.

#### 6.3.3 Error behavior

| ID | Requirement | Origin |
|---|---|---|
| FR-DEMO-013 | Malformed expressions shall be rejected with an understandable diagnostic without silently producing an incorrect result. | Proposed |
| FR-DEMO-014 | An undefined variable reference shall be reported as an error. | Proposed |
| FR-DEMO-015 | An arithmetic failure (such as division by zero) shall be reported and shall not terminate the interactive session. | Proposed |
| FR-DEMO-016 | Following a recoverable input or evaluation error, the application shall accept another input. | Proposed |

#### 6.3.4 Grammar boundary

The owner requested a **single** binary operation, not a general expression parser. The following notation is illustrative, not yet an approved lexical grammar:

```text
statement   := expression | identifier '=' expression
expression  := operand binary_operator operand
operand     := signed_decimal_literal | identifier
binary_operator := '+' | '-' | '*' | '/' | '%' | '<<' | '>>'
```

This grammar intentionally does **not** promise parentheses, operator precedence, chained operations, or unary-only expressions. Whether `ans` is available as an `identifier` is yes; its case-insensitive reserved behavior is specified above. Spaces and lexing rules are awaiting D-07.

**Proposed session illustration (not final acceptance test):**

```text
> 4 * 20
80
> ans * 10
800
> a = 5 * 100
a = 500
> a + 25
525
> ANS / 5
105
```

## 7. Nonfunctional and process requirements

All are proposed as course-aligned requirements, except where directly stated by the project owner or instructor.

| ID | Requirement | Origin / evidence |
|---|---|---|
| NFR-001 | Production components shall be implemented in standard C++23. | Requested |
| NFR-002 | Running production libraries or the CLI shall not require third-party runtime dependencies beyond the chosen platform's standard C++ runtime and other ordinary system prerequisites. | Requested |
| NFR-003 | The project shall be configurable and buildable with CMake. | Requested |
| NFR-004 | Test cases shall be executable through CTest, with Google Test permissible as a **development/test dependency**, not a production runtime dependency. | Requested |
| NFR-005 | The team shall validate intended compiler/platform configurations on GCC and Clang for Linux and MSVC for Windows, or document any unsupported configurations honestly. | Proposed measurable cross-platform verification goal |
| NFR-006 | Core integer computations shall not silently truncate or wrap mathematical results due to exceeding a fixed built-in integer width. | Requested correctness |
| NFR-007 | The three major components shall have independently documented responsibilities, with `BigInt` independently reusable. | Requested modularity |
| NFR-008 | A successful test run shall cover specified arithmetic edge cases, input failures, and calculator session behavior. | Proposed quality measure |
| NFR-009 | The team shall record work and reviews using GitHub issues and pull requests. | Instructor announcement |
| NFR-010 | Each of the five members shall have a clear, substantial and reviewable project contribution. | Instructor announcement |
| NFR-011 | Final deliverables shall include a functioning application, comprehensive documented report, and presentation/demo conforming to the templates when published. | Instructor announcement |
| NFR-012 | The development team shall maintain an SRS revision history explaining significant changes and record architectural/design rationales separately. | Lecture guidance + instructor report emphasis |

**No quantitative speed, memory, or maximum-digit guarantee is currently approved.** Inventing one would create unnecessary acceptance risk. If a measurable performance goal is desired, add a benchmark specification with input sizes, hardware/environment, and threshold. Source-level portability is intended; absolute portability to all C++23 toolchains is not asserted.

## 8. Acceptance and requirements validation

### 8.1 Verification approach

- **Unit tests:** constructors/conversions, positive/negative/zero arithmetic, arithmetic identities, and invalid inputs.
- **Component integration tests:** math functions consume BigInt values; calculator delegates numeric work to the library.
- **CLI tests/manual demonstration:** expression grammar, variables, `ans`, error recovery, exit behavior.
- **Build evidence:** approved compiler configuration checks and test logs.
- **Documentation review:** verify requirement coverage and record team approval/sign-off.

A requirement is complete when the behavior is implemented, appropriate acceptance evidence exists, and a reviewer confirms the behavior matches the approved wording. Proposed or blocked semantics must be resolved before their test results can constitute acceptance.

### 8.2 Representative acceptance cases (proposals)

| Test ID | Requirement(s) | Input/action | Expected behavior | Approval note |
|---|---|---|---|---|
| T-BI-001 | FR-BI-001, 002 | Represent `18446744073709551616` and add `1` | `18446744073709551617` | Demonstrates beyond 64-bit unsigned range |
| T-BI-002 | FR-BI-004, 005, 008 | Parse `-00000042` then output | `-42` | Requires D-02 grammar approval |
| T-BI-003 | FR-BI-004, 007 | Parse `12abc` | Complete input rejected | Requires D-02 |
| T-BI-004 | FR-BI-008, 015 | Negate or compare `-0` and `0` | Equal; display `0` | Proposed canonical behavior |
| T-BI-005 | FR-BI-009 | `999999999999999999 + 1` | `1000000000000000000` | Carry across decimal-limb boundary in selected design |
| T-BI-006 | FR-BI-010 | `0 - 1000` | `-1000` | Signed subtraction |
| T-BI-007 | FR-BI-011 | `123456789 * 987654321` | `121932631112635269` | Exact multiplication |
| T-BI-008 | FR-BI-012, 013 | `-17 / 5`, `-17 % 5` | `-3`, `-2` | **Pending D-03** |
| T-BI-009 | FR-BI-018 | `12 / 0` | Detectable error, no numeric result | Error mechanism described in SDD |
| T-BI-010 | FR-BI-019 | `3 << 4` | `48` | Valid nonnegative count |
| T-BI-011 | FR-BI-020 | `-3 >> 1` | **TBD** | **Blocked on D-04** |
| T-BI-012 | FR-BI-014, 016 | Compare -1 and 1; bool(0), bool(-1) | -1 smaller; false/true | Ordering and truth |
| T-DEMO-001 | FR-DEMO-002, 005 | `4 * 20` | `80` | Basic evaluation |
| T-DEMO-002 | FR-DEMO-006..010 | `a = 4 * 20` then `a * 10` | `800`; assignment does not update `ans` | Confirm D-06 |
| T-DEMO-003 | FR-DEMO-009 | Use `ANS`, `Ans`, `ans` after eligible result | Same stored value | Case-insensitive reserved word |
| T-DEMO-004 | FR-DEMO-013, 016 | `2 +` then `2 + 3` | Error, then `5` | Recoverable syntax error |
| T-DEMO-005 | FR-DEMO-011, 014 | `a = 4 * 5`, invalid assignment `a = z * 2`, then `a + 1` | Error and prior `a` preserved; `21` | Atomicity rule proposed |

The companion `TRACEABILITY.md` contains the full preliminary requirement-to-verification map. Exact acceptance cases for `BigInt_Math`, floating-point conversion, and signed shifts require decisions first.

### 8.3 Review criteria (from Lecture 4-2)

Before accepting v1.0, check **validity, consistency, completeness, realism, and verifiability** with all five members. Document unresolved issues and any rejected requirements instead of silently assuming an interpretation.

## 9. System models and technical documentation boundaries

The final report/SDD should include a simple component dependency diagram, an evaluator processing-flow diagram, and a model of the BigInt public interface where useful. These are planned artifacts, **not claimed to exist**. Class signatures, test structure, limb storage, exact integer algorithms, API exception types, and CMake details go to the SDD.

## 10. System evolution

The team may later consider additional mathematical functions, performance-focused multiplication/division algorithms, more expressive parsing, and additional numeric representations. These are not part of the initial acceptance baseline unless a documented revision promotes them.

## 11. Open decisions and approval gate

| Decision | Blocking question | Needed before |
|---|---|---|
| D-01 | How are fractional/NaN/infinite floating-point values converted or rejected? | Floating conversion implementation/tests |
| D-02 | Which decimal string formats, whitespace, and signs are valid? | Input acceptance tests |
| D-03 | Which signed quotient/remainder convention is required? | Division acceptance tests |
| D-04 | What exactly is right shift of negative values and how are invalid counts handled? | Shift acceptance tests |
| D-05 | Which math functions and their domains are mandatory for the 3-week deliverable? | Math module implementation |
| D-06 | What is the initial/update behavior of `ans`, and may users assign to it? | Calculator state tests |
| D-07 | Which identifier, whitespace and case rules apply? | Calculator parser tests |
| D-08 | Can variables be reassigned; what is printed after assignment? | Calculator variable tests |
| D-09 | How does the session exit (command and/or EOF)? | CLI session tests |
| D-10 | How are math functions demonstrated without extending the two-operand grammar? | Integrated demo plan |
| D-11 | Which C++ integration/packaging choices (header-only vs. compiled) are adopted? | SDD/API work, **not** SRS behavior |

**Approval gate:** Team reviews and resolves D-01 through D-10 where they affect committed features, chooses the mandatory math functions, ensures every active requirement has acceptance evidence planned, and records a dated v1.0 approval. An official final report/presentation template, when released, will govern the final submission's organization. The team's 2026-11-15 working target does not assert an instructor-assigned deadline.
