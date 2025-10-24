# Makefile для ЛР6 - распределенное вычисление факториала
# С поддержкой библиотеки общих функций

# Компилятор
CC = gcc

# Флаги компиляции: -Wall для включения предупреждений
CFLAGS = -Wall

# Флаги линковки: -lpthread для работы с потоками
LDFLAGS = -lpthread

# Объектные файлы
LIB_OBJ = lib.o
CLIENT_OBJ = client.o
SERVER_OBJ = server.o

# Цель по умолчанию - собирает и клиент и сервер
all: client server

# Сборка библиотеки
$(LIB_OBJ): lib.c lib.h
	$(CC) $(CFLAGS) -c lib.c -o $(LIB_OBJ)

# Сборка клиента с использованием библиотеки
client: $(CLIENT_OBJ) $(LIB_OBJ)
	$(CC) $(CFLAGS) -o client $(CLIENT_OBJ) $(LIB_OBJ) $(LDFLAGS)

# Сборка сервера с использованием библиотеки
server: $(SERVER_OBJ) $(LIB_OBJ)
	$(CC) $(CFLAGS) -o server $(SERVER_OBJ) $(LIB_OBJ) $(LDFLAGS)

# Компиляция клиента
$(CLIENT_OBJ): client.c lib.h
	$(CC) $(CFLAGS) -c client.c -o $(CLIENT_OBJ)

# Компиляция сервера
$(SERVER_OBJ): server.c lib.h
	$(CC) $(CFLAGS) -c server.c -o $(SERVER_OBJ)

# Очистка: удаляет скомпилированные программы и объектные файлы
clean:
	rm -f client server *.o

# Псевдоцель для обозначения файлов, которые не нужно удалять
.PHONY: all clean