💡 User-facing cljonic doc examples should be contract-first and avoid implementation-detail terminology.

- Keep prose concise and non-speculative.
- Prefer named examples with `const auto name = Type{...};` for readability.
- In construction-only samples, avoid surfacing methods that are intended to be documented via standalone functions.
- For intentionally unused sample variables, prefer `[[maybe_unused]]` over `(void)var;`.
- Keep example include usage consistent with library guidance (use `cljonic-core.hpp` when that is the recommended umbrella include).
- Accept formatter-native wrapping unless formatting policy is explicitly changed.