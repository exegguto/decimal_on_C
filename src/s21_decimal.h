#ifndef SRC_S21_DECIMAL_H_
#define SRC_S21_DECIMAL_H_

#define FLOAT_MIN 1e-28
#define FLOAT_ROUND 7
#define DEC_MAX "79228162514264337593543950335"
#define MAXEXP 28
#define MAXDEC 29
#define MAXSTR 100
#define MAXBITS 4294967295U
#define EXP1 0b00000000000000010000000000000000
#define MINUS 0b10000000000000000000000000000000
#define PLUS 0b00000000000000000000000000000000
#define MAXINT 2147483647
#define STR_0                                                                  \
  "00000000000000000000000000000000000000000000000000000000000000000000000000" \
  "0000000000000000000000000"

typedef struct {
  int bits[4];
} s21_decimal;

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
// 0 - OK
#define OK 0
// 1 - число слишком велико или равно бесконечности
#define INF 1
// 2 - число слишком мало или равно отрицательной бесконечности
#define NEGATIVE_INF 2
// 3 - деление на 0
#define s21_NAN 3

int s21_is_less(s21_decimal, s21_decimal);
int s21_is_less_or_equal(s21_decimal, s21_decimal);
int s21_is_greater(s21_decimal, s21_decimal);
int s21_is_greater_or_equal(s21_decimal, s21_decimal);
int s21_is_equal(s21_decimal, s21_decimal);
int s21_is_not_equal(s21_decimal, s21_decimal);
// 1 - TRUE
#define TRUE 1
// 0 - FALSE
#define FALSE 0

int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);
// 0 - OK

// 1 - ошибка конвертации
#define ERROR 1

int s21_floor(s21_decimal value, s21_decimal *result);
int s21_round(s21_decimal value, s21_decimal *result);
int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_negate(s21_decimal value, s21_decimal *result);
//  0 - OK
//  1 - ошибка вычисления

// Доп функции

int s21_shift_bits_left(s21_decimal *value, const int num);
void s21_shift_bits_right(s21_decimal *value, const int num);
void s21_add_str(const char *val_1, const char *val_2, char *rez);
void s21_sub_str(const char *val_1, const char *val_2, char *rez);
void s21_toggle_bit(s21_decimal *value, const int num);
int s21_bit(const s21_decimal value, const int num);
int s21_scale(const s21_decimal value);
void s21_mul_two_to_decimal(const s21_decimal src, char *buffer);
void s21_div_decimal_to_two(s21_decimal *src, char *buffer);
void s21_buh_round_zero(char *buffer, int *buflen);
int s21_buh_round_nine(char *buffer, int *buflen, int val);
int s21_buh_round(char *buffer, int *buflen, int flag);
void s21_str_scale(s21_decimal *value_1, s21_decimal *value_2, char *val_1,
                   char *val_2);
int s21_shift_scale_str(char *buffer, const int num);
void s21_init(char *val);
int s21_test_zero(s21_decimal *val);
int s21_sum_bits(s21_decimal *value);
int s21_set_exp(s21_decimal *value, int num);
void s21_shift_str(char *str);

#endif  // SRC_S21_DECIMAL_H_
