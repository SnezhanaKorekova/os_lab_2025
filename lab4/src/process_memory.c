/* Программа для отображения информации о адресах памяти процесса */
/* Адаптировано из Gray, J., program 1.4 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

/* Макроопределение */
#define SHW_ADR(ID, I) (printf("ID %s \t находится по виртуальному адресу: %8X\n", ID, &I))

extern int etext, edata, end; /* Глобальные переменные для памяти процесса
                                 etext - конец TEXT сегмента (код программы)
                                 edata - конец DATA сегмента (инициализированные данные)  
                                 end - конец BSS сегмента (неинициализированные данные) */

char *cptr = "This message is output by the function showit()\n"; /* Статическая строка - в DATA сегменте */
char buffer1[25]; /* Неинициализированный массив - в BSS сегменте */
int showit(); /* Прототип функции */

main() {
  int i = 0; /* Автоматическая переменная - в СТЕКЕ */

  /* Вывод информации о адресах */
  printf("\nАдрес etext (конец TEXT сегмента): %8X \n", &etext);
  printf("Адрес edata (конец DATA сегмента): %8X \n", &edata);
  printf("Адрес end   (конец BSS сегмента):  %8X \n", &end);

  SHW_ADR("main", main);    /* Функция main - в TEXT сегменте */
  SHW_ADR("showit", showit);/* Функция showit - в TEXT сегменте */
  SHW_ADR("cptr", cptr);    /* Указатель на строку - в DATA сегменте */
  SHW_ADR("buffer1", buffer1); /* Массив - в BSS сегменте */
  SHW_ADR("i", i);          /* Локальная переменная - в СТЕКЕ */
  
  strcpy(buffer1, "A demonstration\n");   /* Библиотечная функция */
  write(1, buffer1, strlen(buffer1) + 1); /* Системный вызов */
  showit(cptr);

} /* конец функции main */

/* Функция showit */
int showit(p) char *p;
{
  char *buffer2; /* Локальная переменная-указатель - в СТЕКЕ */
  SHW_ADR("buffer2", buffer2); /* Адрес указателя в стеке */
  
  if ((buffer2 = (char *)malloc((unsigned)(strlen(p) + 1))) != NULL) {
    printf("Выделена память в КУЧЕ по адресу: %X\n", buffer2);
    strcpy(buffer2, p);    /* копируем строку */
    printf("%s", buffer2); /* выводим строку */
    free(buffer2);         /* освобождаем память */
  } else {
    printf("Ошибка выделения памяти\n");
    exit(1);
  }
}