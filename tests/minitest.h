/*
 * Minimal single-header unit test helper for the PC test suite (no dependencies).
 *
 *   TEST_CASE(name) { CHECK(cond); CHECK_EQ(expected, actual); }
 *   int main(void) { RUN(name); return minitest_summary(); }
 *
 * KNOWN_BUG(name, reason) marks a test that documents a known bug: it is
 * expected to fail (XFAIL) and does not break the build. If it starts passing
 * it is reported as XPASS so the marker can be removed.
 */
#ifndef MINITEST_H
#define MINITEST_H

#include <stdio.h>

static int mt_failed_checks, mt_pass, mt_fail, mt_xfail, mt_xpass;
static int mt_quiet;

#define TEST_CASE(name) static void test_##name(void)

#define CHECK(cond) do { if (!(cond)) { \
	mt_failed_checks++; \
	if (!mt_quiet) printf("    %s:%d: CHECK(%s) failed\n", __FILE__, __LINE__, #cond); \
	} } while (0)

#define CHECK_EQ(expected, actual) do { \
	long long mt_e = (long long) (expected), mt_a = (long long) (actual); \
	if (mt_e != mt_a) { \
		mt_failed_checks++; \
		if (!mt_quiet) printf("    %s:%d: %s == %s failed: expected %lld (0x%llx), got %lld (0x%llx)\n", \
			__FILE__, __LINE__, #expected, #actual, mt_e, mt_e, mt_a, mt_a); \
	} } while (0)

#define RUN(name) do { \
	mt_failed_checks = 0; mt_quiet = 0; \
	test_##name(); \
	if (mt_failed_checks) { mt_fail++; printf("FAIL  %s\n", #name); } \
	else { mt_pass++; printf("PASS  %s\n", #name); } \
	} while (0)

#define KNOWN_BUG(name, reason) do { \
	mt_failed_checks = 0; mt_quiet = 1; \
	test_##name(); \
	mt_quiet = 0; \
	if (mt_failed_checks) { mt_xfail++; printf("XFAIL %s  [known bug: %s]\n", #name, reason); } \
	else { mt_xpass++; printf("XPASS %s (known bug seems fixed - remove KNOWN_BUG marker)\n", #name); } \
	} while (0)

static int minitest_summary(void)
{
	printf("\nSummary: %d passed, %d failed, %d expected failures (known bugs), %d unexpectedly passed\n",
		mt_pass, mt_fail, mt_xfail, mt_xpass);
	return mt_fail ? 1 : 0;
}

#endif
