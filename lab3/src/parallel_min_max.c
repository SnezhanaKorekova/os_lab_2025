#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

// Глобальные переменные для обработчика сигнала
pid_t *child_pids = NULL;
int timeout = 0;
int global_pnum = 0;  // Выносим pnum в глобальную область

// Обработчик сигнала SIGALRM
void timeout_handler(int sig) {
    printf("Timeout reached! Sending SIGKILL to child processes...\n");
    
    if (child_pids != NULL) {
        for (int i = 0; i < global_pnum; i++) {  // Используем global_pnum
            if (child_pids[i] > 0) {
                kill(child_pids[i], SIGKILL);
                printf("Sent SIGKILL to child process %d\n", child_pids[i]);
            }
        }
    }
}

int main(int argc, char **argv) {
    int seed = -1;
    int array_size = -1;
    int pnum = -1;  // Локальная переменная
    bool with_files = false;
    timeout = 0;

    while (true) {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {
            {"seed", required_argument, 0, 0},
            {"array_size", required_argument, 0, 0},
            {"pnum", required_argument, 0, 0},
            {"by_files", no_argument, 0, 'f'},
            {"timeout", required_argument, 0, 't'},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "ft:", options, &option_index);

        if (c == -1) break;

        switch (c) {
            case 0:
                switch (option_index) {
                    case 0:
                        seed = atoi(optarg);
                        if (seed <= 0) {
                            printf("seed must be a positive number\n");
                            return 1;
                        }
                        break;
                    case 1:
                        array_size = atoi(optarg);
                        if (array_size <= 0) {
                            printf("array_size must be a positive number\n");
                            return 1;
                        }
                        break;
                    case 2:
                        pnum = atoi(optarg);
                        if (pnum <= 0) {
                            printf("pnum must be a positive number\n");
                            return 1;
                        }
                        break;
                    case 3:
                        with_files = true;
                        break;
                    default:
                        printf("Index %d is out of options\n", option_index);
                }
                break;
            case 'f':
                with_files = true;
                break;
            case 't':
                timeout = atoi(optarg);
                if (timeout <= 0) {
                    printf("timeout must be a positive number\n");
                    return 1;
                }
                printf("Timeout set to %d seconds\n", timeout);
                break;
            case '?':
                break;
            default:
                printf("getopt returned character code 0%o?\n", c);
        }
    }

    if (optind < argc) {
        printf("Has at least one no option argument\n");
        return 1;
    }

    if (seed == -1 || array_size == -1 || pnum == -1) {
        printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" [--by_files] [--timeout \"num\"]\n",
               argv[0]);
        return 1;
    }

    // Сохраняем pnum в глобальную переменную для обработчика
    global_pnum = pnum;

    // Выделяем память для хранения PID детей
    child_pids = malloc(global_pnum * sizeof(pid_t));
    if (child_pids == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    int *array = malloc(sizeof(int) * array_size);
    GenerateArray(array, array_size, seed);
    int active_child_processes = 0;

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    // Регистрируем обработчик сигнала если таймаут задан
    if (timeout > 0) {
        signal(SIGALRM, timeout_handler);
        alarm(timeout);
    }

    int part_size = array_size / pnum;
    
    for (int i = 0; i < pnum; i++) {
        pid_t child_pid = fork();
        if (child_pid >= 0) {
            active_child_processes += 1;
            child_pids[i] = child_pid;
            
            if (child_pid == 0) {
                // child process
                unsigned int begin = i * part_size;
                unsigned int end = (i == pnum - 1) ? array_size : (i + 1) * part_size;
                
                struct MinMax local_min_max = GetMinMax(array, begin, end);
                
                if (with_files) {
                    char filename_min[20], filename_max[20];
                    sprintf(filename_min, "min_%d.txt", i);
                    sprintf(filename_max, "max_%d.txt", i);
                    
                    FILE *file_min = fopen(filename_min, "w");
                    FILE *file_max = fopen(filename_max, "w");
                    if (file_min == NULL || file_max == NULL) {
                        printf("Failed to create files\n");
                        exit(1);
                    }
                    
                    fprintf(file_min, "%d", local_min_max.min);
                    fprintf(file_max, "%d", local_min_max.max);
                    fclose(file_min);
                    fclose(file_max);
                } else {
                    // use pipe here (можно оставить пустым для задания №2)
                }
                free(array);
                exit(0);
            }
        } else {
            printf("Fork failed!\n");
            free(child_pids);
            return 1;
        }
    }

    // Ожидание завершения дочерних процессов
    int completed_processes = 0;
    while (completed_processes < pnum) {
        int status;
        pid_t finished_pid = waitpid(-1, &status, WNOHANG);
        
        if (finished_pid > 0) {
            completed_processes++;
            active_child_processes--;
            
            for (int i = 0; i < pnum; i++) {
                if (child_pids[i] == finished_pid) {
                    child_pids[i] = -1;
                    break;
                }
            }
            
            if (WIFEXITED(status)) {
                printf("Child process %d finished normally with exit code %d\n", 
                       finished_pid, WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("Child process %d killed by signal %d\n", 
                       finished_pid, WTERMSIG(status));
            }
        } else if (finished_pid == 0) {
            sleep(1);
        } else {
            break;
        }
    }

    if (active_child_processes > 0) {
        printf("Some child processes are still running...\n");
    }

    // Отменяем будильник
    if (timeout > 0) {
        alarm(0);
    }

    struct MinMax min_max;
    min_max.min = INT_MAX;
    min_max.max = INT_MIN;

    for (int i = 0; i < pnum; i++) {
        int min = INT_MAX;
        int max = INT_MIN;

        if (with_files) {
            char filename_min[20], filename_max[20];
            sprintf(filename_min, "min_%d.txt", i);
            sprintf(filename_max, "max_%d.txt", i);
            
            FILE *file_min = fopen(filename_min, "r");
            FILE *file_max = fopen(filename_max, "r");
            if (file_min != NULL && file_max != NULL) {
                fscanf(file_min, "%d", &min);
                fscanf(file_max, "%d", &max);
                fclose(file_min);
                fclose(file_max);
                
                remove(filename_min);
                remove(filename_max);
            }
        }

        if (min < min_max.min) min_max.min = min;
        if (max > min_max.max) min_max.max = max;
    }

    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);

    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

    free(array);
    free(child_pids);

    printf("Min: %d\n", min_max.min);
    printf("Max: %d\n", min_max.max);
    printf("Elapsed time: %fms\n", elapsed_time);
    fflush(NULL);
    return 0;
}