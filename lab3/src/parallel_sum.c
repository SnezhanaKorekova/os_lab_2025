#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>

#include <pthread.h>

#include "utils.h"  // Для GenerateArray
#include "sum.h"    // Для Sum и структур

int main(int argc, char **argv) {
    uint32_t threads_num = 0;
    uint32_t array_size = 0;
    uint32_t seed = 0;

    // Парсинг аргументов командной строки
    while (1) {
        static struct option options[] = {
            {"threads_num", required_argument, 0, 't'},
            {"array_size", required_argument, 0, 'a'},
            {"seed", required_argument, 0, 's'},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "t:a:s:", options, &option_index);

        if (c == -1) break;

        switch (c) {
            case 't':
                threads_num = atoi(optarg);
                if (threads_num <= 0) {
                    printf("Количество потоков должно быть положительным числом\n");
                    return 1;
                }
                break;
            case 'a':
                array_size = atoi(optarg);
                if (array_size <= 0) {
                    printf("Размер массива должен быть положительным числом\n");
                    return 1;
                }
                break;
            case 's':
                seed = atoi(optarg);
                if (seed <= 0) {
                    printf("Seed должен быть положительным числом\n");
                    return 1;
                }
                break;
            case '?':
                break;
            default:
                printf("getopt вернул код символа 0%o?\n", c);
        }
    }

    if (threads_num == 0 || array_size == 0 || seed == 0) {
        printf("Использование: %s --threads_num \"число\" --array_size \"число\" --seed \"число\"\n", argv[0]);
        return 1;
    }

    printf("=== Параллельное суммирование массива ===\n");
    printf("Количество потоков: %u\n", threads_num);
    printf("Размер массива: %u\n", array_size);
    printf("Seed: %u\n", seed);

    // Генерация массива (НЕ входит в замер времени)
    printf("Генерируем массив...\n");
    int *array = malloc(sizeof(int) * array_size);
    GenerateArray(array, array_size, seed);
    printf("Массив сгенерирован\n");

    // Подготовка аргументов для потоков
    struct SumArgs args[threads_num];
    int part_size = array_size / threads_num;
    
    for (uint32_t i = 0; i < threads_num; i++) {
        args[i].array = array;
        args[i].begin = i * part_size;
        args[i].end = (i == threads_num - 1) ? array_size : (i + 1) * part_size;
        printf("Поток %u: элементы [%d - %d)\n", i, args[i].begin, args[i].end);
    }

    pthread_t threads[threads_num];
    
    // Начало замера времени
    printf("Начинаем параллельное суммирование...\n");
    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    // Создание потоков
    for (uint32_t i = 0; i < threads_num; i++) {
        if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
            printf("Ошибка: не удалось создать поток!\n");
            free(array);
            return 1;
        }
    }
    printf("Все потоки созданы\n");

    // Ожидание завершения потоков и сбор результатов
    int total_sum = 0;
    for (uint32_t i = 0; i < threads_num; i++) {
        int sum = 0;
        pthread_join(threads[i], (void **)&sum);
        total_sum += sum;
        printf("Поток %u завершился, частичная сумма: %d\n", i, sum);
    }

    // Конец замера времени
    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);

    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

    free(array);
    
    printf("\n=== Результаты ===\n");
    printf("Общая сумма: %d\n", total_sum);
    printf("Затраченное время: %f мс\n", elapsed_time);
    printf("Суммирование завершено!\n");
    
    return 0;
}