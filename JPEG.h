#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

typedef struct 
    {
        char* p;
        int l;
        uint8_t* w;
    } data_word;

//функция перевода слова в двоичное представление
uint8_t* string_to_binary (char* s, int len)
    {
        if (s == NULL)
            return 0;
            
        int i;
        int ii = 0;
        int kk = 0;

        char* binary_char = malloc (len + 1); //указатель на область памяти
        uint8_t* binary_int = malloc (len + 1);

        binary_char[0] = '\0';

        for(i = 0; i < len; ++i)
            {
                char ch = s[i];
                for(int j = 7; j >= 0; --j)
                    {
                        if(ch & (1 << j))
                            strcat(binary_char, "1");
                        else
                            strcat(binary_char, "0");
                    }
            }

        for(ii = 0, kk = 0; ii < len; ii++, kk++)
            {
                binary_int[kk] = binary_char[ii];
                binary_int[kk] -= 48;
            }

        return binary_int;
    }

char* vvod_data ()
    {
        int symbol;
        char symbol_char;
        char* data;

        data = (char*) malloc(sizeof(char));
        if (data != NULL)   //память под месторасположение исходного файла выделена
            {
                data[0] = '\0';
                //чтение одного байта из файла
                symbol = getchar();
                while (symbol != '\n')
                    {
                        //нажат любой символ, кроме Enter
                        data = (char*) realloc (data, sizeof(char) * (strlen(data) + 2));
                        symbol_char = symbol;
                        strncat(data, &symbol_char, 1);
                        symbol = getchar();
                    }
                if (data[0] == '\0')  //имя файла введено не было
                    data = NULL;
            }
        return data;
    }

//запрос данных у пользователя
data_word data_fun (data_word dd_fun)
    {
        int i = 0;
        // char* message;
        // char* path;

        // printf("word you want to encode: ");
        // message = vvod_data();
        
        // printf("\npath to the image you want to encode the word into: ");
        // path = vvod_data();

        char message[] = "hello";
        char* path = "black.jpg";

        int len_of_word = strlen (message) * 8;
        uint8_t* word = string_to_binary (message, len_of_word);

        dd_fun.p = path;
        dd_fun.l = len_of_word;
        dd_fun.w = word;
        
        return dd_fun;
    }

//чтение файла и запись в новый
void read_and_write (data_word info_word_after_fun)
    {
        srand(time(NULL));

        FILE *steam_read = fopen(info_word_after_fun.p, "r+b"); //открыли файл на чтение
        if (steam_read == NULL) //обработка ошибки, если файл не открывается
            {
                printf("cannot open file \n");
                exit (1);
            }

        fseek (steam_read, 0, SEEK_END);      //устанавливаем указатель на конец файл
        int file_size = ftell (steam_read);   //получаем значение указателя относительно начала
        fseek (steam_read, 0, SEEK_SET);      //SEEK_SET – смещение отсчитывается от начала файла
        

        uint8_t array[file_size];     //массив байт из файла
        uint8_t array_new[file_size + info_word_after_fun.l * 2 + 1]; //массив со вставленными байтами (+ info_word_after_fun.l * 2)
        size_t read;
        char str[9] = {0};

        read = fread(array, 1, file_size, steam_read); //считали весь файл в массив
        if (feof(steam_read))   //если конец файла
            {
                printf("unexpected end of file\n");
                printf("available: %s",str);
            }
        else if (ferror(steam_read))    //если ошибка
            perror("error while reading file\n");


        uint16_t position[info_word_after_fun.l];

        for (int w = 3; w < info_word_after_fun.l; w++) //массив случайных чисел
            {
                position[w] = 100 + (rand() % 1000);
            }

        uint8_t ff_da[2]; //размер сегмента FF DA
        size_t read_ff_da;
        unsigned int size_ff_da;

        int max_seg_i, max_seg_j; //позиция первого байта после всех сегментов (первый байт внутри сегмента FF DA)
        int i;

        for (i = 0; i < file_size; i++) //идем по элементам массива
            {
                array_new[i] = array[i];
                if (array[i] == 0xFF && array[i + 1] == 0xDA)
                    {
                        array_new[i + 1] = array[i + 1];

                        fseek(steam_read, i + 2, SEEK_SET);              //устанавливаем указатель байт размера сегмента
                        read_ff_da = fread(ff_da, 1, 2, steam_read);     //считываем размер сегмента
                        size_ff_da = ff_da[1] | (ff_da[0] << 8);    //так как размер сегмента состоит из
                                                                    //двух байт, то необходимо сначала считать
                                                                    //эти байта, а потом из соединить

                        array_new[i + 2] = array[i + 2];
                        array_new[i + 3] = array[i + 3];

                        fseek(steam_read, i + 2 + size_ff_da, SEEK_SET);  //сдвигаемся на размер сегмента,
                                                                          //тем самым позиция указателя файла становиться
                                                                          //на первый байт после сегмента

                        int ii = i + 4 + size_ff_da - 2;
                        int begin_seg = i + 4;
                        int end_seg = i + 14;
                        
                        for (int kk = 0; kk <= end_seg - begin_seg; kk++)
                            {
                                array_new[begin_seg + kk] = array[begin_seg + kk];
                                max_seg_i = begin_seg + kk;
                            }

                        i = max_seg_i;
                        max_seg_j = max_seg_i;

                        break;
                    }
            }

        uint16_t pos;
        int j;
        int c = 0;

        for (int k = 0; k < info_word_after_fun.l; k++) //цикл по случайным числам
            {
                pos = position[k];

                //цикл по байтам файла
                for (i = max_seg_i, j = max_seg_j; i < (file_size + info_word_after_fun.l * 2 + 1); i++, j++, max_seg_j++)
                    {
                        array_new[i] = array[j];
                        // printf("\n%d: %X %X", i, array_new[i], array[j]);
                        if (i == max_seg_i + pos && c != 1)
                            {
                                // printf("\n))))))))))))))))))))))))))))))k: %d", k);
                                array_new[i + 1] = 0xFF;
                                if (info_word_after_fun.w[k] == 1)
                                    array_new[i + 2] = 0xBF;
                                else
                                    array_new[i + 2] = 0xBE;

                                // printf("\n%d: %X %X", i + 1, array_new[i + 1], array_new[i + 2]);

                                max_seg_i += pos;
                                i = i + 3;
                                max_seg_i = i;
                                max_seg_j++;

                                if (k == 39)
                                {
                                    c = 1;
                                    continue;
                                }
                                
                                break;
                            }
                    }
            }

        FILE *steam_write = fopen("black_new.jpg", "w"); //открыли файл на запись
        if (steam_write == NULL) //обработка ошибки, если файл не открывается
            {
                printf("cannot open file to write\n");
                exit (1);
            }
        size_t count = fwrite(array_new, sizeof(array_new[0]), (sizeof(array_new) / sizeof(array_new[0])), steam_write);
        if (feof(steam_write))   //если конец файла
            {
                printf("unexpected end of file\n");
                printf("available: %s",str);
            }
        else if (ferror(steam_read))    //если ошибка
            perror("error while reading file\n");
    }

