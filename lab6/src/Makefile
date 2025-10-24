# Makefile для ЛР6 - распределенное вычисление факториала
# Простая версия с комментариями

# Компилятор
CC = gcc

# Флаги компиляции: -Wall для включения предупреждений
CFLAGS = -Wall

# Флаги линковки: -lpthread для работы с потоками
LDFLAGS = -lpthread

# Цель по умолчанию - собирает и клиент и сервер
all: client server

# Сборка клиента: читает серверы из файла, распределяет работу
client: client.c
	$(CC) $(CFLAGS) -o client client.c $(LDFLAGS)

# Сборка сервера: вычисляет факториал для своего диапазона
server: server.c
	$(CC) $(CFLAGS) -o server server.c $(LDFLAGS)

# Очистка: удаляет скомпилированные программы
clean:
	rm -f client server