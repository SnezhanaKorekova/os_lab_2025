#ifndef SUM_H
#define SUM_H

// Структура для передачи аргументов в поток
struct SumArgs {
    int *array;
    int begin;
    int end;
};

// Функция для подсчета суммы части массива
int Sum(const struct SumArgs *args);

// Функция-обертка для pthread
void *ThreadSum(void *args);

#endif