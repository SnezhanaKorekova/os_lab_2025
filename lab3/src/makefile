CC = gcc
CFLAGS = -I.
LDFLAGS = -lpthread

# Цели по умолчанию
all: parallel_min_max process_memory parallel_sum

# Задание 1: Параллельный поиск min/max с таймаутом
parallel_min_max: utils.o find_min_max.o
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

# Задание 3: Изучение виртуальной памяти
process_memory:
	$(CC) -o process_memory process_memory.c $(CFLAGS)

# Задание 5: Параллельное суммирование
parallel_sum: utils.o sum.o
	$(CC) -o parallel_sum utils.o sum.o parallel_sum.c $(CFLAGS) $(LDFLAGS)

# Вспомогательные объектные файлы
utils.o: utils.c utils.h
	$(CC) -c utils.c -o utils.o $(CFLAGS)

find_min_max.o: find_min_max.c find_min_max.h utils.h
	$(CC) -c find_min_max.c -o find_min_max.o $(CFLAGS)

sum.o: sum.c sum.h
	$(CC) -c sum.c -o sum.o $(CFLAGS)

# Очистка
clean:
	rm -f parallel_min_max process_memory parallel_sum *.o

# Более тщательная очистка
distclean: clean
	rm -f *~ *.bak

# Справка
help:
	@echo "Доступные цели:"
	@echo "  all          - собрать все программы"
	@echo "  parallel_sum - собрать программу параллельного суммирования"
	@echo "  clean        - удалить скомпилированные файлы"
	@echo "  help         - показать эту справку"

.PHONY: all clean distclean help