#include "stdio.h"
#include <locale.h>

#define BLOCK_SIZE 9000


int main()
{
    setlocale(LC_ALL, "ru-RU");

    // Файл чтения
    FILE* fin;
    // Файл записи
    FILE* fout;

    char buffer[BLOCK_SIZE];
    int readed = 0;
    int seek = 0;

    printf("Начали копирование\n\n");

    for (;;)
    {
        #pragma warning(suppress : 4996) // fopen deprecated
        if (!(fin = fopen("D:\\DrvFR_5.16_886_x32.exe", "rb")) == NULL)
        {
            _fseeki64(fin, seek, SEEK_SET);

            readed = fread(buffer, sizeof(char), BLOCK_SIZE, fin);
            printf("Считали %d байт\n", readed);

            fclose(fin);
        }

        #pragma warning(suppress : 4996) // fopen deprecated
        if (!(fout = fopen("D:\\test_download\\DrvFR_5.16_886_x32.exe", "ab")) == NULL)
        {
            int writed = fwrite(buffer, sizeof(char), readed, fout);
            printf("Записали %d байт\n", writed);

            fclose(fout);
        }

        seek += BLOCK_SIZE;
    }

    printf("Закончили копирование\n\n");

    return 0;
}