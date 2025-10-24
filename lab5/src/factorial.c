#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

// Структура для передачи данных в поток
typedef struct {
    int start;
    int end;
    long long mod;
    long long partial_result;
    long long *global_result;
    pthread_mutex_t *mutex;
} thread_data_t;

// Функция, выполняемая в каждом потоке
void* calculate_partial_factorial(void* arg) {
    thread_data_t* data = (thread_data_t*) arg;
    data->partial_result = 1;
    
    printf("Поток: вычисляет от %d до %d\n", data->start, data->end);
    
    // Вычисляем частичный факториал
    for (int i = data->start; i <= data->end; i++) {
        data->partial_result = (data->partial_result * i) % data->mod;
    }
    
    // Критическая секция - защищаем мьютексом
    pthread_mutex_lock(data->mutex);
    *(data->global_result) = (*(data->global_result) * data->partial_result) % data->mod;
    pthread_mutex_unlock(data->mutex);
    
    printf("Поток: частичный результат = %lld\n", data->partial_result);
    
    return NULL;
}

// Функция для разбора аргументов командной строки
void parse_arguments(int argc, char* argv[], int* k, int* pnum, long long* mod) {
    // Значения по умолчанию
    *k = 10;
    *pnum = 4;
    *mod = 1000000007;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-k") == 0 && i + 1 < argc) {
            *k = atoi(argv[++i]);
        } else if (strncmp(argv[i], "--pnum=", 7) == 0) {
            *pnum = atoi(argv[i] + 7);
        } else if (strncmp(argv[i], "--mod=", 6) == 0) {
            *mod = atoll(argv[i] + 6);
        }
    }
}

int main(int argc, char* argv[]) {
    int k, pnum;
    long long mod;
    long long global_result = 1;
    
    // Разбор аргументов командной строки
    parse_arguments(argc, argv, &k, &pnum, &mod);
    
    printf("Вычисляем %d! mod %lld используя %d потоков\n", k, mod, pnum);
    
    // Проверка входных данных
    if (k <= 0 || pnum <= 0 || mod <= 0) {
        printf("Ошибка: Все параметры должны быть положительными\n");
        return 1;
    }
    
    // Если k меньше количества потоков, уменьшаем количество потоков
    if (pnum > k) {
        pnum = k;
        printf("Скорректировано количество потоков до %d (k=%d)\n", pnum, k);
    }
    
    pthread_t threads[pnum];
    thread_data_t thread_data[pnum];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    
    // Распределение работы между потоками
    int numbers_per_thread = k / pnum;
    int remainder = k % pnum;
    int current_start = 1;
    
    printf("\nРаспределение работы:\n");
    
    // Создание потоков
    for (int i = 0; i < pnum; i++) {
        int current_end = current_start + numbers_per_thread - 1;
        
        // Распределяем остаток по первым потокам
        if (remainder > 0) {
            current_end++;
            remainder--;
        }
        
        thread_data[i].start = current_start;
        thread_data[i].end = current_end;
        thread_data[i].mod = mod;
        thread_data[i].global_result = &global_result;
        thread_data[i].mutex = &mutex;
        
        printf("Поток %d: числа от %d до %d\n", i, current_start, current_end);
        
        if (pthread_create(&threads[i], NULL, calculate_partial_factorial, &thread_data[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
        
        current_start = current_end + 1;
    }
    
    // Ожидание завершения всех потоков
    for (int i = 0; i < pnum; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            return 1;
        }
    }
    
    // Уничтожение мьютекса
    pthread_mutex_destroy(&mutex);
    
    printf("\nФинальный результат: %d! mod %lld = %lld\n", k, mod, global_result);
    
    return 0;
}