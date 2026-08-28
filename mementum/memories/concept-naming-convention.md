---
type: Decision
symbol: 🎯
title: concept-naming-convention
---
Adopt the C++ Core Guidelines for all C++ code, including naming.

- Concept identifiers use PascalCase (Core Guidelines `NL.17`), distinct from free/member functions (snake_case) and namespaces (lowercase).
- Nominal identity concepts keep the `Cljonic` domain prefix: `CljonicCollection`, `CljonicVector`, `CljonicMap`, `CljonicSet`, `CljonicQueue`, `CljonicString`.
- Capability concepts drop the prefix: `SequenceableCollection`, `IndexedCollection`, `AssociativeCollection`.
- Value capability concepts: `StableEqualityComparable`, `TotallyOrdered`.
- Storage/element concepts keep the `Element` suffix (no `Value` keyword): `CopyableElement`, `NothrowCopyableElement`, `NothrowElementConstruction`.
- Because cljonic users are developers, C++ concept identifiers are first-class vocabulary terms, not hidden under Examples.