---
type: Insight
symbol: 💡
title: doxygen-reflow-must-respect-list-markers
related: [doc-sample-extraction-requires-unindented-fence.md, docs-user-facing-example-style.md, doxygen-prose-must-be-user-facing.md, cljonic-doxygen-layout.md]
---
`scripts/format-doc-samples.pl` (`format_doxygen_block`) is the ONLY tool that
reflows Doxygen prose — `clang-format` treats `/** */` text as opaque and never
rewraps it. So comment layout correctness lives in that Perl script, not in
`make format`'s clang-format pass.

Bug found 2026-10-03: the paragraph logic joined every `* ` line and rewrapped,
so consecutive ONE-LINE bullets merged into a single list item
(`* - A. - B. - C.` → one bullet). Reproduced minimally; also seen as `equal`'s
first two bullets fusing. Doxygen renders the merged text, so the failure is
invisible in C++ and only shows in rendered docs.

Fix: a line matching `^[ \t]*\*\s?-\s` must be a paragraph BOUNDARY (flush the
current paragraph, start a new one). List integrity is a formatter
responsibility in this repo, not a hand-authoring invariant.

Rule for future doc-comment edits: after changing any `/** */` block, run
`make format` twice and diff — reflow bugs are only visible once the formatter
has run, and idempotency is the check.
