#pragma once

/** \mainpage
 * \b cljonic is an \b embedded-first, \b header-only C++23 functional
 * collection library. It provides \b CopyOnModifyCollection semantics, \b
 * SentinelBasedAccess with \b ProbeFirstAccess discipline, and strict resource
 * determinism: <b>no heap allocation, no exceptions</b>.
 *
 * Element access that cannot produce a valid result returns a \b DefaultElement
 * (i.e., \c T{}) rather than throwing.  Callers use \b ProbeFirstAccess (\c
 * contains, \c has_index) before relying on the returned value in ambiguous
 * domains.
 *
 * ## Collections
 * - \ref Vector "cljonic::Vector"
 *
 * ## Core Functions
 * - \ref Core_Count "count"
 */

#include <cljonic-core-count.hpp>
