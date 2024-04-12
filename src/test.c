#include <check.h>

#include "s21_viewer.h"

#define CK_FORK no

START_TEST(s21_parser_1) {
  char path[] = "./skull.obj";
  s21_data* data = calloc(1, sizeof(struct s21_data));
  parser(path, data);

  ck_assert_int_eq(6669, data->count_of_vertexes);
  ck_assert_int_eq(13334, data->count_of_facets);
  s21_simple_clear_memory(data);
}
END_TEST

START_TEST(s21_parser_2) {
  char path[] = "./skuul.obj";
  s21_data* data = calloc(1, sizeof(struct s21_data));
  parser(path, data);

  ck_assert_int_eq(NOT_EXIST, data->error.code);
  s21_simple_clear_memory(data);
}
END_TEST

START_TEST(s21_parser_3) {
  char path[] = "./skuf.ob";
  s21_data* data = calloc(1, sizeof(struct s21_data));
  parser(path, data);

  ck_assert_int_eq(NOT_VALID_FILE_TYPE, data->error.code);
  s21_simple_clear_memory(data);
}
END_TEST

START_TEST(s21_parser_4) {
  char path[] = "./tests/empty.obj";
  s21_data* data = calloc(1, sizeof(struct s21_data));
  parser(path, data);

  ck_assert_int_eq(EMPTY_FILE, data->error.code);
  s21_simple_clear_memory(data);
}
END_TEST

START_TEST(s21_parser_5) {
  char path[] = "./tests/not-valid-v.obj";
  s21_data* data = calloc(1, sizeof(struct s21_data));
  parser(path, data);

  ck_assert_int_eq(NOT_VALID_V, data->error.code);
  s21_simple_clear_memory(data);
}
END_TEST

START_TEST(s21_parser_6) {
  char path[] = "./tests/not-valid-f.obj";
  s21_data* data = calloc(1, sizeof(struct s21_data));
  parser(path, data);

  ck_assert_int_eq(NOT_VALID_F, data->error.code);
  s21_simple_clear_memory(data);
}
END_TEST

Suite* parser_s(void) {
  Suite* s;
  TCase* tc;
  s = suite_create("\033[42ms21_parser\033[0m");
  tc = tcase_create("case_all_test");

  tcase_add_test(tc, s21_parser_1);
  tcase_add_test(tc, s21_parser_2);
  tcase_add_test(tc, s21_parser_3);
  tcase_add_test(tc, s21_parser_4);
  tcase_add_test(tc, s21_parser_5);
  tcase_add_test(tc, s21_parser_6);

  suite_add_tcase(s, tc);
  return s;
}

void case_test(Suite* s, int* fail) {
  SRunner* runner = srunner_create(s);
  srunner_set_fork_status(runner, CK_NOFORK);
  srunner_run_all(runner, CK_NORMAL);
  *fail = srunner_ntests_failed(runner);
  srunner_free(runner);
}

int main() {
  int fail = 0;
  case_test(parser_s(), &fail);

  return 0;
}
