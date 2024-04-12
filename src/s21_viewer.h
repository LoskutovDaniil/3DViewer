#ifndef S21_VIEWER_H
#define S21_VIEWER_H

#include <ctype.h>
#include <math.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================ СТРУКТУРЫ ============================

/**
 * @brief Коды ошибок при работе с файлом
 */
typedef enum {
  NO_ERROR,
  NOT_EXIST,
  NOT_VALID_FILE_TYPE,
  EMPTY_FILE,
  NOT_VALID_V,
  NOT_VALID_F,
  NOT_VALID_F_GREATER_V,
  REGEX_ERROR,
  MEMORY_ALLOCATION_ERROR,
} error_code;

/**
 * @brief Структура данных из файла
 * @param code Код ошибки
 * @param stringNumber Номер строки с ошибкой
 */
typedef struct error {
  error_code code;
  int stringNumber;
  char message[512];
} error;

/**
 * @brief Матрица из вершин, где индекс строки будет соответствовать индексу
 * вершины в .obj файле.
 * @param **matrix сама матрица.
 * @param rows Количество строк в матрице.
 * @param cols Количество столбцов в матрице.
 */
typedef struct matrix_t {
  double **matrix;
  int rows;
  int cols;

  double *max_x;
  double *max_y;
  double *max_z;

  double *min_x;
  double *min_y;
  double *min_z;
} matrix_t;

/**
 * @brief Хранение полигонов
 * @param *vertexes Массив номеров вершин
 * @param numbers_of_vertexes_in_facets Кол-во вершин необходимых для соединения
 */
typedef struct polygon_t {
  unsigned *vertexes;
  int numbers_of_vertexes_in_facets;
} polygon_t;

/**
 * @brief Структура данных из файла
 * @param count_of_vertexes Вершины
 * @param count_of_facets Полигоны
 * @param matrix_t Матрица из вершин
 * @param *polygons Структура полигонов (длина этого массива будет равняться
 * count_of_facets + 1)
 */
typedef struct s21_data {
  unsigned count_of_vertexes;
  unsigned count_of_facets;
  matrix_t *matrix_3d;
  polygon_t **polygons;

  error error;
} s21_data;

// ============================ ФУНКЦИИ ============================

/// \brief Проверка открытого файла: что существует и что не пуст
/// \param file указатель на открытый файл
/// \param data структура, в которой хранится инфо о файле
/// \return код ошибки: 0 - если ошибок нет, 1 - файла не существует, 2 - файл
/// пуст
int s21_check_file_exist(FILE *file, s21_data *data);

/// \brief Проверка валидности файла: построчное считывание, вершины, полигоны
/// \param file указатель на открытый файл
/// \param data структура, в которой хранится инфо о файле
void s21_check_file_validity(FILE *file, s21_data *data);

/// \brief Первичное считывание файла: обработка ошибок, валидность, подсчет
/// вершин и полигонов \param file указатель на открытый файл \param data
/// структура, в которой хранится инфо о файле \return код ошибки: 0 - если
/// ошибок нет
int s21_check_file(FILE *file, s21_data *data);

/**
 * @brief Эта функция создает матрицу заданного размера, выделяет под нее память
 * с использованием calloc.
 *
 * Для рядов выделяем память: count_of_vertexes + 1. Для колонок выделяем
 * память: 3, так как у нас всего три координаты, которые мы хотим хранить.
 *
 * @param rows Количество рядов (строк) в матрице.
 * @param cols Количество столбцов (колонок) в матрице.
 * @param result Указатель на указатель на матрицу (тип matrix_t). Функция
 * выделит память и сохранит указатель на созданную матрицу в этой переменной.
 * @return 0, если создание матрицы прошло успешно, 1 в случае ошибки (например,
 * неверные аргументы).
 */
int s21_create_matrix(int rows, int cols, matrix_t *result);

/**
 * @brief Фришит и зануляет всю матрицу
 * @param *A Указатель на матрицу
 */
void s21_free_matrix(matrix_t *A);

/**
 * @brief Функция выделяет память для массива вершин и массива полигонов.
 * относительно значений count_of_vertexes иcount_of_facets размер выделенной
 * памяти определяется переменными count_of_vertexes иcount_of_facets
 *
 * @param *input указатель на структуру которой требуется выделить память
 *
 * @return 0, если функция отработала без ошибок, любое другое значени, если
 * память выделить не удалось или не заданны значения в структуре
 */
int s21_memory_allocation(s21_data *input);

/**
 * @brief Функция сбора координат точек, забирает из файла координаты точек x y
 * z записывает в массив структуры data->matrix_3d
 *
 * @param *input указатель на структуру для записи координат точек из файла
 * @param *p_to_file указатель на открытый поток к файлу
 */
void s21_coordinates_from_file_to_data(s21_data *input, FILE *p_to_file);

/**
 * @brief Функция определения и перезписи max и min значений по x y z внутри
 * структуры типа data данная функция вызывается в цикле и проверяет каждое
 * входящее значение по x y z
 * @param *input структура в которой будут изменены мак и мин значения x y z
 * @param row строка значения которой необходимо проверить
 */
void s21_checking_more_and_less(s21_data *input, int row);

/**
 * @brief Фунция сбора полигонов из файла в массив в структуру
 *
 * @param *input структура данных в которую будут записанны полигоны в массив
 * unsigned *vertexes;
 * @param *p_to_file указатель на открытый поток к файлу
 *
 * @return 0 в случае отсутствия ошибок
 * @return 1 вернет ошибку и прекратит работу в случае если номер точки в
 * полигонах превышает количество точек
 *
 */
int s21_polygons_from_file_to_data(s21_data *input, FILE *p_to_file);

/**
 * @brief Функция преобразовывает все координаты точек по x y z в предел от 1 до
 * -1
 *
 * @param *input структура в которой необходимо произвести преобразование
 * координат
 *
 */
void s21_normalization(s21_data *input);

/***
 * @brief Функция изменяет значения координат матрицы на указанную точность
 * необходима для изменения масштаба матрицы, то есть изменения значения каждой
 * координаты матрицы на заданную величину
 *
 * @param *input структура в которой необходимо провести преобразование значений
 * координат
 * @param scale значение на которое нужно изменить масштаб матрицы
 */
void s21_scaling_matrix(s21_data *input, double scale);

/**
 * @brief Функция центровки матрицы относительно ноля
 * определяет центр для каждой из осей координат и изменяет все координаты
 * относительно значений центра
 *
 * @param *input структура в которой необходимо произвести центроку матрицы
 *
 */
void s21_centering_matrix(s21_data *input);

/**
 * @brief Функция сдвига матрицы по оси координат
 *
 * @param *input структура матрицу которой необходимо сдвинуть
 * @param move_x параметр отвечает за смещение координаты, приходит от
 * пользователя с фронтенда
 * @param move_y параметр отвечает за смещение координаты, приходит от
 * пользователя с фронтенда
 * @param move_z параметр отвечает за смещение координаты, приходит от
 * пользователя с фронтенда
 */
void s21_move_matrix(s21_data *input, double move_x, double move_y,
                     double move_z);

/**
 * @brief Функция поворота матрицы по одной из осей координат
 *
 * @param *input структура матрицы
 * @param angle угол поворота
 *
 *
 */
void s21_rotate_matrix(s21_data *input, double angle, int flag_move_x,
                       int flag_move_y, int flag_move_z);

/**
 * @brief Функция обработки ошибок
 *
 * @param *input структура с данными
 * @param file_path название файла
 *
 */
void s21_error_handling(s21_data *input, const char *file_path);

/**
 * @brief Функция проверки типа расширения файла
 *
 * @param *input структура для фиксации кода ошибки в случае если расширенее не
 * подходящие
 * @param  file_name путь к файлу
 *
 */
void s21_check_file_type(s21_data *input, const char *p_to_file);

/**
 * @brief основная функция парсера вершин из файла
 *
 * @param path строка, содержащая путь к .obj файлу
 * @param  data структура, куда будет записана вся информация
 *
 */
int parser(const char *path, s21_data *modelInfo);

/**
 * @brief Простая очистка памяти после отработки программы
 *
 * @param *data указатель на структуру, в которой лежат данные о файле
 *
 */
void s21_simple_clear_memory(s21_data *data);

/**
 * @brief Поиск максимального значения из всех координат (по модулю)
 *
 * @param *input указатель на структуру, в которой лежат данные о файле
 * @return максимальное значение
 */
double s21_find_max_coord(s21_data *input);

/**
 * @brief Поиск максимального значения
 *
 * @param v1 - первое число
 * @param v2 - второе число
 * @return максимальное значение
 */
double s21_find_max_value(double v1, double v2);
#endif
