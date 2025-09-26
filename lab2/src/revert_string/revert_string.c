#include "revert_string.h"
#include <string.h>

void RevertString(char *str)
{
    int length = strlen(str);
    int i = 0;
    int j = length - 1;
    
    // Меняем симметричные символы местами до середины строки
    while (i < j)
    {
        // Обмен значений через временную переменную
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        
        i++;
        j--;
    }
}