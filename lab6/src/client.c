#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "lib.h"  // Подключаем нашу библиотеку

struct Server {
  char ip[255];
  int port;
};

struct ThreadData {
  struct Server server;
  uint64_t begin;
  uint64_t end;
  uint64_t mod;
  uint64_t result;
};

// Уже не нужно - перенесено в lib.c
// uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod) { ... }

bool ConvertStringToUI64(const char *str, uint64_t *val) {
  char *end = NULL;
  unsigned long long i = strtoull(str, &end, 10);
  if (errno == ERANGE) {
    fprintf(stderr, "Out of uint64_t range: %s\n", str);
    return false;
  }
  if (errno != 0)
    return false;
  *val = i;
  return true;
}

// Функция для работы с одним сервером в отдельном потоке
void* ConnectToServer(void* thread_data) {
  struct ThreadData* data = (struct ThreadData*)thread_data;
  
  struct hostent *hostname = gethostbyname(data->server.ip);
  if (hostname == NULL) {
    fprintf(stderr, "gethostbyname failed with %s\n", data->server.ip);
    data->result = 0;
    return NULL;
  }

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(data->server.port);
  server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);

  int sck = socket(AF_INET, SOCK_STREAM, 0);
  if (sck < 0) {
    fprintf(stderr, "Socket creation failed!\n");
    data->result = 0;
    return NULL;
  }

  if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
    fprintf(stderr, "Connection to %s:%d failed\n", data->server.ip, data->server.port);
    close(sck);
    data->result = 0;
    return NULL;
  }

  // Подготавливаем задачу для сервера
  char task[sizeof(uint64_t) * 3];
  memcpy(task, &data->begin, sizeof(uint64_t));
  memcpy(task + sizeof(uint64_t), &data->end, sizeof(uint64_t));
  memcpy(task + 2 * sizeof(uint64_t), &data->mod, sizeof(uint64_t));

  if (send(sck, task, sizeof(task), 0) < 0) {
    fprintf(stderr, "Send to %s:%d failed\n", data->server.ip, data->server.port);
    close(sck);
    data->result = 0;
    return NULL;
  }

  char response[sizeof(uint64_t)];
  if (recv(sck, response, sizeof(response), 0) < 0) {
    fprintf(stderr, "Receive from %s:%d failed\n", data->server.ip, data->server.port);
    close(sck);
    data->result = 0;
    return NULL;
  }

  memcpy(&data->result, response, sizeof(uint64_t));
  printf("Server %s:%d computed factorial [%lu-%lu] mod %lu = %lu\n", 
         data->server.ip, data->server.port, data->begin, data->end, data->mod, data->result);

  close(sck);
  return NULL;
}

// Функция для чтения серверов из файла
int ReadServersFromFile(const char* filename, struct Server** servers) {
  FILE* file = fopen(filename, "r");
  if (!file) {
    perror("Failed to open servers file");
    return 0;
  }

  int capacity = 10;
  int count = 0;
  *servers = malloc(sizeof(struct Server) * capacity);

  char line[255];
  while (fgets(line, sizeof(line), file)) {
    // Удаляем символ новой строки
    line[strcspn(line, "\n")] = 0;
    
    // Парсим IP:port
    char* colon = strchr(line, ':');
    if (colon) {
      *colon = '\0';
      strncpy((*servers)[count].ip, line, sizeof((*servers)[count].ip) - 1);
      (*servers)[count].port = atoi(colon + 1);
      count++;
      
      // Увеличиваем массив при необходимости
      if (count >= capacity) {
        capacity *= 2;
        *servers = realloc(*servers, sizeof(struct Server) * capacity);
      }
    }
  }

  fclose(file);
  return count;
}

int main(int argc, char **argv) {
  uint64_t k = -1;
  uint64_t mod = -1;
  char servers_file[255] = {'\0'};

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"servers", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        ConvertStringToUI64(optarg, &k);
        break;
      case 1:
        ConvertStringToUI64(optarg, &mod);
        break;
      case 2:
        strncpy(servers_file, optarg, sizeof(servers_file) - 1);
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;

    case '?':
      printf("Arguments error\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (k == -1 || mod == -1 || !strlen(servers_file)) {
    fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
            argv[0]);
    return 1;
  }

  // Читаем серверы из файла
  struct Server* servers = NULL;
  int servers_num = ReadServersFromFile(servers_file, &servers);
  
  if (servers_num == 0) {
    fprintf(stderr, "No servers found in file %s\n", servers_file);
    return 1;
  }

  printf("Found %d servers\n", servers_num);

  // Распределяем работу между серверами
  pthread_t threads[servers_num];
  struct ThreadData thread_data[servers_num];

  uint64_t numbers_per_server = k / servers_num;
  uint64_t remainder = k % servers_num;
  uint64_t current_start = 1;

  for (int i = 0; i < servers_num; i++) {
    thread_data[i].server = servers[i];
    thread_data[i].begin = current_start;
    thread_data[i].end = current_start + numbers_per_server - 1;
    
    // Распределяем остаток
    if (remainder > 0) {
      thread_data[i].end++;
      remainder--;
    }
    
    thread_data[i].mod = mod;
    current_start = thread_data[i].end + 1;

    printf("Server %d (%s:%d): numbers %lu to %lu\n", 
           i, servers[i].ip, servers[i].port, thread_data[i].begin, thread_data[i].end);

    if (pthread_create(&threads[i], NULL, ConnectToServer, (void *)&thread_data[i])) {
      fprintf(stderr, "Error: pthread_create failed!\n");
      free(servers);
      return 1;
    }
  }

  // Ждем завершения всех потоков
  uint64_t total_result = 1;
  for (int i = 0; i < servers_num; i++) {
    pthread_join(threads[i], NULL);
    total_result = MultModulo(total_result, thread_data[i].result, mod);  // Используем функцию из библиотеки
  }

  printf("\nFinal result: %lu! mod %lu = %lu\n", k, mod, total_result);

  free(servers);
  return 0;
}