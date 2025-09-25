#include <zephyr/ztest.h>

ZTEST_SUITE(minimal_suite, NULL, NULL, NULL, NULL, NULL);

ZTEST(minimal_suite, test_true_assertion)
{
    zassert_true(true, "This should always be true");
}