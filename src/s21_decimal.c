#include "s21_decimal.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const s21_decimal dec_null = {{0, 0, 0, 0}};

/**
 * @brief Получение значения бита
 *
 * @param value структура
 * @param num порядковый номер нужного бита
 * @param Владимир
 * @return int 0 или 1 бит
 */
int s21_bit(const s21_decimal value, const int num) {
  return (value.bits[num / 32] >> (num % 32)) & 1u;
}

/**
 * @brief Переключение бита
 *
 * @param value указатель на число децимал
 * @param Владимир
 * @param num порядковый номер бита
 */
void s21_toggle_bit(s21_decimal *value, const int num) {
  value->bits[num / 32] ^= 1u << num % 32;
}

/**
 * @brief Получение показателя степени от 0 до 28, который указывает степень 10
 * для разделения целого числа
 *
 * @param value указатель на число децимал
 * @param Владимир
 * @return int степень
 */
int s21_scale(const s21_decimal value) { return (char)(value.bits[3] >> 16); }

/**
 * @brief Подгон числа до нужной степени
 *
 * @param buffer указатель на строку, в которой сохранено число
 * @param num разница между текущей степенью и нужной
 * @param Владимир
 * @return int возвращает ОК или INF если число слишком большое
 */
int s21_shift_scale_str(char *buffer, const int num) {
  int rez = OK;
  if (num >= 0) {
    for (unsigned int j = 0; j < (strlen(buffer) - num); j++)
      buffer[j] = buffer[j + num], buffer[j + num] = '0';
  } else {
    for (int j = (int)strlen(buffer) - 1; j >= -num; j--)
      buffer[j] = buffer[j + num], buffer[j + num] = '0';
  }
  return rez;
}

/**
 * @brief Сложение двух чисел в десятичном виде, хранящееся в строке
 *
 * @param value_1 первое слагаемое
 * @param value_2 второе слагаемое
 * @param result результат сложения
 * @param Владимир
 */
void s21_add_str(const char *val_1, const char *val_2, char *rez) {
  for (int i = MAXSTR - 2, buf = 0; i >= 0; i--) {
    if (val_1[i] != '0' || val_2[i] != '0' || buf != 0) {
      int temp = (val_1[i] - '0') + buf + (val_2[i] - '0');
      buf = (temp >= 10) ? 1 : 0;
      if (temp >= 10) temp -= 10;
      rez[i] = temp + '0';
    }
  }
}

/**
 * @brief Вычитание двух чисел в десятичном виде, хранящееся в строке
 *
 * @param value_1 первое число
 * @param value_2 второе число
 * @param result результат вычитания
 * @param Владимир
 */
void s21_sub_str(const char *val_1, const char *val_2, char *rez) {
  for (int i = MAXSTR - 2, buf = 0; i >= 0; i--) {
    if (val_1[i] != '0' || val_2[i] != '0' || buf != 0) {
      int temp = (val_1[i] - '0') - buf - (val_2[i] - '0');
      buf = (temp < 0) ? 1 : 0;
      if (temp < 0) temp += 10;
      rez[i] = temp + '0';
    }
  }
}

/**
 * @brief Инициализация нулевого массива. Заполнение его нулями и ставит символ
 * конца строки в конце. str[99]='\0'
 *
 * @param val массив символов
 * @param Владимир
 */
void s21_init(char *val) {
  for (int i = 0; i < MAXSTR; i++) val[i] = i < (MAXSTR - 1) ? '0' : '\0';
}

/**
 * @brief Проверка, если число децимал равно нулю, то обнуляется степень и знак
 *
 * @param val указатель на число децимал
 * @param Владимир
 * @return int 1 - число равно 0, 0 - число не равно нулю
 */
int s21_test_zero(s21_decimal *val) {
  int rez = 0;
  if (val->bits[0] == 0 && val->bits[1] == 0 && val->bits[2] == 0)
    val->bits[3] = 0, rez = 1;
  return rez;
}

/**
 * @brief Сложение двух чисел Decimal
 *
 * @param value_1 первое слагаемое
 * @param value_2 второе слагаемое
 * @param result результат сложения
 * @param Владимир
 * @return int возвращают код ошибки
 */
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int ret = OK;
  s21_decimal temp_res;
  temp_res.bits[0] = temp_res.bits[1] = temp_res.bits[2] = temp_res.bits[3] = 0;
  int sign1 = s21_bit(value_1, 127);
  int sign2 = s21_bit(value_2, 127);
  if (sign1 && sign2) {
    s21_toggle_bit(&temp_res, 127);
  } else {
    if (sign1 > sign2) {
      s21_toggle_bit(&value_1, 127);
      ret = s21_sub(value_2, value_1, &temp_res);
    } else {
      if (sign1 < sign2) {
        s21_toggle_bit(&value_2, 127);
        ret = s21_sub(value_1, value_2, &temp_res);
      }
    }
  }
  if (sign1 == sign2 && !ret) {
    char val_1[MAXSTR], val_2[MAXSTR], rez[MAXSTR];
    s21_init(val_1), s21_init(val_2), s21_init(rez);
    s21_str_scale(&value_1, &value_2, val_1, val_2);
    s21_add_str(val_1, val_2, rez);
    int max_scale = s21_scale(value_1);
    ret = s21_buh_round(rez, &max_scale, 0);
    if (!ret) {
      temp_res.bits[3] ^= (max_scale << 16);
      s21_div_decimal_to_two(&temp_res, rez);
      s21_test_zero(&temp_res);
    }
    if (ret == 1) ret = (s21_bit(value_1, 127)) ? NEGATIVE_INF : INF;
  }
  *result = temp_res;
  return ret;
}

/**
 * @brief Вычитание чисел Decimal
 *
 * @param value_1 первое число
 * @param value_2 второе число
 * @param result результат вычитания
 * @param Владимир
 * @return int возвращают код ошибки
 */
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int ret = OK;
  s21_decimal temp_res;
  temp_res.bits[0] = temp_res.bits[1] = temp_res.bits[2] = temp_res.bits[3] = 0;
  int sign1 = s21_bit(value_1, 127);
  int sign2 = s21_bit(value_2, 127);
  if (sign1 != sign2) {
    if (sign1)
      s21_toggle_bit(&value_1, 127);
    else
      s21_toggle_bit(&value_2, 127);
    ret = s21_add(value_1, value_2, &temp_res);
    if (sign1) s21_toggle_bit(&temp_res, 127);
  } else {
    char val_1[MAXSTR], val_2[MAXSTR];
    s21_init(val_1), s21_init(val_2);
    s21_str_scale(&value_1, &value_2, val_1, val_2);
    char rez[MAXSTR];
    s21_init(rez);
    if (strcmp(val_1, val_2) >= 0) {
      if (sign1) s21_toggle_bit(&temp_res, 127);
      s21_sub_str(val_1, val_2, rez);

    } else {
      if (!sign1) s21_toggle_bit(&temp_res, 127);
      s21_sub_str(val_2, val_1, rez);
    }
    int max_scale = s21_scale(value_1);
    ret = s21_buh_round(rez, &max_scale, 0);
    if (!ret) {
      temp_res.bits[3] ^= (max_scale << 16);
      s21_div_decimal_to_two(&temp_res, rez);
      s21_test_zero(&temp_res);
    }
  }
  if (ret == 1)
    ret = (s21_bit(temp_res, 127)) ? NEGATIVE_INF : INF;
  else
    s21_test_zero(&temp_res);

  *result = temp_res;

  return ret;
}

/**
 * @brief Функция преобразует Decimal в десятичный вид в строку и выравнивает
 * показатели степени
 *
 * @param value_1 первое число decimal
 * @param value_2 второе число decimal
 * @param val_1 первое число преобразованное в строку
 * @param val_2 второе число преобразованное в строку
 * @param Владимир
 */
void s21_str_scale(s21_decimal *value_1, s21_decimal *value_2, char *val_1,
                   char *val_2) {
  s21_mul_two_to_decimal(*value_1, val_1);
  s21_mul_two_to_decimal(*value_2, val_2);
  int scale1 = s21_scale(*value_1), scale2 = s21_scale(*value_2);
  int scale = scale1 - scale2;
  if (scale > 0) {
    s21_shift_scale_str(val_2, scale);
    value_2->bits[3] |= (s21_scale(*value_1) << 16);
  }
  if (scale < 0) {
    s21_shift_scale_str(val_1, -scale);
    value_1->bits[3] |= (s21_scale(*value_2) << 16);
  }
}

/**
 * @brief Функция преобразования из типа float в тип Decimal
 *
 * @param src исходное число float
 * @param dst указатель на число decimal, в которое сохраняем результат
 * @param Владимир
 * @return int 0 - OK, 1 - ошибка конвертации
 */
int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int rezult = OK;
  dst->bits[0] = dst->bits[1] = dst->bits[2] = dst->bits[3] = 0;
  if (fabs(src) < FLOAT_MIN || isnan(src) || isinf(src)) {
    rezult = ERROR;
  } else {
    uint32_t fl;
    memcpy(&fl, &src, sizeof(uint32_t));
    if ((fl >> 31) & 1u) s21_toggle_bit(dst, 127);
    int exp = (int)((unsigned char)(fl >> 23)) - 127;
    float mantisa = 1.0;
    if (exp == -127) mantisa = 0.0, exp += 127;
    for (int i = 22; i >= 0; i--)
      if ((fl >> i) & 1u) mantisa += pow(2, -(23 - i));
    mantisa *= pow(2, exp);
    char buffer[MAXSTR];
    s21_init(buffer);
    snprintf(buffer, MAXSTR, "%.29f", mantisa);
    int val = strlen(buffer) - 1;
    char *ach;
    ach = strchr(buffer, '.');
    if (ach != NULL) {  // считаем коэффициент масштабирования
      int buflen = 0;
      buflen = ach - buffer + 1;
      for (unsigned int j = buflen - 1; j < strlen(buffer); j++)
        buffer[j] = buffer[j + 1];
      buflen = strlen(buffer) - buflen + 1;
      buffer[val--] = '\0';
      rezult = s21_buh_round(buffer, &buflen, 1);
      if (buflen) dst->bits[3] ^= (buflen << 16);
    }
    s21_div_decimal_to_two(dst, buffer);
  }
  return rezult;
}

/**
 * @brief Функция удаляет не значащие нули в начале строки и после запятой,
 * уменьшая степень
 *
 * @param buffer строка с числом в десятичном виде
 * @param buflen показатель степени
 * @param Владимир
 */
void s21_buh_round_zero(char *buffer, int *buflen) {
  int val_str = strlen(buffer) - 1;
  while ((*buflen) > 0 && val_str > 0 && buffer[val_str] == '0')
    val_str--, (*buflen)--;
  buffer[val_str + 1] = '\0';
  int i = 0;
  while (buffer[i] == '0' && buffer[i + 1] != '\0') i++;
  for (unsigned int j = 0; j < strlen(buffer) - i; j++)
    buffer[j] = buffer[j + i];
  buffer[strlen(buffer) - i] = '\0';
}

/**
 * @brief Функция округляет пока нужно девятку изменить на ноль
 *
 * @param buffer строка с числом в десятичном виде
 * @param buflen показатель степени
 * @param val номер символа в строке, по который нужно обрезать
 * @param Владимир
 */
int s21_buh_round_nine(char *buffer, int *buflen, int val) {
  buffer[val] = '\0';
  val--;
  int res = 0;
  while (buffer[val] == '9' && val > 0) buffer[val--] = '0';
  if (val == 0 && buffer[val] == '9') {
    buffer[0] = '0';
    for (int j = strlen(buffer); j >= 0; j--) buffer[j + 1] = buffer[j];
    buffer[0] = '1';
    res = s21_buh_round(buffer, buflen, 0);
  } else {
    buffer[val]++;
  }
  return res;
}

/**
 * @brief Функция бухгалтерского округления, попутно удаляет не значащие нули в
 * начале строки и после запятой, уменьшая степень
 *
 * @param buffer строка с числом в десятичном виде
 * @param buflen показатель степени
 * @param flag 0-бухгалтерское огругление; 1-округление до 7 значащих цифр;
 * 2-простое округление
 * @param Владимир
 * @return int возвращает ОК или INF при числе выходящим за границы Decimal
 */
int s21_buh_round(char *buffer, int *buflen, int flag) {
  int rez = OK;
  s21_buh_round_zero(buffer, buflen);
  int val = strlen(buffer);
  if (val - *buflen <= MAXDEC && *buflen - val <= MAXEXP) {
    if (*buflen > 28) val -= *buflen - MAXEXP, *buflen = MAXEXP;
    if (val > 29) *buflen -= (val - MAXDEC), val = MAXDEC;
    if (val > 0 && *buflen >= 0) {
      if (flag == 1 && val > FLOAT_ROUND) {
        for (int j = FLOAT_ROUND + 1; j < (int)strlen(buffer); j++)
          buffer[j] = '0';
        flag = 0;
        *buflen =
            (*buflen - val + FLOAT_ROUND > 0) ? *buflen - val + FLOAT_ROUND : 0;
        val = FLOAT_ROUND;
        // flag=2; если все таки нужно простое округление на флоате
      }
      if (buffer[val] > '5' ||
          (buffer[val] == '5' &&
           ((buffer[val - 1] % 2 == 1 && flag == 0) || flag == 2))) {
        if (buffer[val - 1] == '9')
          rez = s21_buh_round_nine(buffer, buflen, val);
        else
          buffer[val - 1]++, buffer[val] = '0';
      }
      if (*buflen > 0 || val == 29) buffer[val] = '\0';
      s21_buh_round_zero(buffer, buflen);
    }
    if (*buflen <= 0 && !rez) {
      val -= *buflen, *buflen = 0;
      if (val > MAXDEC || (val == MAXDEC && strcmp(buffer, DEC_MAX) > 0))
        rez = INF;
    }
  } else {
    rez = val - *buflen > MAXDEC ? INF : NEGATIVE_INF;
  }
  if (rez) {
    buffer[0] = '0';
    buffer[1] = '\0';
    *buflen = 0;
  }
  return rez;
}

/**
 * @brief Функция деления десятичного числа на 2, для преобразования его в
 * двоичный код
 *
 * @param dst указатель на число децимал для записи результата деления
 * @param buffer строка с десятичным числом
 * @param Владимир
 */
void s21_div_decimal_to_two(s21_decimal *dst, char *buffer) {
  int binar = 0, bit = 0;
  while (strcspn(buffer, ".123456789") != strlen(buffer)) {
    int binar_val = 0;
    for (unsigned int j = 0; j < strlen(buffer); j++) {
      int q = 0;
      if (binar_val == 1) q = 10;
      binar_val = ((buffer[j] - '0') + q) % 2;
      buffer[j] = (((buffer[j] - '0') + q) / 2) + '0';
    }
    if (binar == 32) binar = 0, bit++;
    if (binar_val) s21_toggle_bit(dst, 32 * bit + binar);
    binar++;
  }
}

/**
 * @brief Функция умножения двоичного числа на 2, для преобразования его в
 * десятичное
 *
 * @param src указатель на число децимал
 * @param buffer строка для сохранения десятичного числа
 * @param Владимир
 */
void s21_mul_two_to_decimal(const s21_decimal src, char *buffer) {
  int bit = 2;
  for (int j = 32; bit > -1; j--) {
    for (int i = MAXSTR - 2, one = 0; i > 0; i--) {
      int temp = (buffer[i] - '0') * 2 + one;
      one = (temp > 9) ? 1 : 0;
      if (temp > 9) temp -= 10;
      buffer[i] = temp + '0';
    }
    buffer[MAXSTR - 2] =
        ((buffer[MAXSTR - 2] - '0') + s21_bit(src, 32 * bit + j)) + '0';
    if (j == 0) j = 32, bit--;
  }
}

/**
 * @brief Функция преобразование из типа Decimal в тип Float
 *
 * @param src число Decimal, которое нужно преобразовать
 * @param dst указатель на число float, куда сохранить результат
 * преобразования
 * @param Владимир
 * @return int 0 - OK, 1 - ошибка конвертации
 */
int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  char buffer[MAXSTR];
  s21_init(buffer);
  int rezult = OK;
  s21_mul_two_to_decimal(src, buffer);
  int exp = s21_scale(src);
  if (strcmp(buffer, DEC_MAX) > 0 && exp > 0) rezult = ERROR;
  for (unsigned int j = 0; j < strlen(buffer) - exp - 1; j++)
    buffer[j] = buffer[j + 1];
  buffer[strlen(buffer) - exp - 1] = '.';
  int val = strcspn(buffer, ".123456789");
  char temp[MAXSTR];
  s21_init(temp);
  snprintf(temp, strlen(&buffer[val - 1]) + 1, "%s", &buffer[val - 1]);
  *dst = atof(temp);
  if (s21_bit(src, 127)) *dst *= -1;
  if (fabs(*dst) < FLOAT_MIN || isinf(*dst)) rezult = ERROR, *dst = 0.0;
  return rezult;
}

/**
 * @brief Побитовый сдвиг вправо
 *
 * @param value указатель на число децимал
 * @param num количество сдвигаемых битов
 * @param Владимир
 */
void s21_shift_bits_right(s21_decimal *value, const int num) {
  for (int j = 0; j < num; j++) {
    value->bits[0] >>= 1;
    if (s21_bit(*value, 31) != s21_bit(*value, 32)) s21_toggle_bit(value, 31);
    value->bits[1] >>= 1;
    if (s21_bit(*value, 63) != s21_bit(*value, 64)) s21_toggle_bit(value, 63);
    value->bits[2] >>= 1;
    if (s21_bit(*value, 95)) s21_toggle_bit(value, 95);
  }
}

/**
 * @brief Побитовый сдвиг влево
 *
 * @param value указатель на число децимал
 * @param num количество сдвигаемых битов
 * @param Владимир
 * @return int код ошибки
 */
int s21_shift_bits_left(s21_decimal *value, const int num) {
  int rez = OK;
  for (int j = 0; j < num; j++) {
    value->bits[2] <<= 1;
    if (s21_bit(*value, 63) != s21_bit(*value, 64)) s21_toggle_bit(value, 64);
    value->bits[1] <<= 1;
    if (s21_bit(*value, 31) != s21_bit(*value, 32)) s21_toggle_bit(value, 32);
    value->bits[0] <<= 1;
  }
  return rez;
}

/**
 * @brief Подсчет количества битов
 *
 * @param value указатель на число децимал
 * @param Владимир
 * @return int количество
 */
int s21_sum_bits(s21_decimal *value) {
  int end = 0;
  for (int i = 96; i >= 0; i--) {
    if (s21_bit(*value, i) == 1) {
      end = i + 1;
      i = -1;
    }
  }
  return end;
}

/**
 * @brief Подсчет количества битов
 *
 * @param dst указатель на число децимал
 * @param src десятичное целое число
 * @param Никита
 * @return int код ошибки
 */
int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  int rez = OK;
  char buffer[MAXSTR];
  snprintf(buffer, sizeof(buffer), "%d", src);
  *dst = dec_null;
  if (src < 0) {
    s21_toggle_bit(dst, 127);
    src *= -1;
  }
  unsigned int tmp = *((unsigned int *)&src);
  if (tmp > MAXINT) {
    rez = ERROR;
  }
  s21_div_decimal_to_two(dst, buffer);
  return rez;
}

/**
 * @brief Из децимал в int
 *
 * @param dst указатель на десятичное целое число
 * @param src число децимал
 * @param Никита
 * @return int код ошибки
 */
int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int rez = OK;
  int scale = s21_scale(src);
  char buffer[MAXSTR] = {0};
  s21_init(buffer);
  int sign = 0;
  if (s21_bit(src, 127) == 0) {
    sign = 1;
  } else {
    sign = -1;
  }
  s21_mul_two_to_decimal(src, buffer);
  *dst = atoi(buffer) * sign;
  int tmp = *dst;
  while (scale > 0) {
    tmp = tmp / 10;
    scale--;
  }
  *dst = tmp;
  if (src.bits[1] > 0 || src.bits[2] > 0) {
    *dst = 0;
    rez = ERROR;
  }
  return rez;
}

/**
 * @brief Умножение двух чисел Decimal
 *
 * @param value_1 первой множитель
 * @param value_2 второй множитель
 * @param result результат умножения
 * @param Андрей
 * @return int возвращают код ошибки
 */
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int flag = OK;
  char str_1[200] = {0}, str_2[100] = {0}, res[100] = {0};
  int exp_res = s21_scale(value_1) + s21_scale(value_2);
  s21_init(str_1);
  s21_init(str_2);
  s21_init(res);
  s21_mul_two_to_decimal(value_1, str_1);
  s21_mul_two_to_decimal(value_2, str_2);
  for (int i = strlen(str_2); i > 0; i--) {
    for (int j = str_2[i - 1] - 48; j > 0; j--) s21_add_str(res, str_1, res);
    s21_shift_str(str_1);
  }
  if (s21_buh_round(res, &exp_res, 0) == 1) {
    flag = INF;
  } else {
    *result = dec_null;
    s21_div_decimal_to_two(result, res);
    s21_set_exp(result, exp_res);
  }
  if ((s21_bit(value_2, 127) ^ s21_bit(value_1, 127)) == 1) {
    if (flag == INF)
      flag = NEGATIVE_INF;
    else
      s21_toggle_bit(result, 127);
  }
  return flag;
}

/**
 * @brief Сравнение двух чисел Decimal
 * @param value_1 первое число
 * @param value_2 второе число
 * @param Андрей
 * @return int 0 - FALSE, 1 - TRUE (value_1 меньше value_2)
 */
int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  s21_decimal res;
  s21_sub(value_1, value_2, &res);
  return s21_bit(res, 127);
}

/**
 * @brief Сравнение двух чисел Decimal
 * @param value_1 первое число
 * @param value_2 второе число
 * @param Андрей
 * @return int 0 - FALSE, 1 - TRUE (value_1 меньше value_2)
 */
int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2) {
  return (s21_is_less(value_1, value_2) || s21_is_equal(value_1, value_2));
}

/**
 * @brief Сравнение двух чисел Decimal
 * @param value_1 первое число
 * @param value_2 второе число
 * @param Андрей
 * @return int 0 - FALSE, 1 - TRUE (value_1 больше value_2)
 */
int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
  s21_decimal res;
  s21_sub(value_2, value_1, &res);
  return s21_bit(res, 127);
}

/**
 * @brief Сравнение двух чисел Decimal
 * @param value_1 первое число
 * @param value_2 второе число
 * @param Андрей
 * @return int 0 - FALSE, 1 - TRUE (value_1 больше или равно value_2)
 */
int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
  return (s21_is_greater(value_1, value_2) || s21_is_equal(value_1, value_2));
}

/**
 * @brief Сравнение двух чисел Decimal
 * @param value_1 первое число
 * @param value_2 второе число
 * @param Андрей
 * @return int 0 - FALSE, 1 - TRUE (value_1 равно value_2)
 */
int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  s21_decimal res;
  int flag = FALSE;
  s21_sub(value_2, value_1, &res);
  if (res.bits[0] == 0 && res.bits[1] == 0 && res.bits[2] == 0) flag = TRUE;
  return flag;
}

/**
 * @brief Сравнение двух чисел Decimal
 * @param value_1 первое число
 * @param value_2 второе число
 * @param Андрей
 * @return int 0 - FALSE, 1 - TRUE (value_1 не равно value_2)
 */
int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2) {
  int flag = FALSE;
  if (s21_is_equal(value_1, value_2) == 0) flag = TRUE;
  return flag;
}

/**
 * @brief умножение указанного Decimal на -1
 * @param value число Decimal
 * @param result результат умножения
 * @param Андрей
 * @return int 0 - OK, 1 - ошибка вычисления
 */
int s21_negate(s21_decimal value, s21_decimal *result) {
  int flag = OK;
  if (s21_scale(value) > 28) {
    flag = ERROR;
  } else {
    *result = value;
    s21_toggle_bit(result, 127);
  }
  return flag;
}

/**
 * @brief округление Decimal до ближайшего целого числа
 * @param value число Decimal
 * @param result результат округления
 * @param Андрей
 * @return int 0 - OK, 1 - ошибка вычисления
 */
int s21_round(s21_decimal value, s21_decimal *result) {
  int flag = OK;
  if (s21_scale(value) > 28) {
    flag = ERROR;
  } else {
    *result = dec_null;
    if (s21_scale(value) > 0) {
      int sign = 0;
      if (s21_bit(value, 127) == 1) {
        sign = 1;
        s21_toggle_bit(&value, 127);
      }
      s21_set_exp(&value, s21_scale(value) - 1);
      s21_truncate(value, result);
      char res[100] = {0};
      s21_init(res);
      s21_mul_two_to_decimal(*result, res);
      s21_decimal tmp = *result, one = {{1, 0, 0, 0}};

      *result = dec_null;
      s21_set_exp(&tmp, 1);
      s21_truncate(tmp, result);
      char tmp_str[100] = {0};
      s21_mul_two_to_decimal(*result, tmp_str);
      if ((strcmp(res, STR_0) != 0) && (sign == 1)) {
        s21_toggle_bit(result, 127);
        s21_toggle_bit(&one, 127);
      }
      if (res[strlen(res) - 1] >= 53) {
        tmp = *result;
        s21_add(tmp, one, result);
      }
    } else {
      *result = value;
    }
  }
  return flag;
}

/**
 * @brief Возвращает целые цифры указанного Decimal числа
 * @param value число Decimal
 * @param result результат округления
 * @param Андрей
 * @return int 0 - OK, 1 - ошибка вычисления
 */
int s21_truncate(s21_decimal value, s21_decimal *result) {
  int flag = OK;
  if (s21_scale(value) > 28) {
    flag = ERROR;
  } else {
    *result = dec_null;
    if (s21_scale(value) > 0) {
      int sign = 0;
      char res[100] = {0};
      s21_init(res);
      s21_mul_two_to_decimal(value, res);
      s21_shift_scale_str(res, -s21_scale(value));
      if (strcmp(res, STR_0) != 0 && s21_bit(value, 127) == 1) sign = 1;
      s21_div_decimal_to_two(result, res);
      if (sign == 1) result->bits[3] = MINUS;
    } else {
      *result = value;
    }
  }
  return flag;
}

/**
 * @brief Округляет указанное Decimal число до ближайшего целого числа
 * в сторону отрицательной бесконечности.
 * @param value число Decimal
 * @param result результат округления
 * @param Андрей
 * @return int 0 - OK, 1 - ошибка вычисления
 */
int s21_floor(s21_decimal value, s21_decimal *result) {
  int flag = OK;
  if (s21_scale(value) > 28) {
    flag = ERROR;
  } else {
    s21_decimal tmp = {{1, 0, 0, 0}};
    s21_truncate(value, result);
    if (s21_bit(value, 127) == 1) s21_sub(*result, tmp, result);
  }
  return flag;
}

/**
 * @brief умножение числа на 10
 * @param str указатель на строку, в которой сохранено число
 * @param Андрей
 */
void s21_shift_str(char *str) {
  for (size_t i = 0; i < strlen(str); i++) str[i] = str[i + 1];
  str[strlen(str)] = '0';
}

/**
 * @brief устанавливает степень числа децимал
 * @param value указатель на число децимал
 * @param exp степень
 * @param Андрей
 * @return int возвращает TRUE или FALSE указана неверная степень
 */
int s21_set_exp(s21_decimal *value, int exp) {
  int flag = TRUE, sign = s21_bit(*value, 127);
  if (exp >= 0 && exp <= 28) {
    // value->bits[3] >>= 16;
    value->bits[3] = exp;
    value->bits[3] <<= 16;
    if (sign == 1) s21_toggle_bit(value, 127);
  } else {
    flag = FALSE;
  }
  return flag;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_decimal ten = dec_null;
  ten.bits[0] = 10;
  int rez = OK;
  if (s21_test_zero(&value_2)) {
    rez = s21_NAN;
  } else {
    int sign_v1 = s21_bit(value_1, 127), sign_v2 = s21_bit(value_2, 127);
    int sign_res = sign_v1 == sign_v2 ? 0 : 1;
    if (sign_v1 == 1) s21_toggle_bit(&value_1, 127);
    if (sign_v2 == 1) s21_toggle_bit(&value_2, 127);
    s21_decimal dec_one = dec_null, tmp = dec_null;
    *result = dec_null;
    dec_one.bits[0] = 1;
    int flag_mul = 0, exp_1 = s21_scale(value_1), exp_2 = s21_scale(value_2);
    if (s21_is_greater_or_equal(value_1, value_2) == TRUE)
      s21_toggle_bit(&tmp, 0);
    while (s21_is_less(value_2, dec_one)) {
      if (exp_1 >= exp_2) {
        exp_1 = exp_1 - exp_2;
        s21_set_exp(&value_1, exp_1);
      } else {
        int count_mul = exp_2 - exp_1;
        while (count_mul > 0) {
          flag_mul = s21_mul(value_1, ten, &value_1);
          if (flag_mul != 0) break;
          count_mul--;
        }
        s21_set_exp(&value_1, 0);
      }
      s21_set_exp(&value_2, 0);
    }
    while (s21_is_less(value_1, value_2)) {
      int tmp1 = exp_1;
      s21_set_exp(&value_1, 0);
      flag_mul = s21_mul(value_1, ten, &value_1);
      if (flag_mul != 0 || s21_is_equal(value_1, dec_null)) break;
      s21_set_exp(&value_1, tmp1);
    }
    s21_decimal tmp_value_1 = value_1;
    int exp_res = s21_scale(value_1) + s21_scale(value_2);
    if (s21_is_less_or_equal(value_2, value_1) == TRUE) {
      while (1) {
        int bits_value_1 = s21_sum_bits(&value_1);
        int bits_value_2 = s21_sum_bits(&value_2);
        s21_decimal tmp_value_2 = value_2;
        while (bits_value_1 > bits_value_2) {
          s21_shift_bits_left(&tmp_value_2, 1);
          s21_shift_bits_left(&tmp, 1);
          bits_value_2++;
          if (bits_value_1 == bits_value_2) {
            if (s21_is_greater_or_equal(value_1, tmp_value_2)) {
              break;
            } else {
              s21_shift_bits_right(&tmp_value_2, 1);
              s21_shift_bits_right(&tmp, 1);
            }
          }
        }
        s21_sub(value_1, tmp_value_2, &value_1);
        s21_add(*result, tmp, result);
        tmp = dec_null;
        s21_toggle_bit(&tmp, 0);
        if (s21_is_less(value_1, value_2)) {
          if (s21_test_zero(&value_1)) {
            break;
          } else {
            int res_mul = s21_mul(tmp_value_1, ten, &tmp_value_1);
            if (res_mul == 1) break;
            value_1 = tmp_value_1;
            exp_res++;
            *result = dec_null;
          }
        }
      }
      s21_set_exp(result, exp_res);
      if (sign_res == 1) {
        if (flag_mul == INF) flag_mul = NEGATIVE_INF;
        s21_toggle_bit(result, 127);
      }
    }
    rez = flag_mul;
  }
  return rez;
}

int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int rez = OK, flag = 0, sign_value1 = 0;
  s21_decimal integer_dec = dec_null, mul_intdec_val2 = dec_null,
              mod_dec = dec_null;
  if (s21_bit(value_1, 127) == 1) {
    sign_value1 = 1;
    s21_toggle_bit(&value_1, 127);
  }
  if (s21_bit(value_2, 127) == 1) {
    s21_toggle_bit(&value_2, 127);
  }
  flag = s21_div(value_1, value_2, result);
  if (flag != 0) {
    rez = flag;
  }
  s21_truncate(*result, &integer_dec);
  s21_mul(integer_dec, value_2, &mul_intdec_val2);
  s21_sub(value_1, mul_intdec_val2, &mod_dec);
  if (flag == INF && sign_value1 == 1) {
    flag = NEGATIVE_INF;
    rez = flag;
  }
  *result = mod_dec;
  if (s21_scale(*result) != 0) {
    *result = dec_null;
  }
  return rez;
}
