# Компилятор и флаги
CC = gcc
CFLAGS = -I.
CFLAGS_WARN = -Wall -Wextra -Wpedantic

# Цели по умолчанию
all: parallel_min_max process_memory

# Задание 1: Параллельный поиск min/max с таймаутом
parallel_min_max: utils.o find_min_max.o
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

# Задание 3: Изучение виртуальной памяти
process_memory:
	$(CC) -o process_memory process_memory.c $(CFLAGS)

# Вспомогательные объектные файлы
utils.o: utils.c utils.h
	$(CC) -c utils.c -o utils.o $(CFLAGS)

find_min_max.o: find_min_max.c find_min_max.h utils.h
	$(CC) -c find_min_max.c -o find_min_max.o $(CFLAGS)

# Очистка
clean:
	rm -f parallel_min_max process_memory *.o

# Более тщательная очистка
distclean: clean
	rm -f *~ *.bak

# Справка
help:
	@echo "Доступные цели:"
	@echo "  all          - собрать все программы (по умолчанию)"
	@echo "  parallel_min_max - собрать программу с таймаутом"
	@echo "  process_memory   - собрать программу изучения памяти"
	@echo "  clean        - удалить скомпилированные файлы"
	@echo "  distclean    - полная очистка"
	@echo "  help         - показать эту справку"

# Указываем, что эти цели не создают файлов
.PHONY: all clean distclean help