# Prompt: Design and Implement a Type-Safe C++ Functional Threading Library (`thru`)

I want to design and implement a small, generic C++ functional programming library that provides threading capabilities similar to Clojure's `->` (thread-first) and `->>` (thread-last) macros.

The objective is to bring the expressiveness and readability of Clojure's threading approach into C++, while respecting C++'s type system, value categories, ownership model, and compile-time capabilities.

The library should be small, idiomatic, self-contained, and suitable for incorporation into an existing C++ project.

## 1. Design Goals

Provide a fluent pipeline syntax that allows the result of one function invocation to be passed automatically into the next function invocation.

The public API must use:

- A single factory function named `thru`.
- Overloaded `operator>` for thread-first semantics.
- Overloaded `operator>>` for thread-last semantics.
- A terminal `.value()` operation to extract the final pipeline result.

Do not introduce separate `thru_first` and `thru_last` factory functions. The operator determines the threading behavior, not the factory function.

The intended API should be concise, expressive, and suitable for everyday C++ programming.

Conceptually, the API should look like this:

```cpp
auto result = (input
    > thru(function_a, arg1, arg2)
    > thru(function_b, arg3)
    > thru(function_c)
).value();
```

Thread-last pipelines should use the corresponding operator:

```cpp
auto result = (input
    >> thru(function_a, arg1, arg2)
    >> thru(function_b, arg3)
    >> thru(function_c)
).value();
```

These examples illustrate the intended syntax. Determine the appropriate implementation and exact types needed to support it.

## 2. Threading Semantics

### Thread-first: `operator>`

Insert the value being threaded as the first argument of the callable.

For example, conceptually:

```cpp
thru(function, arg1, arg2)
```

When applied to a value `x`, the invocation is equivalent to:

```cpp
function(x, arg1, arg2);
```

This corresponds to Clojure's `->` macro.

### Thread-last: `operator>>`

Insert the value being threaded as the final argument of the callable.

For example, conceptually:

```cpp
thru(function, arg1, arg2)
```

When applied to a value `x`, the invocation is equivalent to:

```cpp
function(arg1, arg2, x);
```

This corresponds to Clojure's `->>` macro.

The same `thru` factory must support both threading directions.

Both operators must support zero or more additional arguments.

## 3. Variadic Arguments

The `thru` factory must accept:

- A callable.
- Zero or more additional arguments.

There must be no artificial limit on the number of additional arguments.

The additional arguments must be stored in the adapter and expanded when the callable is invoked.

Both threading operators must correctly handle all supported argument counts.

Use variadic templates and standard C++ mechanisms rather than fixed-arity overloads.

## 4. Pipeline Chaining and Operator Consistency

Both operators must support fluent, arbitrarily long pipeline chains.

The result of each operation becomes the input to the next operation.

A pipeline must use exclusively one threading operator:

- `>` for a thread-first pipeline.
- `>>` for a thread-last pipeline.

**Mixing `>` and `>>` within the same pipeline is unsupported and must be rejected at compile time.**

This is an intentional design constraint, consistent with the distinction between Clojure's `->` and `->>` macros.

Do not attempt to support mixed-direction pipelines by relying on parentheses or by changing the interpretation of C++ operator precedence.

### Compile-time enforcement of pipeline mode

The implementation must enforce the selected threading direction through the pipeline's compile-time type.

- The first threading operator establishes the pipeline mode.
- Subsequent stages must use that same operator.
- A subsequent use of the other operator must fail to compile.
- The implementation must not rely solely on documentation or runtime checks to enforce this rule.

Use an internal pipeline type or equivalent compile-time mechanism to encode the selected mode.

The implementation should use constrained overloads, concepts, or another appropriate standard C++ mechanism to make invalid operator combinations unavailable.

### Operator precedence

Do not assume overloaded operators change C++ operator precedence or associativity.

Both `>` and `>>` are left-associative, but `>>` has higher precedence than `>`.

Because mixed-direction pipelines are unsupported, the intended syntax avoids the precedence ambiguity associated with mixing these operators.

Explain the precedence implications and confirm that the supported syntax evaluates stages in the intended left-to-right order.

## 5. Pipeline Result Extraction and Return-Type Preservation

The pipeline must use an internal wrapper to retain its threading mode and support compile-time validation of subsequent stages.

The final result must be extracted using `.value()`.

For example:

```cpp
auto result = (
    input
    > thru(function_a)
    > thru(function_b)
).value();
```

The internal pipeline wrapper must not become the externally observable final result.

### Requirements for `.value()`

The `.value()` operation must:

- Return the actual result of the final callable, not the pipeline wrapper.
- Preserve value returns without introducing unnecessary copies or moves.
- Preserve lvalue-reference returns.
- Preserve rvalue-reference returns where appropriate.
- Preserve the final result's cv-qualification and value category as required by normal C++ return semantics.
- Use `decltype(auto)` or equivalent techniques where appropriate.
- Provide appropriate lvalue-qualified, const-lvalue-qualified, and rvalue-qualified overloads if needed.

The result of `.value()` should have the same effective type and value category as the result of directly invoking the final callable with the preceding stage's result.

The following declarations must behave according to normal C++ type deduction rules:

```cpp
auto result = pipeline.value();
const auto result = pipeline.value();
decltype(auto) result = pipeline.value();
```

In particular:

- `auto` deduces a value type, dropping references and top-level cv-qualification.
- `const auto` deduces a const-qualified value type.
- `decltype(auto)` preserves the exact deduced type, including references.

Include compile-time tests using `decltype`, `std::same_as`, and `std::is_same_v` to verify the return types of `.value()` for value, lvalue-reference, and rvalue-reference results.

Explain why the internal pipeline wrapper is necessary for compile-time mode enforcement and why `.value()` is the explicit boundary between the internal pipeline representation and the final result.

## 6. C++ Value Categories and Ownership Semantics

The implementation must respect C++ value categories, perfect forwarding, and move semantics.

Distinguish between lvalue and rvalue pipeline adapters.

### Lvalue adapters

When a `thru` adapter is an lvalue:

- Its stored callable should be invoked as an lvalue.
- Its stored arguments should be passed as lvalues.
- The adapter should be reusable.
- Stored arguments should not be moved from during invocation.
- Move-only arguments may be passed by reference where appropriate, but should not be implicitly transferred.

Constness must also be respected. A const lvalue adapter should expose its stored callable and arguments as const lvalues.

### Rvalue adapters

When a `thru` adapter is an rvalue:

- Its stored callable should be forwarded as an rvalue.
- Its stored arguments should be forwarded as rvalues.
- The adapter may be consumed.
- Move-only callables and arguments, such as `std::unique_ptr`, must be supported.
- Reusing a consumed adapter must not be assumed safe.

Use ref-qualified member functions and appropriate forwarding-reference overloads to implement these semantics.

Apply equivalent value-category handling to the stored callable itself, not just its arguments.

### Pipeline value categories

Apply consistent value-category handling to the pipeline's current result and its stored adapters.

- An lvalue pipeline should expose its current result as an lvalue when applying the next stage.
- A const lvalue pipeline should respect constness.
- An rvalue pipeline should forward its current result as an rvalue when applying the next stage.
- An rvalue pipeline may consume its stored state.
- `.value()` must respect the value category of the pipeline object.

Ensure that chaining does not accidentally turn lvalue results into rvalues or rvalue results into lvalues.

## 7. Compile-Time Pipeline Type Safety (Fundamental Requirement)

Compile-time type safety is a fundamental design requirement, not an optional enhancement.

The compiler must validate the type compatibility of every stage in the pipeline before execution.

Specifically:

- Each stage's output type must be compatible with the input parameter expected by the next stage.
- Invalid pipelines must fail to compile rather than producing runtime errors.
- Type checking must account for the distinction between thread-first and thread-last argument placement.
- Validate that each callable is invocable with the threaded value and its additional arguments in the appropriate positions.
- Respect normal C++ overload resolution and implicit conversions where appropriate.
- Preserve value-category information (lvalue, const lvalue, and rvalue) when determining invocability.
- Account for the callable's constness and ref qualification.
- Validate compatibility across the entire pipeline, not merely individual stages in isolation.
- Ensure that intermediate return types, including references, are correctly propagated into subsequent stages.
- Ensure that the pipeline's selected threading mode is consistent across every stage.

Prefer C++ concepts, constraints, and compile-time invocability checks, such as `std::invocable`, `std::is_invocable`, or equivalent mechanisms.

Avoid relying on errors deep inside template instantiation.

Provide useful compiler diagnostics where practical, identifying the pipeline stage and nature of the type incompatibility.

### Void-returning callables

Evaluate whether void-returning stages should be supported.

A void-returning stage cannot supply a value to the next pipeline stage. Recommend an appropriate design and explain the implications.

Unless there is a compelling reason to support them, prefer rejecting void-returning stages at compile time when they appear in a pipeline that requires a subsequent value.

If a final void-returning stage is supported, explain how `.value()` behaves in that case.

Include positive and negative compile-time tests demonstrating that valid pipelines compile and invalid pipelines are rejected.

The goal is to make the `thru` library behave like a statically type-checked functional composition mechanism.

## 8. Implementation Requirements

Use standard C++ facilities and prefer a straightforward, generic implementation.

Potentially useful facilities include:

- Variadic templates.
- Tuples.
- `std::invoke`.
- `std::apply`.
- Perfect forwarding.
- `std::forward`.
- Ref-qualified member functions.
- C++ concepts and constraints.
- Compile-time invocability checks.
- Type traits.
- `decltype(auto)`.
- Conditional `noexcept`.

Avoid unnecessary complexity and external dependencies.

Do not require C++26 reflection or other experimental language features.

Identify the minimum practical C++ standard required by the final implementation.

Ensure correct handling of:

- Free functions.
- Function objects.
- Ordinary lambdas.
- Generic lambdas.
- Member function pointers where practical.
- Move-only callables.
- Move-only arguments.

Prefer `std::invoke` so that member function pointers and other standard invocable forms are handled naturally where practical.

## 9. Argument Storage and Lifetime Management

Carefully evaluate the storage and lifetime characteristics of the initial thru value and the additional arguments passed to `thru`.

### Callable and additional argument storage

Consider:

- Whether callables and arguments should be stored by decayed value by default.
- How explicitly supplied references should be handled.
- Whether `std::reference_wrapper` should be supported.
- How temporary objects and move-only objects are handled.
- Whether the callable and its arguments should have consistent storage policies.

Prefer safe, predictable defaults.

A reasonable default is to store decayed values and allow explicit reference semantics through `std::ref` and `std::cref`, or an equivalent clearly documented mechanism.

Explain the consequences of the selected policy.

### Initial thru value storage

The initial value supplied before the first threading operator must also have a clearly defined storage and lifetime policy.

Evaluate whether to:

- Store rvalues by value and borrow lvalues by reference; or
- Store decayed values by default and require an explicit reference wrapper when borrowing is desired.

Choose one policy and apply it consistently.

The choice must account for:

- Move-only initial values.
- Non-copyable lvalue initial values.
- Const lvalue initial values.
- Temporary initial values.
- Pipeline objects that outlive the full expression in which they were created.

Do not silently retain references to temporary objects beyond their lifetime.

Document any lifetime responsibilities that remain with the caller.

### Intermediate results

Consider how intermediate values and references are represented in the pipeline.

Ensure that:

- Values are not copied or moved unnecessarily.
- References are propagated correctly.
- The implementation does not create dangling references through internal storage.
- Any unavoidable lifetime restrictions are clearly documented.

## 10. Return Types, Invocation Semantics, and Exceptions

Preserve the natural return semantics of the underlying callable.

Specifically:

- Preserve value returns.
- Preserve lvalue-reference returns.
- Preserve rvalue-reference returns where appropriate.
- Avoid unnecessary copies and moves.
- Use appropriate trailing return types or `decltype(auto)` where necessary.
- Consider exception specifications and conditional `noexcept`.
- Ensure that each pipeline stage receives the correct value category from the preceding stage.
- Ensure `.value()` exposes the final callable's result rather than an internal wrapper.

Explain any limitations, especially regarding reference lifetimes, void-returning functions, and consumed rvalue adapters.

## 11. Operator Overload Design

Evaluate the implications of overloading `operator>` and `operator>>`.

Consider:

- Appropriate template constraints to avoid unintended overload matches.
- Potential conflicts with built-in comparison and shift operators.
- Whether the overloads should be constrained to accept only the library's `thru` adapter or its own pipeline type.
- Whether the overloads should be defined as free functions or hidden friends.
- How to preserve the value category of both the thru value and the adapter.
- How to ensure that chaining works naturally.
- How to enforce pipeline mode consistency at compile time.

Avoid making the operators overly permissive.

Do not introduce overloads that interfere unnecessarily with ordinary comparisons or shift operations.

Explain the chosen overload strategy and its trade-offs.

## 12. Testing Requirements

Provide comprehensive tests covering the following areas.

### Basic threading

- Thread-first with zero additional arguments.
- Thread-first with one additional argument.
- Thread-first with multiple additional arguments.
- Thread-last with zero additional arguments.
- Thread-last with one additional argument.
- Thread-last with multiple additional arguments.

### Pipeline composition

- Multiple consecutive thread-first stages.
- Multiple consecutive thread-last stages.
- Arbitrarily long pipelines.
- Pipelines with different callable types.
- Pipelines with different intermediate return types.
- Correct left-to-right evaluation.
- Correct final result extraction using `.value()`.
- Rejection of a thread-first stage added to a thread-last pipeline.
- Rejection of a thread-last stage added to a thread-first pipeline.

### Value categories and ownership

- Lvalue thru values.
- Const lvalue thru values.
- Rvalue thru values.
- Lvalue pipeline adapters.
- Const lvalue pipeline adapters.
- Rvalue pipeline adapters.
- Reusable lvalue adapters.
- Consumed rvalue adapters.
- Move-only initial values.
- Move-only arguments.
- Move-only callables.
- Explicit reference arguments using `std::ref` or equivalent.
- Explicit const-reference arguments using `std::cref` or equivalent.

### Callable support

- Free functions.
- Function objects.
- Ordinary lambdas.
- Generic lambdas.
- Member function pointers where practical.
- Const callables.
- Ref-qualified callable operators.

### Compile-time type safety

- Valid pipelines that should compile.
- Invalid pipelines that should fail to compile.
- Thread-first argument incompatibility.
- Thread-last argument incompatibility.
- Incompatible intermediate return and input types.
- Constness incompatibility.
- Lvalue/rvalue reference incompatibility.
- Callable invocability constraints.
- Overload resolution and implicit conversion behavior.
- Pipeline mode inconsistency.
- Unsupported void-returning intermediate stages.

### Return-type preservation

Use compile-time assertions to verify:

- Value return types.
- Lvalue-reference return types.
- Rvalue-reference return types where appropriate.
- Correct behavior of `.value()` on lvalue and rvalue pipeline objects.
- Correct behavior of `auto`, `const auto`, and `decltype(auto)`.

Use `decltype`, `std::same_as`, and `std::is_same_v` as appropriate.

Use compile-fail tests where appropriate to demonstrate that invalid pipelines are rejected.

## 13. Expected Deliverables

Develop the library incrementally and provide:

1. A proposed API and explanation of its semantics.
2. An explanation of the operator precedence implications.
3. A clear explanation of the internal pipeline representation and why `.value()` is required.
4. A complete, compilable implementation.
5. An explanation of important design decisions and trade-offs.
6. A clear explanation of compile-time pipeline type checking.
7. Comprehensive unit tests.
8. Positive and negative compile-time tests.
9. An assessment of limitations and potential pitfalls.
10. The minimum required C++ standard and build instructions.

Prefer a small, self-contained library that can be incorporated into an existing C++ project.

Provide complete source code rather than pseudocode.

Where appropriate, separate the implementation and tests into distinct files and show how to build and run them.

## 14. Overall Design Philosophy

The primary objective is a small, idiomatic C++ library that brings the expressiveness of Clojure's threading macros into C++.

The library should combine:

- A simple API consisting of one `thru` factory.
- `operator>` for thread-first argument insertion.
- `operator>>` for thread-last argument insertion.
- Variadic support for arbitrary additional arguments.
- Natural fluent pipeline composition.
- A single, consistent threading direction per pipeline.
- Compile-time enforcement of pipeline mode.
- Correct C++ value-category and ownership semantics.
- Safe and predictable argument storage.
- Preservation of the final callable's return type and value category through `.value()`.
- Comprehensive compile-time type safety across pipeline stages.

**Compile-time type safety is a core requirement.** The library should provide the guarantees expected from statically typed functional composition: every stage must be validated against the actual output of its predecessor, with incompatible pipelines rejected during compilation.

Do not sacrifice API simplicity for unnecessary generality. Avoid introducing abstractions or features that do not materially improve usability, safety, or correctness.

Explain any compromises explicitly and justify them in terms of idiomatic C++ design.

If any requirements cannot be satisfied simultaneously or would require a significant usability trade-off, identify the conflict and recommend the simplest internally consistent solution before proceeding with implementation.