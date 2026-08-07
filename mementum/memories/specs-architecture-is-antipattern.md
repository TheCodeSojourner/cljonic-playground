❌ specs/architecture/ is an antipattern — deleted 2026-08-07

Architecture constrains specs; expressing architecture as allium specs breaks the authority chain (arch > spec > tests > code). The spec-to-code traceability chain has no production-code leg for architecture entities, producing zombie traceability stubs (SUCCEED() placeholders with no behavioral content).

Architecture lives authoritatively in architecture.md only. Do not recreate specs/architecture/ via gybis-arch-propagate or any other means. If gybis-arch-propagate is invoked, its output target should be specs/collections/ (or future feature domains), not an architecture category.
