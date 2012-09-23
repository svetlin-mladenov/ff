#include <check.h>

START_TEST(test_ff_read) {
}
END_TEST

Suite * ff_suite() {
	Suite *s = suite_create("ff");

	TCase *tc = tcase_create("ff");
	tcase_add_test(tc, test_ff_read);
	/*add additional tests here*/
	suite_add_tcase(s, tc);

	return s;
}
