#include <concepts>

#include <catch2/catch_test_macros.hpp>

#include "cljonic-test-api.hpp"

#define TRACE_ID(id_literal) INFO("trace-id: " id_literal)

TEST_CASE("Peek free function operations", "[peek]") {
    using cljonic::conj;
    using cljonic::count;
    using cljonic::empty;
    using cljonic::peek;
    using cljonic::Queue;
    using cljonic::Vector;

    TRACE_ID("entity-fields.Peek");
    TRACE_ID("invariant.Peek.PeekFreeFunctionSupported");
    TRACE_ID("invariant.Peek.AdmitsVectorAndQueueOnly");
    TRACE_ID("invariant.Peek.ReturnsLastVectorElement");
    TRACE_ID("invariant.Peek.ReturnsQueueFrontElement");
    TRACE_ID("invariant.Peek.ReturnsDefaultValueWhenEmpty");
    TRACE_ID("invariant.Peek.ReturnsValueByValue");
    TRACE_ID("invariant.Peek.LeavesInputUnchanged");
    TRACE_ID("invariant.Peek.NoHeapAllocation");
    TRACE_ID("invariant.Peek.NoRtti");
    TRACE_ID("invariant.Peek.NoExceptions");
    TRACE_ID("invariant.Peek.SingleThreadedExecutionModel");
    TRACE_ID("invariant.Peek.ReferentialTransparency");
    TRACE_ID("invariant.Peek.RejectsUnsupportedDomainWithTargetedDiagnostic");
    TRACE_ID("invariant.Peek.DiagnosticNamesSupportedDomains");

    constexpr auto vector = Vector<int, 4>{10, 20, 99};
    constexpr auto queue = conj(conj(Queue<int, 4>{}, 99), 100);
    constexpr auto empty_vector = Vector<int, 4>{};
    constexpr auto empty_queue = Queue<int, 4>{};
    constexpr auto zero_capacity_vector = Vector<int, 0>{};
    constexpr auto zero_capacity_queue = Queue<int, 0>{};
    STATIC_REQUIRE(peek(vector) == 99);
    STATIC_REQUIRE(peek(queue) == 99);
    STATIC_REQUIRE(peek(empty_vector) == 0);
    STATIC_REQUIRE(peek(empty_queue) == 0);
    STATIC_REQUIRE(peek(zero_capacity_vector) == 0);
    STATIC_REQUIRE(peek(zero_capacity_queue) == 0);
    STATIC_REQUIRE(std::same_as<decltype(peek(vector)), int>);
    STATIC_REQUIRE(std::same_as<decltype(peek(queue)), int>);
    STATIC_REQUIRE(noexcept(peek(vector)));
    STATIC_REQUIRE(noexcept(peek(queue)));

    volatile int vector_first_raw = 123;
    volatile int vector_last_raw = 456;
    volatile int queue_front_raw = 789;
    volatile int queue_back_raw = 987;
    int vector_first = vector_first_raw;
    int vector_last = vector_last_raw;
    int queue_front = queue_front_raw;
    int queue_back = queue_back_raw;
    const auto runtime_vector = Vector<int, 4>{vector_first, vector_last};
    const auto runtime_queue = conj(conj(Queue<int, 4>{}, queue_front), queue_back);

    REQUIRE(peek(runtime_vector) == 456);
    REQUIRE(peek(runtime_queue) == 789);
    REQUIRE(peek(empty(runtime_vector)) == 0);
    REQUIRE(peek(empty(runtime_queue)) == 0);
    REQUIRE(count(runtime_vector) == 2);
    REQUIRE(peek(runtime_vector) == 456);
    REQUIRE(count(runtime_queue) == 2);
    REQUIRE(peek(runtime_queue) == 789);
}
