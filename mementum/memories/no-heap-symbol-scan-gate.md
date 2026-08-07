🔁 Three-layer no-heap verification in `make no-heap`:
1. `no-heap-src` — `rg` source scan blocking heap APIs and heap-backed STL containers in `src/`
2. Probe build — compiles `cljonic_no_heap_probe` dedicated harness (excluded from default test runs)
3. `no-heap-symbols` — `nm` binary scan for forbidden allocator symbols:
   - C: malloc/free/calloc/realloc/aligned_alloc/posix_memalign
   - C++: _Znwm/_Znam/_ZdlPv/_ZdaPv

Symbol filter: `nm binary | grep -E '^[0-9a-f]+ [UT] (forbidden_name)'`
`[UT]` covers undefined + text symbols; catches linked-in allocators.

`make upsert-gate-strict` calls `make no-heap` automatically.
Standalone: `make no-heap-symbols` requires probe already built first.
