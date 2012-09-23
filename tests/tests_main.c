#include <check.h>

Suite *ff_suite();

int main(int argc, char **argv) {
	int number_failed;
	Suite *s = ff_suite();
	SRunner *sr = srunner_create(s);
	/*add more suites here*/
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return number_failed;
}
