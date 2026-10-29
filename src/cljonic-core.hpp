#pragma once

#include <cljonic-config.hpp>

/** \mainpage
 * \b cljonic is a header-only C++ library for embedded system developers who want to program in a functional style. It
 * provides functions similar in name and behavior to Clojure.
 *
 * To address its embedded system focus, cljonic does not use the heap or exceptions. It uses modern C++ features to
 * protect, guide, and teach C++ developers, includes unit tests with nearly <b>100% coverage</b>, and is designed to
 * be efficiently maintained.
 *
 * The motivation for \b cljonic comes from observations made about the pragmatic use of C++ for embedded system
 * software development, the progression of the C++ language (e.g., C++11, C++14, C++17, C++20, C++23), and the counsel
 * of software development "experts", in many different programming languages, that <b>a functional programming style
 * should be eagerly embraced by all developers</b>.
 *
 * Many embedded system C++ developers are <b>not academic programming language experts</b> with in-depth knowledge of
 * every C++ feature, and many <b>may not have time</b> to follow the progression of the language.  Rather, many
 * embedded system C++ developers have <b>strong hardware backgrounds</B>, and <B>know a pragmatic specific subset of
 * C++</B> that is often sufficient to deliver reasonably good software in a predictable amount of time, though this is
 * often <b>not optimized for minimal long term cost of ownership</b>.
 *
 * Generally speaking, many C++ developers use a subset of the \b C programming language with some of C++'s <b>Object
 * Oriented</b> capabilities. This begs a question:
 *
 * <b>Is there a pragmatic subset of Modern C++ that enables embedded system developers to produce higher-quality, more
 * efficient, and more maintainable code?</b>
 *
 * Changes to C++ have provided valuable capabilities that are time consuming to master, hence it is both <b> valuable
 * to development organizations</b> for their developers to use modern C++ capabilities, and <b> too expensive to
 * train</b> all of their developers to be experts in their use.  This, too, begs a question:
 *
 * <b>Can one or two embedded system software developers cost-effectively introduce modern C++ capabilities to the
 * entire team?</b>
 *
 * In this post:
 *
 *     https://www.gamedeveloper.com/programming/in-depth-functional-programming-in-c-
 *
 * the author makes the following statements:
 *
 * <b>"A large fraction of the flaws in software development are due to programmers not fully understanding all the
 * possible states their code may execute in. In a multithreaded environment, the lack of understanding and the
 *  resulting problems are greatly amplified, almost to the point of panic if
 * you are paying attention. Programming in a functional style makes the state presented to your code explicit, which
 * makes it much easier to reason about
 *  [(i.e., Fewer mistakes, and less debugging)], and, in a completely pure
 * system, makes thread race conditions impossible."
 *
 * "No matter what language you work in, programming in a functional style provides benefits. You should do it whenever
 * it is convenient, and you should think hard about the decision when it isn't convenient."</b>
 *
 * \b cljonic is a \b header-only library that utilizes \b Modern \b C++ capabilities to provide a functional style C++
 * \b Clojure subset to \b embedded \b system developers, promotes the \b predictable \b development of \b higher \b
 * quality code \b faster, and can be \b cost-effectively \b maintained over time.
 *
 * \anchor Core_Cheatsheet
 * ___
 * # Cheatsheet
 *
 * ## Aliases
 *
 * Namespace                      | C++ "using" Statement
 * ------------------------------ | ---------------------------------------------
 * \ref Namespace_Core "Core"     | using **core** = cljonic::core;
 * \ref Namespace_Set "Set"       | using **set** = cljonic::set;
 * \ref Namespace_String "String" | using **string** = cljonic::string;
 *
 * ## Collection Types
 *
 * - \ref Map "Map", \ref MapEntry "MapEntry"
 * - \ref Queue "Queue"
 * - \ref Set "Set"
 * - \ref String "String"
 * - \ref Vector "Vector"
 *
 * ## Producer Types
 *
 * - \ref Cycle "Cycle"
 * - \ref Iterate "Iterate"
 * - \ref Range "Range"
 * - \ref Repeat "Repeat"
 * - \ref Repeatedly "Repeatedly"
 *
 * ## Variant Type
 *
 * - \ref Variant "Variant"
 *
 * ## Core Functions
 *
 * Implemented free functions link to their documentation. Remaining names are deferred functions that the requirements
 * govern but whose specification, tests, and implementation have not yet been propagated.
 *
 * - add, apply, \ref Assoc "assoc", assoc_in
 * - bit_and, bit_and_not, bit_clear, bit_flip, bit_not, bit_or, bit_set, bit_shift_left, bit_shift_right, bit_test,
 *   bit_xor, butlast
 * - can_add, \ref CanAssoc "can_assoc", \ref CanConj "can_conj", can_multiply, can_subtract,
 *   \ref CharacterIsValid "character_is_valid", comp, compare, complement, \ref Conj "conj", constantly,
 *   \ref Contains "contains", \ref Count "count", \ref Cycle "cycle"
 * - dec, dedupe, deref, \ref Disj "disj", \ref Dissoc "dissoc", distinct, div, double, drop, drop_last, drop_while
 * - emplace, \ref Empty "empty", \ref Equal "equal", every_pred
 * - ffirst, find, \ref First "first", \ref FitsInto "fits_into", fits_print, flatten, float, fnext, fnil, frequencies
 * - \ref Get "get", get_if, get_in, group_by
 * - \ref HasNext "has_next", has_some, \ref HashMapFactory "hash_map", \ref HashSetFactory "hash_set", holds
 * - identity, inc, index, int, interleave, interpose, \ref Into "into", \ref IsEmpty "is_empty", is_even, is_every,
 *   is_negative, is_odd, is_positive, is_zero, \ref Iterate "iterate"
 * - juxt
 * - keep, keep_indexed, keys
 * - make_keyword_enum_name_map, map, \ref MapEntryFactory "map_entry", map_indexed, mapcat, mapv, max, max_key,
 *   merge, merge_with, min, min_key, mod, mult
 * - negate, \ref Next "next", nfirst, nnext, not_any, \ref NotEmpty "not_empty", \ref NotEqual "not_equal",
 *   not_every, nth, nthnext, nthrest
 * - partial, partition, partition_all, partition_by, partitionv, partitionv_all, \ref Peek "peek", \ref Pop "pop",
 *   pr_str, print_to, prn_str
 * - \ref QueueFactory "queue", quot
 * - \ref RangeFactory "range", reduce_kv, reductions, rem, remove, \ref RepeatFactory "repeat", \ref Repeatedly "repeatedly",
 * replace, replicate, reset, reset_vals, \ref Rest "rest", rseq
 * - second, select_keys, seq, some, some_fn, sort, sort_by, split_at, split_with, str, \ref StringFactory "string",
 *   sub, subvec, swap, swap_vals
 * - take, take_last, take_nth, take_while, to_byte, to_char, to_double, to_float, to_int, to_long, to_num, to_short,
 *   tree_seq
 * - unsigned_bit_shift_right, update, update_in, update_keys, update_vals
 * - vals, vec, \ref VariantFactory "variant", variant_alternative, variant_size, \ref cljonic::vector "vector", visit
 * - zipmap
 *
 * ## Set Functions
 *
 * All entries below are deferred functions that the requirements govern but whose specification, tests, and
 * implementation have not yet been propagated.
 *
 * - difference
 * - fits_join, fits_set_algebra
 * - index, intersection, is_subset, is_superset
 * - join
 * - map_invert
 * - project
 * - rename, rename_keys
 * - select
 * - union
 *
 * ## String Functions
 *
 * All entries below are deferred functions that the requirements govern but whose specification, tests, and
 * implementation have not yet been propagated.
 *
 * - capitalize
 * - ends_with, escape
 * - has_re_find, has_re_matches
 * - includes, index_of, is_blank
 * - join
 * - last_index_of, lower_case
 * - parse_bool, parse_float, parse_int
 * - re_find, re_groups, re_matcher, re_matches, re_pattern, re_quote_replacement, re_seq, replace, replace_first,
 *   reverse
 * - split, split_lines, starts_with
 * - trim, trim_newline, triml, trimr
 * - upper_case
 */

/** \anchor Namespace_Core
 * The \b Core namespace provides the vast majority of \b cljonic functions. The current Core surface provides the
 * bounded collections and the active producers, and its free-function collection operations are introduced module by
 * module as their requirements are approved.
 */

#include <cljonic-assoc.hpp>
#include <cljonic-can-assoc.hpp>
#include <cljonic-can-conj.hpp>
#include <cljonic-character-is-valid.hpp>
#include <cljonic-concepts.hpp>
#include <cljonic-conj.hpp>
#include <cljonic-contains.hpp>
#include <cljonic-core-collection-maximum-element-count.hpp>
#include <cljonic-count.hpp>
#include <cljonic-cycle.hpp>
#include <cljonic-disj.hpp>
#include <cljonic-dissoc.hpp>
#include <cljonic-empty.hpp>
#include <cljonic-equal.hpp>
#include <cljonic-first.hpp>
#include <cljonic-fits-into.hpp>
#include <cljonic-get.hpp>
#include <cljonic-has-next.hpp>
#include <cljonic-into.hpp>
#include <cljonic-is-empty.hpp>
#include <cljonic-iterate.hpp>
#include <cljonic-map.hpp>
#include <cljonic-next.hpp>
#include <cljonic-not-empty.hpp>
#include <cljonic-not-equal.hpp>
#include <cljonic-peek.hpp>
#include <cljonic-pop.hpp>
#include <cljonic-queue.hpp>
#include <cljonic-range.hpp>
#include <cljonic-repeat.hpp>
#include <cljonic-repeatedly.hpp>
#include <cljonic-rest.hpp>
#include <cljonic-set.hpp>
#include <cljonic-string.hpp>
#include <cljonic-variant.hpp>
#include <cljonic-vector.hpp>

namespace cljonic::core {}
