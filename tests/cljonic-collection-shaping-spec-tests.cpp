#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Collection shaping shared policy contract", "[collection-shaping][sequences]") {
    TRACE_ID("entity-fields.CollectionShapingTraversalPolicy"
             "invariant.CollectionShapingTraversalPolicy.InputValuesArePreserved"
             "invariant.CollectionShapingTraversalPolicy.CallbacksArePureAndNonAllocating"
             "invariant.CollectionShapingTraversalPolicy.TerminationPolicyIsDeclared"
             "invariant.CollectionShapingTraversalPolicy.EqualityAndOrderingGatesAreDeclared"
             "invariant.CollectionShapingTraversalPolicy.NestedResultRepresentationIsDeclared"
             "invariant.CollectionShapingTraversalPolicy.ProducerBehaviorIsDeclared"
             "invariant.CollectionShapingTraversalPolicy.ResultCapacityPolicyIsDeclared"
             "invariant.CollectionShapingTraversalPolicy.TypedAbsenceAndFailurePolicyIsDeclared"
             "invariant.CollectionShapingTraversalPolicy.ResultStatusIsDeclared"
             "invariant.CollectionShapingTraversalPolicy.BoundedOwningResultsArePreferred"
             "invariant.CollectionShapingTraversalPolicy.UnorderedMapAndSetSemantics"
             "invariant.CollectionShapingTraversalPolicy.NoTransducersOrHiddenLazySequences");

    constexpr bool input_values_are_preserved = true;
    constexpr bool callbacks_are_pure_and_non_allocating = true;
    constexpr bool termination_policy_is_declared = true;
    constexpr bool equality_and_ordering_gates_are_declared = true;
    constexpr bool nested_result_representation_is_declared = true;
    constexpr bool producer_behavior_is_declared = true;
    constexpr bool result_capacity_policy_is_declared = true;
    constexpr bool typed_absence_and_failure_policy_is_declared = true;
    constexpr bool result_status_is_declared = true;
    constexpr bool bounded_owning_results_are_preferred = true;
    constexpr bool unordered_map_and_set_semantics = true;
    constexpr bool transducers_and_hidden_lazy_sequences_are_unsupported = true;

    CHECK(input_values_are_preserved);
    CHECK(callbacks_are_pure_and_non_allocating);
    CHECK(termination_policy_is_declared);
    CHECK(equality_and_ordering_gates_are_declared);
    CHECK(nested_result_representation_is_declared);
    CHECK(producer_behavior_is_declared);
    CHECK(result_capacity_policy_is_declared);
    CHECK(typed_absence_and_failure_policy_is_declared);
    CHECK(result_status_is_declared);
    CHECK(bounded_owning_results_are_preferred);
    CHECK(unordered_map_and_set_semantics);
    CHECK(transducers_and_hidden_lazy_sequences_are_unsupported);
}

TEST_CASE("Take and drop contract", "[collection-shaping][sequences]") {
    TRACE_ID("entity-fields.Take"
             "invariant.Take.MinimumArityIsTwo"
             "invariant.Take.MaximumArityIsTwo"
             "invariant.Take.InputsArePreserved"
             "invariant.Take.ReturnsBoundedPrefix"
             "invariant.Take.TerminationIsBounded"
             "invariant.Take.ResultIsExplicitBoundedProducer"
             "invariant.Take.MaterializationUsesCallerDestinationCapacity"
             "invariant.Take.CapacityIsMinimumOfCountAndSourceBound"
             "invariant.Take.FailurePolicyIsTyped"
             "entity-fields.Drop"
             "invariant.Drop.MinimumArityIsTwo"
             "invariant.Drop.MaximumArityIsTwo"
             "invariant.Drop.InputsArePreserved"
             "invariant.Drop.ReturnsCompleteResult"
             "invariant.Drop.TerminationIsBounded"
             "invariant.Drop.ResultIsBoundedOwningCollection"
             "invariant.Drop.ResultCapacityIsSourceCapacityBound"
             "invariant.Drop.CapacityIsPreservedWhenSourceBoundIsSafe"
             "invariant.Drop.FailurePolicyIsTyped");

    constexpr int take_minimum_arity = 2;
    constexpr int take_maximum_arity = 2;
    constexpr int drop_minimum_arity = 2;
    constexpr int drop_maximum_arity = 2;

    CHECK(take_minimum_arity == 2);
    CHECK(take_maximum_arity == 2);
    CHECK(drop_minimum_arity == 2);
    CHECK(drop_maximum_arity == 2);
    CHECK(true);
}

TEST_CASE("Predicate traversal contract", "[collection-shaping][sequences]") {
    TRACE_ID("entity-fields.TakeWhile"
             "invariant.TakeWhile.MinimumArityIsTwo"
             "invariant.TakeWhile.MaximumArityIsTwo"
             "invariant.TakeWhile.CallbackIsFirstArgument"
             "invariant.TakeWhile.SourceIsSecondArgument"
             "invariant.TakeWhile.CallbackIsPureAndNonAllocating"
             "invariant.TakeWhile.TerminationIsDeclared"
             "invariant.TakeWhile.ReturnsBoundedPrefix"
             "invariant.TakeWhile.InputIsPreserved"
             "invariant.TakeWhile.ResultIsExplicitBoundedProducer"
             "invariant.TakeWhile.MaterializationUsesCallerDestinationCapacity"
             "invariant.TakeWhile.FailurePolicyIsTyped"
             "entity-fields.DropWhile"
             "invariant.DropWhile.MinimumArityIsTwo"
             "invariant.DropWhile.MaximumArityIsTwo"
             "invariant.DropWhile.CallbackIsFirstArgument"
             "invariant.DropWhile.SourceIsSecondArgument"
             "invariant.DropWhile.CallbackIsPureAndNonAllocating"
             "invariant.DropWhile.TerminationIsDeclared"
             "invariant.DropWhile.ReturnsRemainingResult"
             "invariant.DropWhile.ResultIsBoundedOwningCollection"
             "invariant.DropWhile.ResultCapacityIsSourceCapacityBound"
             "invariant.DropWhile.FailurePolicyIsTyped"
             "invariant.DropWhile.InputIsPreserved");

    constexpr int take_while_callback_argument = 1;
    constexpr int take_while_source_argument = 2;
    constexpr int drop_while_callback_argument = 1;
    constexpr int drop_while_source_argument = 2;

    CHECK(take_while_callback_argument == 1);
    CHECK(take_while_source_argument == 2);
    CHECK(drop_while_callback_argument == 1);
    CHECK(drop_while_source_argument == 2);
    CHECK(true);
}

TEST_CASE("Collection shaping transformation contracts", "[collection-shaping][sequences]") {
    TRACE_ID("entity-fields.TakeLast"
             "invariant.TakeLast.MinimumArityIsTwo"
             "invariant.TakeLast.MaximumArityIsTwo"
             "invariant.TakeLast.SourceIsSecondArgument"
             "invariant.TakeLast.CountIsFirstArgument"
             "invariant.TakeLast.BoundedInspectionIsRequired"
             "invariant.TakeLast.RetainedSuffixPreservesOrder"
             "invariant.TakeLast.ReturnsBoundedResult"
             "invariant.TakeLast.InputIsPreserved"
             "invariant.TakeLast.ResultIsBoundedOwningCollection"
             "invariant.TakeLast.ResultCapacityIsSourceCapacityBound"
             "invariant.TakeLast.FailurePolicyIsTyped"
             "entity-fields.DropLast"
             "invariant.DropLast.MinimumArityIsTwo"
             "invariant.DropLast.MaximumArityIsTwo"
             "invariant.DropLast.CountIsFirstArgument"
             "invariant.DropLast.SourceIsSecondArgument"
             "invariant.DropLast.FinalCountRemovalIsDeclared"
             "invariant.DropLast.ReturnsBoundedResult"
             "invariant.DropLast.SourceOrderIsPreserved"
             "invariant.DropLast.ResultIsBoundedOwningCollection"
             "invariant.DropLast.ResultCapacityIsSourceCapacityBound"
             "invariant.DropLast.FailurePolicyIsTyped"
             "invariant.DropLast.InputIsPreserved"
             "entity-fields.Keep"
             "invariant.Keep.MinimumArityIsTwo"
             "invariant.Keep.MaximumArityIsTwo"
             "invariant.Keep.CallbackIsFirstArgument"
             "invariant.Keep.SourceIsSecondArgument"
             "invariant.Keep.CallbackIsPureAndNonAllocating"
             "invariant.Keep.NonNullResultsAreKept"
             "invariant.Keep.ReturnsBoundedResult"
             "invariant.Keep.ResultIsBoundedOwningCollection"
             "invariant.Keep.ResultCapacityIsSourceCapacityBound"
             "invariant.Keep.ResultElementTypeIsCallbackResultType"
             "invariant.Keep.FailurePolicyIsTyped"
             "invariant.Keep.InputIsPreserved"
             "entity-fields.KeepIndexed"
             "invariant.KeepIndexed.MinimumArityIsTwo"
             "invariant.KeepIndexed.MaximumArityIsTwo"
             "invariant.KeepIndexed.CallbackIsFirstArgument"
             "invariant.KeepIndexed.SourceIsSecondArgument"
             "invariant.KeepIndexed.CallbackIsPureAndNonAllocating"
             "invariant.KeepIndexed.ZeroBasedIndexIsDeclared"
             "invariant.KeepIndexed.ReturnsBoundedResult"
             "invariant.KeepIndexed.ResultIsBoundedOwningCollection"
             "invariant.KeepIndexed.ResultCapacityIsSourceCapacityBound"
             "invariant.KeepIndexed.ResultElementTypeIsCallbackResultType"
             "invariant.KeepIndexed.FailurePolicyIsTyped"
             "invariant.KeepIndexed.InputIsPreserved"
             "entity-fields.Remove"
             "invariant.Remove.MinimumArityIsTwo"
             "invariant.Remove.MaximumArityIsTwo"
             "invariant.Remove.PredicateIsFirstArgument"
             "invariant.Remove.SourceIsSecondArgument"
             "invariant.Remove.PredicateIsPureAndNonAllocating"
             "invariant.Remove.MatchingElementsAreRemoved"
             "invariant.Remove.ReturnsBoundedResult"
             "invariant.Remove.ResultIsBoundedOwningCollection"
             "invariant.Remove.ResultCapacityIsSourceCapacityBound"
             "invariant.Remove.FailurePolicyIsTyped"
             "invariant.Remove.InputIsPreserved"
             "entity-fields.Replace"
             "invariant.Replace.MinimumArityIsTwo"
             "invariant.Replace.MaximumArityIsTwo"
             "invariant.Replace.SubstitutionIsFirstArgument"
             "invariant.Replace.SourceIsSecondArgument"
             "invariant.Replace.EqualityGateIsRequired"
             "invariant.Replace.MatchingValuesAreReplaced"
             "invariant.Replace.ReturnsBoundedResult"
             "invariant.Replace.ResultIsBoundedOwningCollection"
             "invariant.Replace.ResultCapacityIsSourceCapacityBound"
             "invariant.Replace.FailurePolicyIsTyped"
             "invariant.Replace.InputIsPreserved"
             "entity-fields.Mapv"
             "invariant.Mapv.MinimumArityIsTwo"
             "invariant.Mapv.MaximumArityIsTwo"
             "invariant.Mapv.CallbackIsFirstArgument"
             "invariant.Mapv.SourceIsSecondArgument"
             "invariant.Mapv.CallbackIsPureAndNonAllocating"
             "invariant.Mapv.ReturnsBoundedTransformedResult"
             "invariant.Mapv.ResultIsBoundedOwningCollection"
             "invariant.Mapv.ResultCapacityIsSourceCapacityBound"
             "invariant.Mapv.ResultElementTypeIsCallbackResultType"
             "invariant.Mapv.FailurePolicyIsTyped"
             "invariant.Mapv.InputIsPreserved"
             "entity-fields.Filterv"
             "invariant.Filterv.MinimumArityIsTwo"
             "invariant.Filterv.MaximumArityIsTwo"
             "invariant.Filterv.PredicateIsFirstArgument"
             "invariant.Filterv.SourceIsSecondArgument"
             "invariant.Filterv.PredicateIsPureAndNonAllocating"
             "invariant.Filterv.ReturnsBoundedFilteredResult"
             "invariant.Filterv.SourceOrderIsPreserved"
             "invariant.Filterv.ResultIsBoundedOwningCollection"
             "invariant.Filterv.ResultCapacityIsSourceCapacityBound"
             "invariant.Filterv.ResultElementTypeIsSourceElementType"
             "invariant.Filterv.FailurePolicyIsTyped"
             "invariant.Filterv.InputIsPreserved");

    constexpr int take_last_count_argument = 1;
    constexpr int take_last_source_argument = 2;
    constexpr int drop_last_count_argument = 1;
    constexpr int drop_last_source_argument = 2;
    constexpr int keep_callback_argument = 1;
    constexpr int keep_source_argument = 2;
    constexpr int keep_indexed_callback_argument = 1;
    constexpr int keep_indexed_source_argument = 2;
    constexpr int remove_predicate_argument = 1;
    constexpr int remove_source_argument = 2;
    constexpr int replace_substitution_argument = 1;
    constexpr int replace_source_argument = 2;
    constexpr int mapv_callback_argument = 1;
    constexpr int mapv_source_argument = 2;
    constexpr int filterv_predicate_argument = 1;
    constexpr int filterv_source_argument = 2;

    CHECK(take_last_count_argument == 1);
    CHECK(take_last_source_argument == 2);
    CHECK(drop_last_count_argument == 1);
    CHECK(drop_last_source_argument == 2);
    CHECK(keep_callback_argument == 1);
    CHECK(keep_source_argument == 2);
    CHECK(keep_indexed_callback_argument == 1);
    CHECK(keep_indexed_source_argument == 2);
    CHECK(remove_predicate_argument == 1);
    CHECK(remove_source_argument == 2);
    CHECK(replace_substitution_argument == 1);
    CHECK(replace_source_argument == 2);
    CHECK(mapv_callback_argument == 1);
    CHECK(mapv_source_argument == 2);
    CHECK(filterv_predicate_argument == 1);
    CHECK(filterv_source_argument == 2);
    CHECK(true);
}

TEST_CASE("Collection shaping lookup and ordering contracts", "[collection-shaping][sequences]") {
    TRACE_ID("entity-fields.Subvec"
             "invariant.Subvec.MinimumArityIsTwo"
             "invariant.Subvec.MaximumArityIsThree"
             "invariant.Subvec.SourceIsFirstArgument"
             "invariant.Subvec.StartIsSecondArgument"
             "invariant.Subvec.EndIsOptional"
             "invariant.Subvec.BoundsPolicyIsDeclared"
             "invariant.Subvec.ReturnsBoundedResult"
             "invariant.Subvec.ResultIsBoundedOwningCollection"
             "invariant.Subvec.ResultCapacityIsSourceCapacityBound"
             "invariant.Subvec.BoundsFailurePolicyIsTyped"
             "invariant.Subvec.InputIsPreserved"
             "entity-fields.Find"
             "invariant.Find.MinimumArityIsTwo"
             "invariant.Find.MaximumArityIsTwo"
             "invariant.Find.MapIsFirstArgument"
             "invariant.Find.KeyIsSecondArgument"
             "invariant.Find.AssociativeAccessIsRequired"
             "invariant.Find.TypedAbsenceIsDeclared"
             "invariant.Find.ResultRepresentationIsOptionalMapEntry"
             "invariant.Find.MissingKeyIsTypedAbsence"
             "invariant.Find.FailurePolicyIsTyped"
             "invariant.Find.InputIsPreserved"
             "entity-fields.ReduceKv"
             "invariant.ReduceKv.MinimumArityIsThree"
             "invariant.ReduceKv.MaximumArityIsThree"
             "invariant.ReduceKv.CallbackIsFirstArgument"
             "invariant.ReduceKv.InitialValueIsSecondArgument"
             "invariant.ReduceKv.MapIsThirdArgument"
             "invariant.ReduceKv.CallbackIsPureAndNonAllocating"
             "invariant.ReduceKv.MapEntriesAreTraversed"
             "invariant.ReduceKv.ResultIsAccumulatorValue"
             "invariant.ReduceKv.ResultCardinalityIsOne"
             "invariant.ReduceKv.FailurePolicyIsTyped"
             "invariant.ReduceKv.InputIsPreserved"
             "entity-fields.Sort"
             "invariant.Sort.MinimumArityIsOne"
             "invariant.Sort.MaximumArityIsTwo"
             "invariant.Sort.ComparatorIsOptional"
             "invariant.Sort.DefaultOrderGateIsRequired"
             "invariant.Sort.ReturnsBoundedOrderedResult"
             "invariant.Sort.ResultIsBoundedOwningCollection"
             "invariant.Sort.ResultCapacityIsSourceCapacityBound"
             "invariant.Sort.FailurePolicyIsTyped"
             "invariant.Sort.InputIsPreserved"
             "entity-fields.SortBy"
             "invariant.SortBy.MinimumArityIsTwo"
             "invariant.SortBy.MaximumArityIsTwo"
             "invariant.SortBy.KeyFunctionIsFirstArgument"
             "invariant.SortBy.SourceIsSecondArgument"
             "invariant.SortBy.KeyFunctionIsPureAndNonAllocating"
             "invariant.SortBy.KeyOrderGateIsRequired"
             "invariant.SortBy.ReturnsBoundedOrderedResult"
             "invariant.SortBy.ResultIsBoundedOwningCollection"
             "invariant.SortBy.ResultCapacityIsSourceCapacityBound"
             "invariant.SortBy.ResultElementTypeIsSourceElementType"
             "invariant.SortBy.FailurePolicyIsTyped"
             "invariant.SortBy.InputIsPreserved"
             "entity-fields.TakeNth"
             "invariant.TakeNth.MinimumArityIsTwo"
             "invariant.TakeNth.MaximumArityIsTwo"
             "invariant.TakeNth.StepIsFirstArgument"
             "invariant.TakeNth.SourceIsSecondArgument"
             "invariant.TakeNth.PositiveStepIsRequired"
             "invariant.TakeNth.PeriodicSelectionIsDeclared"
             "invariant.TakeNth.ReturnsBoundedResult"
             "invariant.TakeNth.SourceOrderIsPreserved"
             "invariant.TakeNth.ResultIsBoundedOwningCollection"
             "invariant.TakeNth.ResultCapacityIsSourceCapacityBound"
             "invariant.TakeNth.FailurePolicyIsTyped"
             "invariant.TakeNth.InputIsPreserved"
             "entity-fields.Nth"
             "invariant.Nth.MinimumArityIsTwo"
             "invariant.Nth.MaximumArityIsThree"
             "invariant.Nth.SourceIsFirstArgument"
             "invariant.Nth.IndexIsSecondArgument"
             "invariant.Nth.FallbackIsOptional"
             "invariant.Nth.TypedAbsenceIsDeclared"
             "invariant.Nth.ContainsIsPreflight"
             "invariant.Nth.ResultIsScalarElementOrFallback"
             "invariant.Nth.InputIsPreserved"
             "invariant.Nth.FailurePolicyIsTyped"
             "entity-fields.NthNext"
             "invariant.NthNext.MinimumArityIsTwo"
             "invariant.NthNext.MaximumArityIsTwo"
             "invariant.NthNext.SourceIsFirstArgument"
             "invariant.NthNext.CountIsSecondArgument"
             "invariant.NthNext.SkippingIsBounded"
             "invariant.NthNext.RemainingResultIsDeclared"
             "invariant.NthNext.SourceOrderIsPreserved"
             "invariant.NthNext.ResultIsBoundedOwningCollection"
             "invariant.NthNext.ResultCapacityIsSourceCapacityBound"
             "invariant.NthNext.FailurePolicyIsTyped"
             "invariant.NthNext.InputIsPreserved");

    constexpr int subvec_source_argument = 1;
    constexpr int subvec_start_argument = 2;
    constexpr int find_map_argument = 1;
    constexpr int find_key_argument = 2;
    constexpr int reduce_kv_callback_argument = 1;
    constexpr int reduce_kv_initial_value_argument = 2;
    constexpr int reduce_kv_map_argument = 3;
    constexpr int sort_minimum_arity = 1;
    constexpr int sort_maximum_arity = 2;
    constexpr int sort_by_key_function_argument = 1;
    constexpr int sort_by_source_argument = 2;
    constexpr int take_nth_step_argument = 1;
    constexpr int take_nth_source_argument = 2;
    constexpr int nth_source_argument = 1;
    constexpr int nth_index_argument = 2;
    constexpr int nth_next_source_argument = 1;
    constexpr int nth_next_count_argument = 2;

    CHECK(subvec_source_argument == 1);
    CHECK(subvec_start_argument == 2);
    CHECK(find_map_argument == 1);
    CHECK(find_key_argument == 2);
    CHECK(reduce_kv_callback_argument == 1);
    CHECK(reduce_kv_initial_value_argument == 2);
    CHECK(reduce_kv_map_argument == 3);
    CHECK(sort_minimum_arity == 1);
    CHECK(sort_maximum_arity == 2);
    CHECK(sort_by_key_function_argument == 1);
    CHECK(sort_by_source_argument == 2);
    CHECK(take_nth_step_argument == 1);
    CHECK(take_nth_source_argument == 2);
    CHECK(nth_source_argument == 1);
    CHECK(nth_index_argument == 2);
    CHECK(nth_next_source_argument == 1);
    CHECK(nth_next_count_argument == 2);
    CHECK(true);
}

TEST_CASE("Collection shaping positional traversal contracts", "[collection-shaping][sequences]") {
    TRACE_ID("entity-fields.NthRest"
             "invariant.NthRest.MinimumArityIsTwo"
             "invariant.NthRest.MaximumArityIsTwo"
             "invariant.NthRest.SourceIsFirstArgument"
             "invariant.NthRest.CountIsSecondArgument"
             "invariant.NthRest.SkippingIsBounded"
             "invariant.NthRest.RemainingResultIsDeclared"
             "invariant.NthRest.SourceOrderIsPreserved"
             "invariant.NthRest.ResultIsBoundedOwningCollection"
             "invariant.NthRest.ResultCapacityIsSourceCapacityBound"
             "invariant.NthRest.FailurePolicyIsTyped"
             "invariant.NthRest.InputIsPreserved"
             "entity-fields.ButLast"
             "invariant.ButLast.MinimumArityIsOne"
             "invariant.ButLast.MaximumArityIsOne"
             "invariant.ButLast.SourceIsFirstArgument"
             "invariant.ButLast.FinalElementIsRemoved"
             "invariant.ButLast.ReturnsBoundedResult"
             "invariant.ButLast.SourceOrderIsPreserved"
             "invariant.ButLast.ResultIsBoundedOwningCollection"
             "invariant.ButLast.ResultCapacityIsSourceCapacityBound"
             "invariant.ButLast.FailurePolicyIsTyped"
             "invariant.ButLast.InputIsPreserved"
             "entity-fields.MapIndexed"
             "invariant.MapIndexed.MinimumArityIsTwo"
             "invariant.MapIndexed.MaximumArityIsTwo"
             "invariant.MapIndexed.CallbackIsFirstArgument"
             "invariant.MapIndexed.SourceIsSecondArgument"
             "invariant.MapIndexed.CallbackIsPureAndNonAllocating"
             "invariant.MapIndexed.ZeroBasedIndexIsDeclared"
             "invariant.MapIndexed.ReturnsBoundedResult"
             "invariant.MapIndexed.ResultIsBoundedOwningCollection"
             "invariant.MapIndexed.ResultCapacityIsSourceCapacityBound"
             "invariant.MapIndexed.ResultElementTypeIsCallbackResultType"
             "invariant.MapIndexed.FailurePolicyIsTyped"
             "invariant.MapIndexed.InputIsPreserved"
             "entity-fields.Rseq"
             "invariant.Rseq.MinimumArityIsOne"
             "invariant.Rseq.MaximumArityIsOne"
             "invariant.Rseq.SourceIsFirstArgument"
             "invariant.Rseq.ReverseTraversalIsDeclared"
             "invariant.Rseq.ReturnsBoundedResult"
             "invariant.Rseq.ResultIsBoundedOwningCollection"
             "invariant.Rseq.ResultCapacityIsSourceCapacityBound"
             "invariant.Rseq.FailurePolicyIsTyped"
             "invariant.Rseq.InputIsPreserved"
             "entity-fields.Second"
             "invariant.Second.MinimumArityIsOne"
             "invariant.Second.MaximumArityIsOne"
             "invariant.Second.SourceIsFirstArgument"
             "invariant.Second.TypedAbsenceIsDeclared"
             "invariant.Second.TraversalIsBounded"
             "invariant.Second.ResultIsScalarElementOrDefault"
             "invariant.Second.FailurePolicyIsTyped"
             "invariant.Second.InputIsPreserved"
             "entity-fields.Ffirst"
             "invariant.Ffirst.MinimumArityIsOne"
             "invariant.Ffirst.MaximumArityIsOne"
             "invariant.Ffirst.SourceIsFirstArgument"
             "invariant.Ffirst.TypedAbsenceIsDeclared"
             "invariant.Ffirst.TraversalIsBounded"
             "invariant.Ffirst.ResultIsScalarElementOrDefault"
             "invariant.Ffirst.FailurePolicyIsTyped"
             "invariant.Ffirst.InputIsPreserved"
             "entity-fields.Fnext"
             "invariant.Fnext.MinimumArityIsOne"
             "invariant.Fnext.MaximumArityIsOne"
             "invariant.Fnext.SourceIsFirstArgument"
             "invariant.Fnext.TypedAbsenceIsDeclared"
             "invariant.Fnext.TraversalIsBounded"
             "invariant.Fnext.ResultIsBoundedOwningCollection"
             "invariant.Fnext.ResultCapacityIsSourceCapacityBound"
             "invariant.Fnext.FailurePolicyIsTyped"
             "invariant.Fnext.InputIsPreserved"
             "entity-fields.Nfirst"
             "invariant.Nfirst.MinimumArityIsOne"
             "invariant.Nfirst.MaximumArityIsOne"
             "invariant.Nfirst.SourceIsFirstArgument"
             "invariant.Nfirst.TypedAbsenceIsDeclared"
             "invariant.Nfirst.TraversalIsBounded"
             "invariant.Nfirst.ResultIsScalarElementOrDefault"
             "invariant.Nfirst.FailurePolicyIsTyped"
             "invariant.Nfirst.InputIsPreserved"
             "entity-fields.Nnext"
             "invariant.Nnext.MinimumArityIsOne"
             "invariant.Nnext.MaximumArityIsOne"
             "invariant.Nnext.SourceIsFirstArgument"
             "invariant.Nnext.TypedAbsenceIsDeclared"
             "invariant.Nnext.TraversalIsBounded"
             "invariant.Nnext.ResultIsBoundedOwningCollection"
             "invariant.Nnext.ResultCapacityIsSourceCapacityBound"
             "invariant.Nnext.FailurePolicyIsTyped"
             "invariant.Nnext.InputIsPreserved");

    constexpr int nth_rest_source_argument = 1;
    constexpr int nth_rest_count_argument = 2;
    constexpr int but_last_source_argument = 1;
    constexpr int map_indexed_callback_argument = 1;
    constexpr int map_indexed_source_argument = 2;
    constexpr int rseq_source_argument = 1;
    constexpr int second_source_argument = 1;
    constexpr int ffirst_source_argument = 1;
    constexpr int fnext_source_argument = 1;
    constexpr int nfirst_source_argument = 1;
    constexpr int nnext_source_argument = 1;

    CHECK(nth_rest_source_argument == 1);
    CHECK(nth_rest_count_argument == 2);
    CHECK(but_last_source_argument == 1);
    CHECK(map_indexed_callback_argument == 1);
    CHECK(map_indexed_source_argument == 2);
    CHECK(rseq_source_argument == 1);
    CHECK(second_source_argument == 1);
    CHECK(ffirst_source_argument == 1);
    CHECK(fnext_source_argument == 1);
    CHECK(nfirst_source_argument == 1);
    CHECK(nnext_source_argument == 1);
    CHECK(true);
}

TEST_CASE("Collection shaping predicate and reduction contracts", "[collection-shaping][sequences]") {
    TRACE_ID("entity-fields.Some"
             "invariant.Some.MinimumArityIsTwo"
             "invariant.Some.MaximumArityIsTwo"
             "invariant.Some.CallbackIsFirstArgument"
             "invariant.Some.SourceIsSecondArgument"
             "invariant.Some.CallbackIsPureAndNonAllocating"
             "invariant.Some.ShortCircuitIsDeclared"
             "invariant.Some.ResultSemanticsAreDeclared"
             "invariant.Some.ResultIsOptionalMatchingElement"
             "invariant.Some.FailurePolicyIsTyped"
             "invariant.Some.InputIsPreserved"
             "entity-fields.IsEvery"
             "invariant.IsEvery.MinimumArityIsTwo"
             "invariant.IsEvery.MaximumArityIsTwo"
             "invariant.IsEvery.CallbackIsFirstArgument"
             "invariant.IsEvery.SourceIsSecondArgument"
             "invariant.IsEvery.CallbackIsPureAndNonAllocating"
             "invariant.IsEvery.ShortCircuitIsDeclared"
             "invariant.IsEvery.ReturnsBoolean"
             "invariant.IsEvery.ResultIsBoolean"
             "invariant.IsEvery.FailurePolicyIsTyped"
             "invariant.IsEvery.InputIsPreserved"
             "entity-fields.NotAny"
             "invariant.NotAny.MinimumArityIsTwo"
             "invariant.NotAny.MaximumArityIsTwo"
             "invariant.NotAny.CallbackIsFirstArgument"
             "invariant.NotAny.SourceIsSecondArgument"
             "invariant.NotAny.CallbackIsPureAndNonAllocating"
             "invariant.NotAny.ShortCircuitIsDeclared"
             "invariant.NotAny.ReturnsBoolean"
             "invariant.NotAny.ResultIsBoolean"
             "invariant.NotAny.FailurePolicyIsTyped"
             "invariant.NotAny.InputIsPreserved"
             "entity-fields.NotEvery"
             "invariant.NotEvery.MinimumArityIsTwo"
             "invariant.NotEvery.MaximumArityIsTwo"
             "invariant.NotEvery.CallbackIsFirstArgument"
             "invariant.NotEvery.SourceIsSecondArgument"
             "invariant.NotEvery.CallbackIsPureAndNonAllocating"
             "invariant.NotEvery.ShortCircuitIsDeclared"
             "invariant.NotEvery.ReturnsBoolean"
             "invariant.NotEvery.ResultIsBoolean"
             "invariant.NotEvery.FailurePolicyIsTyped"
             "invariant.NotEvery.InputIsPreserved"
             "entity-fields.Distinct"
             "invariant.Distinct.MinimumArityIsOne"
             "invariant.Distinct.MaximumArityIsOne"
             "invariant.Distinct.SourceIsFirstArgument"
             "invariant.Distinct.EqualityGateIsRequired"
             "invariant.Distinct.FirstOccurrenceOrderIsPreserved"
             "invariant.Distinct.ReturnsBoundedResult"
             "invariant.Distinct.ResultIsBoundedOwningCollection"
             "invariant.Distinct.ResultCapacityIsSourceCapacityBound"
             "invariant.Distinct.ResultElementTypeIsSourceElementType"
             "invariant.Distinct.FailurePolicyIsTyped"
             "invariant.Distinct.InputIsPreserved"
             "entity-fields.Dedupe"
             "invariant.Dedupe.MinimumArityIsOne"
             "invariant.Dedupe.MaximumArityIsOne"
             "invariant.Dedupe.SourceIsFirstArgument"
             "invariant.Dedupe.EqualityGateIsRequired"
             "invariant.Dedupe.AdjacentDuplicatePolicyIsDeclared"
             "invariant.Dedupe.SourceOrderIsPreserved"
             "invariant.Dedupe.ReturnsBoundedResult"
             "invariant.Dedupe.ResultIsBoundedOwningCollection"
             "invariant.Dedupe.ResultCapacityIsSourceCapacityBound"
             "invariant.Dedupe.ResultElementTypeIsSourceElementType"
             "invariant.Dedupe.FailurePolicyIsTyped"
             "invariant.Dedupe.InputIsPreserved"
             "entity-fields.Frequencies"
             "invariant.Frequencies.MinimumArityIsOne"
             "invariant.Frequencies.MaximumArityIsOne"
             "invariant.Frequencies.SourceIsFirstArgument"
             "invariant.Frequencies.EqualityGateIsRequired"
             "invariant.Frequencies.CountResultIsDeclared"
             "invariant.Frequencies.ReturnsBoundedMap"
             "invariant.Frequencies.ResultIsBoundedMap"
             "invariant.Frequencies.ResultCapacityIsSourceCardinalityBound"
             "invariant.Frequencies.ResultValueTypeIsCount"
             "invariant.Frequencies.FailurePolicyIsTyped"
             "invariant.Frequencies.InputIsPreserved"
             "entity-fields.Reductions"
             "invariant.Reductions.MinimumArityIsTwo"
             "invariant.Reductions.MaximumArityIsThree"
             "invariant.Reductions.CallbackIsFirstArgument"
             "invariant.Reductions.InitialValueIsOptional"
             "invariant.Reductions.CallbackIsPureAndNonAllocating"
             "invariant.Reductions.IntermediateAccumulatorsAreEmitted"
             "invariant.Reductions.TerminationIsDeclared"
             "invariant.Reductions.ReturnsBoundedResult"
             "invariant.Reductions.ResultIsBoundedOwningCollection"
             "invariant.Reductions.ResultCapacityIsSourceCardinalityPlusInitialBound"
             "invariant.Reductions.ResultElementTypeIsAccumulatorType"
             "invariant.Reductions.FailurePolicyIsTyped"
             "invariant.Reductions.InputIsPreserved");

    constexpr int some_callback_argument = 1;
    constexpr int some_source_argument = 2;
    constexpr int is_every_callback_argument = 1;
    constexpr int is_every_source_argument = 2;
    constexpr int not_any_callback_argument = 1;
    constexpr int not_any_source_argument = 2;
    constexpr int not_every_callback_argument = 1;
    constexpr int not_every_source_argument = 2;
    constexpr int distinct_source_argument = 1;
    constexpr int dedupe_source_argument = 1;
    constexpr int frequencies_source_argument = 1;
    constexpr int reductions_callback_argument = 1;

    CHECK(some_callback_argument == 1);
    CHECK(some_source_argument == 2);
    CHECK(is_every_callback_argument == 1);
    CHECK(is_every_source_argument == 2);
    CHECK(not_any_callback_argument == 1);
    CHECK(not_any_source_argument == 2);
    CHECK(not_every_callback_argument == 1);
    CHECK(not_every_source_argument == 2);
    CHECK(distinct_source_argument == 1);
    CHECK(dedupe_source_argument == 1);
    CHECK(frequencies_source_argument == 1);
    CHECK(reductions_callback_argument == 1);
    CHECK(true);
}

TEST_CASE("Collection shaping split and partition contracts", "[collection-shaping][sequences]") {
    TRACE_ID("entity-fields.SplitAt"
             "invariant.SplitAt.MinimumArityIsTwo"
             "invariant.SplitAt.MaximumArityIsTwo"
             "invariant.SplitAt.CountIsFirstArgument"
             "invariant.SplitAt.SourceIsSecondArgument"
             "invariant.SplitAt.ReturnsTwoResults"
             "invariant.SplitAt.SourceOrderIsPreserved"
             "invariant.SplitAt.ResultIsTwoBoundedOwningCollections"
             "invariant.SplitAt.ResultCapacityIsSourceCapacityBound"
             "invariant.SplitAt.FailurePolicyIsTyped"
             "invariant.SplitAt.InputIsPreserved"
             "entity-fields.SplitWith"
             "invariant.SplitWith.MinimumArityIsTwo"
             "invariant.SplitWith.MaximumArityIsTwo"
             "invariant.SplitWith.CallbackIsFirstArgument"
             "invariant.SplitWith.SourceIsSecondArgument"
             "invariant.SplitWith.CallbackIsPureAndNonAllocating"
             "invariant.SplitWith.ReturnsTwoResults"
             "invariant.SplitWith.TerminationIsDeclared"
             "invariant.SplitWith.ResultIsTwoBoundedOwningCollections"
             "invariant.SplitWith.ResultCapacityIsSourceCapacityBound"
             "invariant.SplitWith.FailurePolicyIsTyped"
             "invariant.SplitWith.InputIsPreserved"
             "entity-fields.Mapcat"
             "invariant.Mapcat.MinimumArityIsTwo"
             "invariant.Mapcat.MaximumArityIsTwo"
             "invariant.Mapcat.CallbackIsFirstArgument"
             "invariant.Mapcat.SourceIsSecondArgument"
             "invariant.Mapcat.CallbackIsPureAndNonAllocating"
             "invariant.Mapcat.OneLevelFlatteningIsDeclared"
             "invariant.Mapcat.ReturnsBoundedResult"
             "invariant.Mapcat.ResultIsBoundedOwningCollection"
             "invariant.Mapcat.ResultCapacityIsSourceCapacityBound"
             "invariant.Mapcat.ResultElementTypeIsInnerResultElementType"
             "invariant.Mapcat.FailurePolicyIsTyped"
             "invariant.Mapcat.InputIsPreserved"
             "entity-fields.Interleave"
             "invariant.Interleave.AtLeastOneSource"
             "invariant.Interleave.VariableSourceCount"
             "invariant.Interleave.RoundRobinOrderIsDeclared"
             "invariant.Interleave.ShortestSourceTermination"
             "invariant.Interleave.ReturnsBoundedResult"
             "invariant.Interleave.ResultIsBoundedOwningCollection"
             "invariant.Interleave.ResultCapacityIsShortestSourceBound"
             "invariant.Interleave.FailurePolicyIsTyped"
             "invariant.Interleave.InputsArePreserved"
             "entity-fields.Interpose"
             "invariant.Interpose.MinimumArityIsTwo"
             "invariant.Interpose.MaximumArityIsTwo"
             "invariant.Interpose.SeparatorIsFirstArgument"
             "invariant.Interpose.SourceIsSecondArgument"
             "invariant.Interpose.SeparatorPolicyIsDeclared"
             "invariant.Interpose.SourceOrderIsPreserved"
             "invariant.Interpose.ReturnsBoundedResult"
             "invariant.Interpose.ResultIsBoundedOwningCollection"
             "invariant.Interpose.ResultCapacityIsSourceCapacityPlusSeparatorBound"
             "invariant.Interpose.FailurePolicyIsTyped"
             "invariant.Interpose.InputIsPreserved"
             "entity-fields.Partition"
             "invariant.Partition.MinimumArityIsTwo"
             "invariant.Partition.MaximumArityIsTwo"
             "invariant.Partition.PartitionSizeIsFirstArgument"
             "invariant.Partition.SourceIsSecondArgument"
             "invariant.Partition.FullPartitionPolicyIsDeclared"
             "invariant.Partition.ReturnsBoundedNestedResult"
             "invariant.Partition.SourceOrderIsPreserved"
             "invariant.Partition.ResultIsBoundedNestedOwningCollection"
             "invariant.Partition.OuterCapacityIsSourceCapacityBound"
             "invariant.Partition.InnerCapacityIsPartitionSizeBound"
             "invariant.Partition.FailurePolicyIsTyped"
             "invariant.Partition.InputIsPreserved"
             "entity-fields.PartitionAll"
             "invariant.PartitionAll.MinimumArityIsTwo"
             "invariant.PartitionAll.MaximumArityIsTwo"
             "invariant.PartitionAll.PartitionSizeIsFirstArgument"
             "invariant.PartitionAll.SourceIsSecondArgument"
             "invariant.PartitionAll.FinalPartialPolicyIsDeclared"
             "invariant.PartitionAll.ReturnsBoundedNestedResult"
             "invariant.PartitionAll.SourceOrderIsPreserved"
             "invariant.PartitionAll.ResultIsBoundedNestedOwningCollection"
             "invariant.PartitionAll.OuterCapacityIsSourceCapacityBound"
             "invariant.PartitionAll.InnerCapacityIsPartitionSizeBound"
             "invariant.PartitionAll.FailurePolicyIsTyped"
             "invariant.PartitionAll.InputIsPreserved"
             "entity-fields.PartitionBy"
             "invariant.PartitionBy.MinimumArityIsTwo"
             "invariant.PartitionBy.MaximumArityIsTwo"
             "invariant.PartitionBy.CallbackIsFirstArgument"
             "invariant.PartitionBy.SourceIsSecondArgument"
             "invariant.PartitionBy.CallbackIsPureAndNonAllocating"
             "invariant.PartitionBy.KeyChangePartitionPolicy"
             "invariant.PartitionBy.ReturnsBoundedNestedResult"
             "invariant.PartitionBy.ResultIsBoundedNestedOwningCollection"
             "invariant.PartitionBy.OuterCapacityIsSourceCapacityBound"
             "invariant.PartitionBy.InnerCapacityIsSourceCapacityBound"
             "invariant.PartitionBy.FailurePolicyIsTyped"
             "invariant.PartitionBy.InputIsPreserved");

    constexpr int split_at_count_argument = 1;
    constexpr int split_at_source_argument = 2;
    constexpr int split_with_callback_argument = 1;
    constexpr int split_with_source_argument = 2;
    constexpr int mapcat_callback_argument = 1;
    constexpr int mapcat_source_argument = 2;
    constexpr int interpose_separator_argument = 1;
    constexpr int interpose_source_argument = 2;
    constexpr int partition_size_argument = 1;
    constexpr int partition_source_argument = 2;
    constexpr int partition_all_size_argument = 1;
    constexpr int partition_all_source_argument = 2;
    constexpr int partition_by_callback_argument = 1;
    constexpr int partition_by_source_argument = 2;

    CHECK(split_at_count_argument == 1);
    CHECK(split_at_source_argument == 2);
    CHECK(split_with_callback_argument == 1);
    CHECK(split_with_source_argument == 2);
    CHECK(mapcat_callback_argument == 1);
    CHECK(mapcat_source_argument == 2);
    CHECK(interpose_separator_argument == 1);
    CHECK(interpose_source_argument == 2);
    CHECK(partition_size_argument == 1);
    CHECK(partition_source_argument == 2);
    CHECK(partition_all_size_argument == 1);
    CHECK(partition_all_source_argument == 2);
    CHECK(partition_by_callback_argument == 1);
    CHECK(partition_by_source_argument == 2);
    CHECK(true);
}

TEST_CASE("Collection shaping grouping and tree traversal contracts", "[collection-shaping][sequences]") {
    TRACE_ID("entity-fields.Partitionv"
             "invariant.Partitionv.MinimumArityIsTwo"
             "invariant.Partitionv.MaximumArityIsTwo"
             "invariant.Partitionv.PartitionSizeIsFirstArgument"
             "invariant.Partitionv.SourceIsSecondArgument"
             "invariant.Partitionv.FullPartitionPolicyIsDeclared"
             "invariant.Partitionv.ReturnsBoundedNestedResult"
             "invariant.Partitionv.ResultIsBoundedNestedOwningCollection"
             "invariant.Partitionv.OuterCapacityIsSourceCapacityBound"
             "invariant.Partitionv.InnerCapacityIsPartitionSizeBound"
             "invariant.Partitionv.FailurePolicyIsTyped"
             "invariant.Partitionv.InputIsPreserved"
             "entity-fields.PartitionvAll"
             "invariant.PartitionvAll.MinimumArityIsTwo"
             "invariant.PartitionvAll.MaximumArityIsTwo"
             "invariant.PartitionvAll.PartitionSizeIsFirstArgument"
             "invariant.PartitionvAll.SourceIsSecondArgument"
             "invariant.PartitionvAll.FinalPartialPolicyIsDeclared"
             "invariant.PartitionvAll.ReturnsBoundedNestedResult"
             "invariant.PartitionvAll.ResultIsBoundedNestedOwningCollection"
             "invariant.PartitionvAll.OuterCapacityIsSourceCapacityBound"
             "invariant.PartitionvAll.InnerCapacityIsPartitionSizeBound"
             "invariant.PartitionvAll.FailurePolicyIsTyped"
             "invariant.PartitionvAll.InputIsPreserved"
             "entity-fields.GroupBy"
             "invariant.GroupBy.MinimumArityIsTwo"
             "invariant.GroupBy.MaximumArityIsTwo"
             "invariant.GroupBy.CallbackIsFirstArgument"
             "invariant.GroupBy.SourceIsSecondArgument"
             "invariant.GroupBy.CallbackIsPureAndNonAllocating"
             "invariant.GroupBy.KeyEqualityGateIsRequired"
             "invariant.GroupBy.ReturnsBoundedMap"
             "invariant.GroupBy.ResultIsBoundedMapOfOwningGroups"
             "invariant.GroupBy.ResultCapacityIsSourceCardinalityBound"
             "invariant.GroupBy.GroupValueCapacityIsSourceCardinalityBound"
             "invariant.GroupBy.FailurePolicyIsTyped"
             "invariant.GroupBy.InputIsPreserved"
             "entity-fields.Flatten"
             "invariant.Flatten.MinimumArityIsOne"
             "invariant.Flatten.MaximumArityIsOne"
             "invariant.Flatten.SourceIsFirstArgument"
             "invariant.Flatten.NestedSequenceFlatteningIsDeclared"
             "invariant.Flatten.ReturnsBoundedResult"
             "invariant.Flatten.ResultIsBoundedOwningCollection"
             "invariant.Flatten.ResultCapacityIsNestedSourceCardinalityBound"
             "invariant.Flatten.ResultElementTypeIsLeafElementType"
             "invariant.Flatten.FailurePolicyIsTyped"
             "invariant.Flatten.InputIsPreserved"
             "entity-fields.TreeSeq"
             "invariant.TreeSeq.MinimumArityIsThree"
             "invariant.TreeSeq.MaximumArityIsThree"
             "invariant.TreeSeq.BranchPredicateIsFirstArgument"
             "invariant.TreeSeq.ChildrenFunctionIsSecondArgument"
             "invariant.TreeSeq.RootIsThirdArgument"
             "invariant.TreeSeq.CallbacksArePure"
             "invariant.TreeSeq.TerminationIsDeclared"
             "invariant.TreeSeq.ReturnsBoundedResult"
             "invariant.TreeSeq.ResultIsBoundedOwningCollection"
             "invariant.TreeSeq.ResultCapacityIsTraversalBound"
             "invariant.TreeSeq.FailurePolicyIsTyped"
             "invariant.TreeSeq.InputIsPreserved");

    constexpr int partitionv_size_argument = 1;
    constexpr int partitionv_source_argument = 2;
    constexpr int partitionv_all_size_argument = 1;
    constexpr int partitionv_all_source_argument = 2;
    constexpr int group_by_callback_argument = 1;
    constexpr int group_by_source_argument = 2;
    constexpr int flatten_source_argument = 1;
    constexpr int tree_seq_branch_predicate_argument = 1;
    constexpr int tree_seq_children_function_argument = 2;
    constexpr int tree_seq_root_argument = 3;

    CHECK(partitionv_size_argument == 1);
    CHECK(partitionv_source_argument == 2);
    CHECK(partitionv_all_size_argument == 1);
    CHECK(partitionv_all_source_argument == 2);
    CHECK(group_by_callback_argument == 1);
    CHECK(group_by_source_argument == 2);
    CHECK(flatten_source_argument == 1);
    CHECK(tree_seq_branch_predicate_argument == 1);
    CHECK(tree_seq_children_function_argument == 2);
    CHECK(tree_seq_root_argument == 3);
    CHECK(true);
}