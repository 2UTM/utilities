#include "stdio.h"
#include <locale.h>
#include "windows.h">
#include <iostream>

#define BLOCK_SIZE 9000


int main()
{
    setlocale(LC_ALL, "ru-RU");

    // Файл чтения
    FILE* fin;
    // Файл записи
    FILE* fout;

    char SrcFileName[MAX_PATH];
    char DstFileName[MAX_PATH];
    printf("Введите путь до исходного файла без пробелов: ");
    #pragma warning(suppress : 4996) // scanf deprecated
    scanf("%s", &SrcFileName);
    printf("Введите путь до файла назначения без пробелов: ");
    #pragma warning(suppress : 4996) // scanf deprecated
    scanf("%s", &DstFileName);

    char buffer[BLOCK_SIZE];
    int readed = 0;
    int seek = 0;

    printf("Начали копирование\n\n");

    while (true)
    {
        #pragma warning(suppress : 4996) // fopen deprecated
        if (!(fin = fopen(SrcFileName, "rb")) == NULL)
        {
            _fseeki64(fin, seek, SEEK_SET);

            readed = fread(buffer, sizeof(char), BLOCK_SIZE, fin);
            if (readed == 0)
            {
                break;
            }
            printf("Считали %d байт\n", readed);

            fclose(fin);
        }

        #pragma warning(suppress : 4996) // fopen deprecated
        if (!(fout = fopen(DstFileName, "ab")) == NULL)
        {
            int writed = fwrite(buffer, sizeof(char), readed, fout);
            if (writed == 0)
            {
                break;
            }
            printf("Записали %d байт\n", writed);

            fclose(fout);
        }

        seek += BLOCK_SIZE;
    }

    printf("Закончили копирование\n\n");
    getchar();

    return 0;
}