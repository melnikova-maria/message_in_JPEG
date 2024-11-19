#include "JPEG.h"

int main()
    {
        data_word info_word_before;    //пустая структура слова
        data_word info_word_after;     //заполненная структура слова

        //запрос данных у пользователя
        info_word_after = data_fun (info_word_before);

        //открытие файла и обработка его заголовка
        read_and_write (info_word_after);

        return 0;
    };