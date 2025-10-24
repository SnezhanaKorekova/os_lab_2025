#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Два мьютекса для демонстрации deadlock
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

void* thread1_function(void* arg) {
    printf("Поток 1: Пытаюсь захватить мьютекс 1...\n");
    pthread_mutex_lock(&mutex1);
    printf("Поток 1: Захватил мьютекс 1\n");
    
    // Имитация работы - даем время второму потоку захватить второй мьютекс
    printf("Поток 1: Спим 1 секунду...\n");
    sleep(1);
    
    printf("Поток 1: Пытаюсь захватить мьютекс 2...\n");
    pthread_mutex_lock(&mutex2); // ЗДЕСЬ ПРОИЗОЙДЕТ DEADLOCK!
    printf("Поток 1: Захватил мьютекс 2\n"); // Эта строка не выполнится
    
    // Критическая секция (никогда не выполнится из-за deadlock)
    printf("Поток 1: Вхожу в критическую секцию\n");
    sleep(1);
    printf("Поток 1: Выхожу из критической секции\n");
    
    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);
    
    return NULL;
}

void* thread2_function(void* arg) {
    printf("Поток 2: Пытаюсь захватить мьютекс 2...\n");
    pthread_mutex_lock(&mutex2);
    printf("Поток 2: Захватил мьютекс 2\n");
    
    // Имитация работы - даем время первому потоку захватить первый мьютекс
    printf("Поток 2: Спим 1 секунду...\n");
    sleep(1);
    
    printf("Поток 2: Пытаюсь захватить мьютекс 1...\n");
    pthread_mutex_lock(&mutex1); // ЗДЕСЬ ПРОИЗОЙДЕТ DEADLOCK!
    printf("Поток 2: Захватил мьютекс 1\n"); // Эта строка не выполнится
    
    // Критическая секция (никогда не выполнится из-за deadlock)
    printf("Поток 2: Вхожу в критическую секцию\n");
    sleep(1);
    printf("Поток 2: Выхожу из критической секции\n");
    
    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
    
    return NULL;
}

int main() {
    pthread_t thread1, thread2;
    
    printf("=== Демонстрация Deadlock (Взаимной блокировки) ===\n");
    printf("Эта программа покажет классический сценарий deadlock:\n");
    printf("- Поток 1 захватывает мьютекс 1, потом пытается захватить мьютекс 2\n");
    printf("- Поток 2 захватывает мьютекс 2, потом пытается захватить мьютекс 1\n");
    printf("- Оба потока будут вечно ждать друг друга!\n\n");
    
    // Создаем потоки
    if (pthread_create(&thread1, NULL, thread1_function, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }
    
    if (pthread_create(&thread2, NULL, thread2_function, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }
    
    // Даем потокам время для входа в состояние deadlock
    printf("Основной поток: Ждем 3 секунды чтобы потоки вошли в deadlock...\n");
    sleep(3);
    
    // Пытаемся присоединиться к потокам
    printf("\nОсновной поток: Проверяем состояние потоков...\n");
    printf("Основной поток: Если вы видите это сообщение, потоки скорее всего в deadlock.\n");
    printf("Основной поток: Программа зависнет на вызовах pthread_join()...\n\n");
    
    // Эти вызовы заблокируются навсегда из-за deadlock
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    // Эти строки никогда не выполнятся из-за deadlock:
    printf("Это сообщение никогда не будет напечатано из-за deadlock!\n");
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    
    return 0;
}