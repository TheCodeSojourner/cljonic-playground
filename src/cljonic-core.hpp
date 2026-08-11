#ifndef CLJONIC_CORE_HPP
#define CLJONIC_CORE_HPP

/** \mainpage
 * \b cljonic is a \b header-only C++ library designed to allow an \b embedded
 * \b systems C++ developer to program in a \b Functional \b Programming \b
 * Style using a set of functions \b similar in \b name and \b function to \b
 * Clojure. To address the embedded systems focus, \b cljonic \b DOES \b NOT use
 * the heap or exceptions, and uses \b Modern \b C++ features to \b protect , \b
 * guide , and \b teach C++ developers. Finally, \b cljonic is a library that
 * includes unit tests with nearly <b>100% coverage</b>, and is designed to be
 * efficiently maintained.
 *
 * The motivation for \b cljonic comes from observations made about the
 * pragmatic use of C++ for embedded systems software development, the
 * progression of the C++ language (e.g., C++11, C++14, C++17, C++20, C++23),
 * and the counsel of software development "experts", in many different
 * programming languages, that <b>a functional programming style should be
 * eagerly embraced by all developers</b>.
 *
 * Many embedded systems C++ developers are <b>not academic programming language
 * experts</b> with in-depth knowledge of every C++ feature, and many <b>may not
 * have time</b> to follow the progression of the language.  Rather, many
 * embedded systems C++ developers have <b>strong hardware backgrounds</B>, and
 * <B>know a pragmatic specific subset of C++</B> that is often sufficient to
 * deliver reasonably good software in a predictable amount of time, though this
 * is often <b>not optimized for minimal long term cost of ownership</b>.
 * Generally speaking, many C++ developers use a subset of the \b C programming
 * language with some of C++'s <b>Object Oriented</b> capabilities.  This begs a
 * question:
 *
 * <b>Is there a pragmatic subset of Modern C++ that enables embedded systems
 * developers to produce higher-quality, more efficient, and more maintainable
 * code?</b>
 *
 * Changes to C++ have provided valuable capabilities that are time consuming to
 * master, hence it is both <b> valuable to development organizations</b> for
 * their developers to use modern C++ capabilities, and <b> too expensive to
 * train</b> all of their developers to be experts in their use.  This, too,
 * begs a question:
 *
 * <b>Can one or two embedded systems software developers cost-effectively
 * introduce modern C++ capabilities to the entire team?</b>
 *
 * In this post:
 *
 *     https://www.gamedeveloper.com/programming/in-depth-functional-programming-in-c-
 *
 * the author makes the following statements:
 *
 * <b>"A large fraction of the flaws in software development are due to
 * programmers not fully understanding all the possible states their code may
 * execute in. In a multithreaded environment, the lack of understanding and the
 *  resulting problems are greatly amplified, almost to the point of panic if
 * you are paying attention. Programming in a functional style makes the state
 * presented to your code explicit, which makes it much easier to reason about
 *  [(i.e., Fewer mistakes, and less debugging)], and, in a completely pure
 * system, makes thread race conditions impossible."
 *
 * "No matter what language you work in, programming in a functional style
 * provides benefits. You should do it whenever it is convenient, and you should
 * think hard about the decision when it isn't convenient."</b>
 *
 * \b cljonic is a \b header-only library that utilizes \b Modern \b C++
 * capabilities to provide a functional style C++ \b Clojure subset to \b
 * embedded \b systems developers, promotes the \b predictable \b development of
 * \b higher \b quality code \b faster, and can be \b cost-effectively \b
 * maintained over time.
 *
 * \anchor Core_Cheatsheet
 * ___
 * # Cheatsheet
 *
 * ## Aliases
 *
 * | Namespace | "using" C++ Statement |
 * |-----------|-------------------------|
 * | \ref Namespace_Core "core" | using **core** = cljonic::core; |
 * | \ref Namespace_Regex "regex" | using **regex** = cljonic::regex; |
 * | \ref Namespace_Set "set" | using **set** = cljonic::set; |
 * | \ref Namespace_String "string" | using **string** = cljonic::string; |
 *
 * ## Collection Types
 *
 * - \ref Vector "cljonic::Vector"
 * - \ref Range  "cljonic::Range"
 * - \ref Repeat "cljonic::Repeat"
 * - \ref Set    "cljonic::Set"
 * - \ref String "cljonic::String"
 *
 * ## Core Functions
 *
 * ### Composition
 *
 * - \ref Core_Partial "Partial"
 *
 * ### Control
 *
 * - \ref Core_DoSeq "DoSeq", \ref Core_DoTimes "DoTimes", \ref Core_If "If",
 * \ref Core_Unless "Unless", \ref Core_When "When", \ref Core_WhenNot "WhenNot"
 *
 * ### Predicate
 *
 * - \ref Core_Compare "Compare"
 * - \ref Core_EQ "EQ"
 * - \ref Core_GE "GE", \ref Core_GT "GT"
 * - \ref Core_IsAlpha "IsAlpha", \ref Core_IsAlphanumeric "IsAlphanumeric",
 * \ref Core_IsBlank "IsBlank", \ref Core_IsControl "IsControl", \ref
 * Core_IsDigit "IsDigit", \ref Core_IsEven "IsEven", \ref Core_IsGraphical
 * "IsGraphical", \ref Core_IsHexDigit "IsHexDigit", \ref Core_IsLowercase
 * "IsLowercase", \ref Core_IsNatural "IsNatural", \ref Core_IsNegative
 * "IsNegative", \ref Core_IsOdd "IsOdd", \ref Core_IsPositive "IsPositive",
 * \ref Core_IsPrintable "IsPrintable", \ref Core_IsPunctuation "IsPunctuation",
 * \ref Core_IsSpace "IsSpace", \ref Core_IsUppercase "IsUppercase", \ref
 * Core_IsZero "IsZero"
 * - \ref Core_LE "LE", \ref Core_LT "LT"
 * - \ref Core_NE "NE"
 *
 * ### Primitive
 *
 * - \ref Core_Abs "Abs", \ref Core_Add "Add"
 * - \ref Core_Char "Char", \ref Core_Combinations "Combinations", \ref
 * Core_CString "CString"
 * - \ref Core_Dec "Dec", \ref Core_Div "Div"
 * - \ref Core_Factorial "Factorial"
 * - \ref Core_Inc "Inc"
 * - \ref Core_Lowercase "Lowercase"
 * - \ref Core_Max "Max", \ref Core_Min "Min", \ref Core_Mod "Mod", \ref
 * Core_Mul "Mul"
 * - \ref Core_Ord "Ord"
 * - \ref Core_Permutations "Permutations"
 * - \ref Core_Sub "Sub",
 * - \ref Core_Uppercase "Uppercase"
 *
 * ### Seq (i.e., Sequence: Array, Range, Repeat, Set, And/Or String)
 *
 * - \ref Core_Compose "Compose", \ref Core_Concat "Concat", \ref Core_Conj
 * "Conj", \ref Core_Conj_M "Conj_M", \ref Core_Count "Count", \ref Core_Count_M
 * "Count_M", \ref Core_Cycle "Cycle"
 * - \ref Core_Dedupe "Dedupe", \ref Core_DedupeBy "DedupeBy", \ref
 * Core_DefaultElement "DefaultElement", \ref Core_DefaultElement_M
 * "DefaultElement_M", \ref Core_Different "Different", \ref Core_Distinct
 * "Distinct", \ref Core_DistinctBy "DistinctBy", \ref Core_Drop "Drop", \ref
 * Core_DropLast "DropLast", \ref Core_DropWhile "DropWhile"
 * - \ref Core_Empty_M "Empty_M", \ref Core_Equal "Equal", \ref Core_EqualBy
 * "EqualBy", \ref Core_Every "Every"
 * - \ref Core_Filter "Filter", \ref Core_First "First", \ref Core_Flatten
 * "Flatten", \ref Core_FlattenSize "FlattenSize", \ref Core_Frequencies
 * "Frequencies", \ref Core_FrequenciesBy "FrequenciesBy"
 * - \ref Core_Identical "Identical", \ref Core_Identity "Identity", \ref
 * Core_IndexOf "IndexOf", \ref Core_IndexOfBy "IndexOfBy", \ref Core_Interleave
 * "Interleave", \ref Core_Interpose "Interpose", \ref Core_IsDistinct
 * "IsDistinct", \ref Core_IsDistinctBy "IsDistinctBy", \ref Core_IsEmpty
 * "IsEmpty", \ref Core_IsFull "IsFull", \ref Core_Iterate "Iterate"
 * - \ref Core_Juxt "Juxt"
 * - \ref Core_Last "Last", \ref Core_LastIndexOf "LastIndexOf", \ref
 * Core_LastIndexOfBy "LastIndexOfBy"
 * - \ref Core_Map "Map", \ref Core_Max "Max", \ref Core_MaxBy "MaxBy", \ref
 * Core_Min "Min", \ref Core_MinBy "MinBy"
 * - \ref Core_NotAny "NotAny", \ref Core_NotEvery "NotEvery", \ref Core_Nth
 * "Nth", \ref Core_Nth_M "Nth_M"
 * - \ref Core_Partition "Partition", \ref Core_PartitionAll "PartitionAll",
 * \ref Core_PartitionBy "PartitionBy"
 * - \ref Core_Reduce "Reduce", \ref Core_Reductions "Reductions", \ref
 * Core_Remove "Remove", \ref Core_Replace "Replace", \ref Core_Reverse
 * "Reverse"
 * - \ref Core_Second "Second", \ref Core_Seq "Seq", \ref Core_Size "Size", \ref
 * Core_Some "Some", \ref Core_Sort "Sort", \ref Core_SortBy "SortBy", \ref
 * Core_SplitAt "SplitAt", \ref Core_SplitWith "SplitWith", \ref Core_Subs
 * "Subs"
 * - \ref Core_Take "Take", \ref Core_TakeLast "TakeLast", \ref Core_TakeNth
 * "TakeNth", \ref Core_TakeWhile "TakeWhile"
 *
 * ## Regex Functions
 *
 * - \ref Regex_ReFind "ReFind", \ref Regex_ReSeq "ReSeq"
 *
 * ## Set Functions
 *
 * - \ref Set_Conj "Conj", \ref Set_ConjBy "ConjBy", \ref Set_Contains
 * "Contains", \ref Set_ContainsBy "ContainsBy"
 * - \ref Set_Difference "Difference", \ref Set_DifferenceBy "DifferenceBy",
 * \ref Set_Disj "Disj", \ref Set_DisjBy "DisjBy"
 * - \ref Set_IndexOf "IndexOf", \ref Set_IndexOfBy "IndexOfBy", \ref
 * Set_Intersection "Intersection", \ref Set_IntersectionBy "IntersectionBy"
 * - \ref Set_ToSet "ToSet", \ref Set_ToSetBy "ToSetBy"
 * - \ref Set_Union "Union", \ref Set_UnionBy "UnionBy"
 *
 * ## String Functions
 *
 * - \ref String_Capitalize "Capitalize", \ref String_Compare "Compare", \ref
 * String_Concat "Concat"
 * - \ref String_EndsWith "EndsWith"
 * - \ref String_FloatToString "FloatToString", \ref String_Format "Format"
 * - \ref String_Includes "Includes", \ref String_IndexOf "IndexOf", \ref
 * String_Interpose "Interpose", \ref String_IntToString "IntToString", \ref
 * String_IntToStringHex "IntToStringHex", \ref String_IsBlank "IsBlank"
 * - \ref String_LastIndexOf "LastIndexOf", \ref String_Lowercase "Lowercase"
 * - \ref String_Partition "Partition", \ref String_PartitionAll "PartitionAll",
 * \ref String_PartitionBy "PartitionBy"
 * - \ref String_Replace "Replace", \ref String_ReplaceFirst "ReplaceFirst"
 * - \ref String_Split "Split", \ref String_SplitCount "SplitCount", \ref
 * String_SplitLine "SplitLine", \ref String_StartsWith "StartsWith", \ref
 * String_StrLen "StrLen", \ref String_StrOfChar "StrOfChar", \ref
 * String_StrToFloat "StrToFloat", \ref String_StrToIntDecimal
 * "StrToIntDecimal", \ref String_StrToIntHex "StrToIntHex", \ref String_Subs
 * "Subs"
 * - \ref String_Trim "Trim", \ref String_TrimLeft "TrimLeft", \ref
 * String_TrimNewLine "TrimNewLine", \ref String_TrimRight "TrimRight"
 * - \ref String_Uppercase "Uppercase"
 */

/** \anchor Namespace_Core
 * The \b Core namespace provides the vast majority of \b cljonic functions.
 * Many \b Core functions deal with \ref Array, \ref Range, \ref Repeat, \ref
 * Set, and \ref String, collections. The variety and capability of the \b Core
 * functions provide much of the overall value of the <b>cljonic functional
 * style of programming</b>.
 */

#include <cljonic-concepts.hpp>
#include <cljonic-core-count.hpp>
#include <cljonic-vector.hpp>

namespace cljonic {

namespace core {} // namespace core

} // namespace cljonic

#endif // CLJONIC_CORE_HPP
