﻿#include <fstream>
#pragma warning(disable : 4996) // fopen deprecated

#define BLOCK_SIZE 512 // размер блоков, по которым читаем файл

int main()
{
    // Исходный файл
    FILE* fin;
    // Файл результат
    FILE* fout;

    unsigned char buffer[BLOCK_SIZE];
    if (!(fin = fopen("d:\\labi_C++\\InfoShops_dll\\client\\db.zip", "rb")) == NULL &&
        !(fout = fopen("d:\\labi_C++\\InfoShops_dll\\client\\db_new.zip", "wb")) == NULL)
    {

        // Перемещаем курсор на конец исходного файла
        _fseeki64(fin, 0, SEEK_END);

        // Получаем размер исходного файла
        long long m_file_size = _ftelli64(fin);

        // Перемещаем курсор на начало исходного файла
        _fseeki64(fin, 0, SEEK_SET);

        // Копирование
        // Чтение и запись по байтам "BLOCK_SIZE"
        for (size_t i = 0; i <= m_file_size / BLOCK_SIZE; ++i)
        {
            int readed = fread(buffer, sizeof(unsigned char), BLOCK_SIZE, fin);
            fwrite(buffer, sizeof(unsigned char), readed, fout);
        }
        fclose(fin);
        fclose(fout);

    }
}
