CC=gcc
CFLAGS=-I.

# Добавляем zombie_demo в цель all
all : sequential_min_max parallel_min_max run_sequential zombie_demo

sequential_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o sequential_min_max find_min_max.o utils.o sequential_min_max.c $(CFLAGS)

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

# Добавляем цель для run_sequential
run_sequential : sequential_min_max
	$(CC) -o run_sequential run_sequential.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

# программа для демонстрации зомби-процессов
zombie_demo :
	$(CC) -o zombie_demo zombie_demo.c $(CFLAGS)

# и обновляем clear
clean :
	rm utils.o find_min_max.o sequential_min_max parallel_min_max run_sequential zombie_demo