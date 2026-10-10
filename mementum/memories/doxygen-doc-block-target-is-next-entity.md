---
type: Insight
symbol: 💡
title: doxygen-doc-block-target-is-next-entity
related: [cljonic-doxygen-layout.md, /mementum/knowledge/doxygen-doc-tooling.md]
---
Doxygen attaches a documentation block to the declaration immediately following
it. In headers that reopen `namespace concepts_detail`, placing a public
function's `/** ... */` block before that namespace documents the internal
namespace instead. Repeated reopenings then merge those descriptions and anchors
into the namespace page, while the public function loses its intended
documentation and links target the wrong page.

Keep implementation helpers first, close `namespace concepts_detail`, then put
the public API's documentation block immediately before its first public
declaration. After relocating a block, regenerate `docs/` and verify both its
anchor target and the internal namespace's Detailed Description.
