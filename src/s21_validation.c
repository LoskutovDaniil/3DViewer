#include "s21_viewer.h"

int s21_check_file_exist(FILE* file, s21_data* data) {
  if (file == NULL) {
    data->error.code = NOT_EXIST;
  } else if (fgetc(file) == EOF) {
    data->error.code = EMPTY_FILE;
  } else {
    fseek(file, 0, 0);
    data->error.code = NO_ERROR;
  }

  return data->error.code;
}

void s21_check_file_validity(FILE* file, s21_data* data) {
  int line = 1;

  char buff[1024] = {0};

  regex_t pattern_v, pattern_f;
  if (regcomp(&pattern_v, "^v([ ]+[+-]?[0-9]+(.[0-9]+)?){3}", REG_EXTENDED) ||
      regcomp(&pattern_f,
              "^f([ ]+[1-9]([0-9]+)?((/([1-9]([0-9]+)?)?){1,2})?){3,}",
              REG_EXTENDED)) {
    data->error.code = REGEX_ERROR;
  }

  while ((fgets(buff, sizeof(buff), file) != NULL) && !data->error.code) {
    if (strncmp(buff, "v ", 2) == 0) {
      if (!(regexec(&pattern_v, buff, 0, NULL, 0))) {
        data->count_of_vertexes++;
      } else {
        data->error.code = NOT_VALID_V;
        data->error.stringNumber = line;
      }
    } else if (strncmp(buff, "f ", 2) == 0) {
      if (!(regexec(&pattern_f, buff, 0, NULL, 0))) {
        data->count_of_facets++;
      } else {
        data->error.code = NOT_VALID_F;
        data->error.stringNumber = line;
      }
    }

    line++;

    memset(buff, 0, sizeof(buff));
  }

  regfree(&pattern_v);
  regfree(&pattern_f);
}

int s21_check_file(FILE* file, s21_data* data) {
  if (!s21_check_file_exist(file, data)) {
    s21_check_file_validity(file, data);
  }

  return data->error.code;
}

void s21_check_file_type(s21_data* input, const char* p_to_file) {
  int len = strlen(p_to_file);
  if (p_to_file[len - 1] != 'j') input->error.code = NOT_VALID_FILE_TYPE;
  if (p_to_file[len - 2] != 'b') input->error.code = NOT_VALID_FILE_TYPE;
  if (p_to_file[len - 3] != 'o') input->error.code = NOT_VALID_FILE_TYPE;
  if (p_to_file[len - 4] != '.') input->error.code = NOT_VALID_FILE_TYPE;
}

void s21_error_handling(s21_data* input, const char* file_path) {
  switch (input->error.code) {
    case NOT_EXIST:
      sprintf(input->error.message, "ERROR: The file '%s' does not exist",
              file_path);
      break;
    case NOT_VALID_FILE_TYPE:
      sprintf(input->error.message,
              "ERROR: Not valid type of file '%s', '.obj' type required",
              file_path);
      break;
    case EMPTY_FILE:
      sprintf(input->error.message, "ERROR: The file '%s' is empty", file_path);
      break;
    case NOT_VALID_V:
      sprintf(input->error.message,
              "ERROR: Incorrect value of (v) -- in '%s', line %d", file_path,
              input->error.stringNumber);
      break;
    case NOT_VALID_F:
      sprintf(input->error.message,
              "ERROR: Incorrect value of (f) -- in '%s', line %d", file_path,
              input->error.stringNumber);
      break;
    case NOT_VALID_F_GREATER_V:
      sprintf(input->error.message,
              "ERROR: Incorrect value of (f), the vertex value is greater than "
              "the total number of vertexes -- in '%s', "
              "line %d",
              file_path, input->error.stringNumber);
      break;
    case REGEX_ERROR:
      sprintf(input->error.message,
              "SERVER ERROR: Something went wrong with regex");
      break;
    case MEMORY_ALLOCATION_ERROR:
      sprintf(input->error.message,
              "SERVER ERROR: Something went wrong with memory allocation");
      break;
    default:
      break;
  }
}
