/*
 * Ring Buffer Module - Homework Test Skeleton
 *
 * test_fresh_state is provided as a worked example. Fill in the remaining
 * 7 ZTEST bodies according to TEST_SPEC.md. Stubs call ztest_test_skip()
 * so the binary builds and runs cleanly before each test is implemented.
 *
 * Run:
 *   west twister -T tests/ring_buf -p native_sim
 */

#include <zephyr/ztest.h>
#include <errno.h>

#include "ring_buf.h"

/*
 * Shared before hook: every suite reinitialises the ring buffer with a
 * capacity of 4 so tests start from a clean, known state. Capacity 4 is
 * enough to exercise FIFO order (push 1, 2, 3) and overflow (full at 4).
 */
static void before(void *f)
{
	ARG_UNUSED(f);
	rb_init(4);
}

/*
 * ============================================================================
 * Test Suite: ring_buf_init
 *
 * Initial state and re-initialization behaviour.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_init, NULL, NULL, before, NULL, NULL);

/* PROVIDED — study this test before writing the rest. */
ZTEST(ring_buf_init, test_fresh_state)
{
	zassert_true(rb_is_empty(), "Fresh buffer must be empty");
	zassert_equal(rb_count(), 0, "Fresh buffer count must be 0");
}

ZTEST(ring_buf_init, test_reinit_clears_state)
{
	/* TEST_SPEC.md "Suite ring_buf_init" #2: push(99), rb_init(4)
	 * again, then verify the previous push is fully discarded.
	 */
	zassert_equal(rb_push(99), 0, "push(99) must succeed on fresh buffer");

	zassert_equal(rb_init(4), 0, "re-init with capacity 4 must succeed");

	zassert_true(rb_is_empty(), "Buffer must be empty after re-init");
	zassert_equal(rb_count(), 0, "Buffer count must be 0 after re-init");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_push_pop
 *
 * Single push/pop round-trip, FIFO order, full error path.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_push_pop, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_push_pop, test_single_push_pop)
{
	/* TEST_SPEC.md "Suite ring_buf_push_pop" #1:
	 * push(42) == 0; pop(&v) == 0; v == 42; empty after the pop.
	 */
	int v = -1;

	zassert_equal(rb_push(42), 0, "push(42) must succeed");
	zassert_equal(rb_pop(&v), 0, "pop must succeed");
	zassert_equal(v, 42, "popped value must be 42");
	zassert_true(rb_is_empty(), "Buffer must be empty after popping the only element");
}

ZTEST(ring_buf_push_pop, test_fifo_order)
{
	/* TEST_SPEC.md "Suite ring_buf_push_pop" #2:
	 * three pushes return 0; the pops yield 1, then 2, then 3.
	 */
	int v = -1;

	zassert_equal(rb_push(1), 0, "push(1) must succeed");
	zassert_equal(rb_push(2), 0, "push(2) must succeed");
	zassert_equal(rb_push(3), 0, "push(3) must succeed");

	zassert_equal(rb_pop(&v), 0, "1st pop must succeed");
	zassert_equal(v, 1, "1st popped value must be 1");

	zassert_equal(rb_pop(&v), 0, "2nd pop must succeed");
	zassert_equal(v, 2, "2nd popped value must be 2");

	zassert_equal(rb_pop(&v), 0, "3rd pop must succeed");
	zassert_equal(v, 3, "3rd popped value must be 3");

	zassert_true(rb_is_empty(), "Buffer must be empty after popping all elements");
}

ZTEST(ring_buf_push_pop, test_push_full_returns_enospc)
{
	/* TEST_SPEC.md "Suite ring_buf_push_pop" #3:
	 * four pushes return 0; is_full; the 5th push returns exactly
	 * -ENOSPC; count stays 4 (rejected push consumed no slot).
	 */
	zassert_equal(rb_push(1), 0, "push(1) must succeed");
	zassert_equal(rb_push(2), 0, "push(2) must succeed");
	zassert_equal(rb_push(3), 0, "push(3) must succeed");
	zassert_equal(rb_push(4), 0, "push(4) must succeed");

	zassert_true(rb_is_full(), "Buffer must be full after 4 pushes");

	zassert_equal(rb_push(99), -ENOSPC, "Push past capacity must return -ENOSPC");
	zassert_equal(rb_count(), 4, "Rejected push must not consume a slot");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_boundaries
 *
 * Peek semantics and NULL-pointer boundary conditions.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_boundaries, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_boundaries, test_peek_does_not_consume)
{
	/* TEST_SPEC.md "Suite ring_buf_boundaries" #1:
	 * push(7); two peeks both return 0 with v == 7; count still 1.
	 */
	int v = -1;

	zassert_equal(rb_push(7), 0, "push(7) must succeed");

	zassert_equal(rb_peek(&v), 0, "1st peek must succeed");
	zassert_equal(v, 7, "1st peek must yield 7");

	zassert_equal(rb_peek(&v), 0, "2nd peek must succeed");
	zassert_equal(v, 7, "2nd peek must yield 7");

	zassert_equal(rb_count(), 1, "Peek must not consume the element");
}

ZTEST(ring_buf_boundaries, test_pop_null_returns_einval)
{
	/* TEST_SPEC.md "Suite ring_buf_boundaries" #2:
	 * rb_pop(NULL) returns exactly -EINVAL.
	 */
	zassert_equal(rb_pop(NULL), -EINVAL, "rb_pop(NULL) must return -EINVAL");
}

ZTEST(ring_buf_boundaries, test_is_full_after_fill)
{
	/* TEST_SPEC.md "Suite ring_buf_boundaries" #3:
	 * four pushes return 0; is_full; count == 4.
	 */
	zassert_equal(rb_push(1), 0, "push(1) must succeed");
	zassert_equal(rb_push(2), 0, "push(2) must succeed");
	zassert_equal(rb_push(3), 0, "push(3) must succeed");
	zassert_equal(rb_push(4), 0, "push(4) must succeed");

	zassert_true(rb_is_full(), "Buffer must be full after 4 pushes");
	zassert_equal(rb_count(), 4, "Buffer count must be 4");
}
