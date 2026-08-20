---
type: Insight
symbol: 💡
title: free-function-addition-organization
---
When adding a new free function, organize it according to its contract scope before deciding where the tests live.

- If the function is a core API contract, like `count`, it belongs with the core free-function specs and core free-function tests.
- If the function is collection-specific behavior, keep it with the collection-specific spec and collection-specific tests.
- The deciding signal is the layer of truth: generic API contract vs concrete collection behavior.
- A function should not be placed in a collection test file just because the first implementation happens to be a single collection type.
- When the contract is generic but the implementation is narrow, keep the spec generic and the tests aligned to the general contract, even if the first implementation is Vector-only.

This project pattern keeps the durable truth stable and prevents collection-implementation drift from masquerading as a core API contract.
