#include "s21_viewer.h"

int s21_create_matrix(int rows, int columns, matrix_t *result) {
  if (result == NULL || rows <= 0 || columns <= 0) {
    return 1;
  }
  int res = 0;
  result->rows = rows;
  result->cols = columns;
  result->matrix = calloc(rows, sizeof(double *));
  if (result->matrix == NULL) {
    res = 1;
  } else {
    for (int i = 0; i < rows; i++) {
      result->matrix[i] = calloc(columns, sizeof(double));
    }
  }

  return res;
}

void s21_free_matrix(matrix_t *A) {
  if (A != NULL) {
    for (int row = 0; row < A->rows; row++) {
      if (A->matrix[row]) free(A->matrix[row]);
    }
    if (A->matrix != NULL) free(A->matrix);
    A->matrix = NULL;
    if (A->cols) A->cols = 0;
    if (A->rows) A->rows = 0;
  }
}

int s21_memory_allocation(s21_data *input) {
  input->polygons = calloc(input->count_of_facets, sizeof(polygon_t *));

  if (input->polygons == NULL) input->error.code = MEMORY_ALLOCATION_ERROR;

  if (!input->error.code) {
    input->matrix_3d = calloc(1, sizeof(matrix_t));
    input->matrix_3d->cols = 3;
    input->matrix_3d->rows = input->count_of_vertexes + 1;

    input->matrix_3d->max_x = calloc(1, sizeof(double));
    input->matrix_3d->max_y = calloc(1, sizeof(double));
    input->matrix_3d->max_z = calloc(1, sizeof(double));
    input->matrix_3d->min_x = calloc(1, sizeof(double));
    input->matrix_3d->min_y = calloc(1, sizeof(double));
    input->matrix_3d->min_z = calloc(1, sizeof(double));

    if (s21_create_matrix(input->count_of_vertexes, 3, input->matrix_3d)) {
      input->error.code = MEMORY_ALLOCATION_ERROR;
    }
  }

  return input->error.code;
}

void s21_coordinates_from_file_to_data(s21_data *input, FILE *p_to_file) {
  rewind(p_to_file);

  char *tmp_str = calloc(256, sizeof(char));

  int row_counter = 0;

  int col_counter = 0;

  while (fgets(tmp_str, 1990, p_to_file)) {
    if (tmp_str[0] == 'v' && tmp_str[1] == ' ') {
      for (int i = 0; tmp_str[i] != '\0' && col_counter < 3; ++i) {
        while (tmp_str[i++] != ' ') continue;

        double value = 0;

        if (sscanf(&tmp_str[i], "%lf", &value)) {
          input->matrix_3d->matrix[row_counter][col_counter++] = value;
        }
      }
      s21_checking_more_and_less(input, row_counter);

      row_counter++;
    }

    memset(tmp_str, 0, strlen(tmp_str));
    col_counter = 0;
  }

  free(tmp_str);
  tmp_str = NULL;
}

void s21_checking_more_and_less(s21_data *input, int row) {
  matrix_t *m = input->matrix_3d;

  if (row == 1) {
    m->min_x = &m->matrix[row][0];
    m->max_x = &m->matrix[row][0];
    m->min_y = &m->matrix[row][1];
    m->max_y = &m->matrix[row][1];
    m->min_z = &m->matrix[row][2];
    m->max_z = &m->matrix[row][2];
  } else {
    if (*(m->min_x) > m->matrix[row][0]) m->min_x = &m->matrix[row][0];
    if (*(m->max_x) < m->matrix[row][0]) m->max_x = &m->matrix[row][0];
    if (*(m->min_y) > m->matrix[row][1]) m->min_y = &m->matrix[row][1];
    if (*(m->max_y) < m->matrix[row][1]) m->max_y = &m->matrix[row][1];
    if (*(m->min_z) > m->matrix[row][2]) m->min_z = &m->matrix[row][2];
    if (*(m->max_z) < m->matrix[row][2]) m->max_z = &m->matrix[row][2];
  }
}

int s21_polygons_from_file_to_data(s21_data *input, FILE *p_to_file) {
  int line = 1;

  rewind(p_to_file);

  int st_poligons = 0;

  char *tmp_str = calloc(256, sizeof(char));
  if (tmp_str == NULL) input->error.code = MEMORY_ALLOCATION_ERROR;

  while (fgets(tmp_str, 1990, p_to_file) && !input->error.code) {
    if (tmp_str[0] == 'f') {
      input->polygons[st_poligons] = calloc(1, sizeof(polygon_t));

      input->polygons[st_poligons]->vertexes =
          calloc(1000, sizeof(unsigned int));
      short record = 1;

      int st_to_tmp_str = 2;

      int st_vertex = 0;

      if (input->polygons[st_poligons] == NULL ||
          input->polygons[st_poligons]->vertexes == NULL)
        input->error.code = MEMORY_ALLOCATION_ERROR;

      while (record && !input->error.code) {
        int value = 0;

        if (sscanf(&tmp_str[st_to_tmp_str], "%d", &value)) {
          if (input->count_of_vertexes < (unsigned)value) {
            input->error.code = NOT_VALID_F_GREATER_V;
            input->error.stringNumber = line;
          }

          input->polygons[st_poligons]->vertexes[st_vertex] = value - 1;
        }

        st_vertex++;

        while (tmp_str[st_to_tmp_str] != ' ' &&
               tmp_str[st_to_tmp_str] != '\0') {
          st_to_tmp_str++;
        }
        while (tmp_str[st_to_tmp_str] == ' ' ||
               tmp_str[st_to_tmp_str] == '\n') {
          st_to_tmp_str++;
        }

        if (tmp_str[st_to_tmp_str] == '\0' || tmp_str[st_to_tmp_str] == '\n') {
          record = 0;
        }
        input->polygons[st_poligons]->numbers_of_vertexes_in_facets = st_vertex;
      }
      st_poligons++;
    }
    line++;

    memset(tmp_str, 0, strlen(tmp_str));
  }

  free(tmp_str);
  tmp_str = NULL;

  return input->error.code;
}

void s21_simple_clear_memory(s21_data *data) {
  s21_free_matrix(data->matrix_3d);
  free(data->matrix_3d);
  data->matrix_3d = NULL;

  for (unsigned int i = 0; i < data->count_of_facets; i++) {
    if (data->polygons != NULL) {
      if (data->polygons[i] != NULL) {
        if (data->polygons[i]->vertexes != NULL) {
          free(data->polygons[i]->vertexes);
          data->polygons[i]->vertexes = NULL;
        }

        free(data->polygons[i]);
        data->polygons[i] = NULL;
      }
    }
  }

  free(data->polygons);
  data->polygons = NULL;

  free(data);
  data = NULL;
}

void s21_normalization(s21_data *input) {
  double max_value = s21_find_max_coord(input);
  double scale = 1 / max_value;

  s21_scaling_matrix(input, scale);
}

double s21_find_max_coord(s21_data *input) {
  double x_1 = fabs(*(input->matrix_3d->min_x));
  double x_2 = fabs(*(input->matrix_3d->max_x));
  double y_1 = fabs(*(input->matrix_3d->min_y));
  double y_2 = fabs(*(input->matrix_3d->max_y));
  double z_1 = fabs(*(input->matrix_3d->min_z));
  double z_2 = fabs(*(input->matrix_3d->max_z));

  double max =
      s21_find_max_value(s21_find_max_value(s21_find_max_value(z_1, z_2),
                                            s21_find_max_value(y_1, y_2)),
                         s21_find_max_value(x_1, x_2));

  return max;
}

double s21_find_max_value(double v1, double v2) {
  if (v1 > v2) {
    return v1;
  } else {
    return v2;
  }
}

void s21_scaling_matrix(s21_data *input, double scale) {
  for (unsigned int i = 0; i < input->count_of_vertexes; ++i) {
    input->matrix_3d->matrix[i][0] *= scale;
    input->matrix_3d->matrix[i][1] *= scale;
    input->matrix_3d->matrix[i][2] *= scale;
  }
}

void s21_centering_matrix(s21_data *input) {
  double center_x =
      *(input->matrix_3d->min_x) +
      (*(input->matrix_3d->max_x) - *(input->matrix_3d->min_x)) / 2;
  double center_y =
      *(input->matrix_3d->min_y) +
      (*(input->matrix_3d->max_y) - *(input->matrix_3d->min_y)) / 2;
  double center_z =
      *(input->matrix_3d->min_z) +
      (*(input->matrix_3d->max_z) - *(input->matrix_3d->min_z)) / 2;

  for (unsigned int i = 0; i < input->count_of_vertexes; ++i) {
    input->matrix_3d->matrix[i][0] -= center_x;
    input->matrix_3d->matrix[i][1] -= center_y;
    input->matrix_3d->matrix[i][2] -= center_z;
  }
}

void s21_move_matrix(s21_data *input, double move_x, double move_y,
                     double move_z) {
  for (unsigned int i = 0; i < input->count_of_vertexes; ++i) {
    input->matrix_3d->matrix[i][0] += move_x;
    input->matrix_3d->matrix[i][1] += move_y;
    input->matrix_3d->matrix[i][2] += move_z;
  }
}

void s21_rotate_matrix(s21_data *input, double angle, int flag_move_x,
                       int flag_move_y, int flag_move_z) {
  double sin_a = sin(angle);
  double cos_a = cos(angle);
  matrix_t *m = input->matrix_3d;

  for (unsigned int i = 0; i < input->count_of_vertexes; ++i) {
    double x = m->matrix[i][0];
    double y = m->matrix[i][1];
    double z = m->matrix[i][2];

    if (flag_move_x) {
      m->matrix[i][1] = y * cos_a - z * sin_a;

      m->matrix[i][2] = y * sin_a + z * cos_a;
    } else if (flag_move_y) {
      m->matrix[i][0] = x * cos_a + z * sin_a;

      m->matrix[i][2] = x * (-sin_a) + z * cos_a;
    } else if (flag_move_z) {
      m->matrix[i][0] = x * cos_a - y * sin_a;

      m->matrix[i][1] = x * sin_a + y * cos_a;
    }
  }
}

int parser(const char *path, s21_data *modelInfo) {
  int isError = 0;

  s21_check_file_type(modelInfo, path);
  isError = modelInfo->error.code;
  FILE *file = NULL;
  if (!isError) file = fopen(path, "r");

  if (!isError) isError = s21_check_file(file, modelInfo);

  if (!isError) isError = s21_memory_allocation(modelInfo);

  if (!isError) {
    s21_coordinates_from_file_to_data(modelInfo, file);

    isError = s21_polygons_from_file_to_data(modelInfo, file);

    fclose(file);
    file = NULL;
  }
  if (!isError) {
    s21_normalization(modelInfo);
    s21_centering_matrix(modelInfo);
    if (*modelInfo->matrix_3d->min_x < 1.3) {
      s21_scaling_matrix(modelInfo, 2);
    }
  }

  s21_error_handling(modelInfo, path);

  if (file != NULL) {
    fclose(file);
    file = NULL;
  }

  return 0;
}
