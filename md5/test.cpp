#include <fstream>
#include <stdio.h>
#include "string"

#define BLOCK_SIZE 512 // размер блоков, по которым читаем файл (в байтах)

int main()
{
    // Исходный файл
    FILE* fin;
    // Файл результат
    FILE* fout;

    char buffer[BLOCK_SIZE];
    #pragma warning(suppress : 4996) // fopen deprecated
    if (!(fin = fopen("c:\\Install\\Розница_2.3.13.23_чистая.dt", "rb")) == NULL &&
        #pragma warning(suppress : 4996) // fopen deprecated
        !(fout = fopen("c:\\Install\\Розница_2.3.13.23_чистая_new.dt", "wb")) == NULL)
    {

        // Перемещаем курсор на конец исходного файла
        _fseeki64(fin, 0, SEEK_END);

        // Получаем размер исходного файла
        long long fileSize = _ftelli64(fin);

        // Перемещаем курсор на начало исходного файла
        _fseeki64(fin, 0, SEEK_SET);

        // Копирование
        // Чтение и запись по байтам "BLOCK_SIZE"
        for (size_t i = 0; i <= fileSize / BLOCK_SIZE; ++i)
        {
            int readed = fread(buffer, sizeof(char), BLOCK_SIZE, fin);
            fwrite(buffer, sizeof(char), readed, fout);
        }
        fclose(fin);
        fclose(fout);

    }
}
