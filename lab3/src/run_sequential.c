#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {  // проверка аргументов
        printf("Usage: %s <seed> <array_size>\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();  // Создаем дочерний процесс
    
    if (pid == -1) {
        // Ошибка при создании процесса
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        // ДОЧЕРНИЙ ПРОЦЕСС - запускаем sequential_min_max (последовательный)
        printf("Child process: Starting sequential_min_max...\n");
        
        // Подготавливаем аргументы для exec
        char *args[] = {"./sequential_min_max", argv[1], argv[2], NULL};
        
        // Заменяем текущий процесс на sequential_min_max
        execvp(args[0], args);
        
        // Ошибка, если exec не сработал
        perror("exec failed");
        exit(1);
    } else {
        // РОДИТЕЛЬСКИЙ ПРОЦЕСС - ждем завершения дочернего
        printf("Parent process: Waiting for child to finish...\n");
        
        int status;
        waitpid(pid, &status, 0);  // Ждем завершения конкретного процесса
        
        if (WIFEXITED(status)) {  // проверяет на нормальное завершение
            printf("Parent process: Child finished with exit code %d\n", WEXITSTATUS(status));
        } else {
            printf("Parent process: Child terminated abnormally\n");
        }
    }
    
    return 0;
}