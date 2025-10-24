#ifndef LIB_H
#define LIB_H

#include <stdint.h>

// Структура для передачи данных о диапазоне вычислений
struct FactorialArgs {
    uint64_t begin;
    uint64_t end;
    uint64_t mod;
};

// Функция для умножения по модулю (используется в факториале)
uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod);

// Функция для вычисления факториала диапазона по модулю
uint64_t Factorial(const struct FactorialArgs *args);

#endif