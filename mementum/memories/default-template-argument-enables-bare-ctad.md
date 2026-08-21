---
type: Insight
symbol: 💡
title: default-template-argument-enables-bare-ctad
---
Giving a class template a default template argument (`template <std::integral value_type = int> class Range`) lets bare-brace CTAD (`Range{}`) deduce that default, while explicit-argument CTAD (`Range{5L}`) still deduces normally from the arguments. No extra deduction guide is needed for the zero-argument constructor. Useful for giving a Clojure-style `(range)` default without sacrificing per-call-site type flexibility.
