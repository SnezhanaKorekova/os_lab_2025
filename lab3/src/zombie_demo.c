#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    printf("=== Демонстрация зомби-процессов ===\n");
    
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        // ДОЧЕРНИЙ ПРОЦЕСС
        printf("Дочерний процесс: PID = %d, PPID = %d\n", getpid(), getppid());
        printf("Дочерний процесс: Завершаю работу через 5 секунд...\n");
        sleep(5);
        printf("Дочерний процесс: Завершаюсь!\n");
        exit(42);  // Завершаемся с кодом 42
    } else {
        // РОДИТЕЛЬСКИЙ ПРОЦЕСС
        printf("Родительский процесс: PID = %d\n", getpid());
        printf("Родительский процесс: Создал дочерний процесс с PID = %d\n", pid);
        printf("Родительский процесс: НЕ буду вызывать wait() - создам зомби!\n");
        printf("Родительский процесс: Спим 30 секунд...\n");
        
        // НЕ вызываем wait() - создаем зомби!
        sleep(30);
        
        printf("Родительский процесс: Проснулся, теперь подожду ребенка...\n");
        
        int status;
        waitpid(pid, &status, 0);  // Теперь ждем ребенка
        
        if (WIFEXITED(status)) {
            printf("Родительский процесс: Дочерний процесс завершился с кодом %d\n", 
                   WEXITSTATUS(status));
        }
        
        printf("Родительский процесс: Завершаю работу\n");
    }
    
    return 0;
}