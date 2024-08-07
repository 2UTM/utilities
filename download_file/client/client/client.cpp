﻿#include "client.h"

int sock;
std::string packet;
std::string infoUpdate;
std::string nameFolderUpdate;
std::string nameFileUpdate;

// Инициализация сокета
int initialization()
{
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
    {
        printf("Ошибка инициализация сокета.\n");
        WSACleanup();
        return 1;
    }
    return 0;
}

// подключение к серверу
int connectToServer(std::string ip, int port)
{
    int s;
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
        printf("Ошибка создания сокета.\n");
        return 1;
    }
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr(ip.c_str());
    sa.sin_port = htons(port);
    int socklen = sizeof(sa);
    if (connect(s, (struct sockaddr*)&sa, socklen))
    {
        printf("Ошибка соединения с сервером.\n");
        return 1;
    }
    
    return s;
}

// прочитать сообщение из сокета
int RecvPacket(std::string& packet)
{
    // Читаем размер, первые 4 символа
    std::vector<char> tmpSize(4);
    if (RecvAll(sock, &tmpSize.front(), 4))
    {
        printf("Ошибка recv: %d\n", WSAGetLastError());
        return 1;
    }
    std::string sSize(tmpSize.begin(), tmpSize.end());

    // Читаем остальное
    int iSize = atoi(sSize.c_str());
    std::vector<char> tmp(iSize);
    if (RecvAll(sock, &tmp.front(), iSize))
    {
        printf("Ошибка recv: %d\n", WSAGetLastError());
        return 1;
    }
    std::string result(tmp.begin(), tmp.end());
    packet += result;

    return 0;
}

// прочитать сообщение из сокета про обновление
int RecvPacketUpdate(std::string& nameFile, std::string& sizeFile, long long& seek)
{
    // Читаем размер, первые 9 символов
    std::vector<char> tmpSize(9);
    if (RecvAll(sock, &tmpSize.front(), 9))
    {
        printf("Ошибка recv: %d\n", WSAGetLastError());
        return 1;
    }
    std::string sSize(tmpSize.begin(), tmpSize.end());

    // Читаем имя файла, далее 256 символов
    std::vector<char> tmpNameFile(256);
    if (RecvAll(sock, &tmpNameFile.front(), 256))
    {
        printf("Ошибка recv: %d\n", WSAGetLastError());
        return 1;
    }
    std::string sNameFile(tmpNameFile.begin(), tmpNameFile.end());

    // Убираем знаки вопроса и точку с запятой из имени файла
    boost::replace_all(sNameFile, "?", "");
    boost::replace_all(sNameFile, ";", "");

    nameFile += sNameFile;

    // Читаем размер файла, далее 12 символов
    std::vector<char> tmpSizeFile(12);
    if (RecvAll(sock, &tmpSizeFile.front(), 12))
    {
        printf("Ошибка recv: %d\n", WSAGetLastError());
        return 1;
    }
    std::string sSizeFile(tmpSizeFile.begin(), tmpSizeFile.end());
    sizeFile += sSizeFile;

    // Читаем остальное
    seek = atoi(sSize.c_str());
    char* packet = new char[seek];
    if (RecvAll(sock, packet, seek))
    {
        printf("Ошибка recv: %d\n", WSAGetLastError());
        return 1;
    }

    // Пишем в файл
    int result = writeFile(nameFolderUpdate, nameFile, packet, sizeFile, seek);
    if (result == 1)
    {
        printf("Ошибка writeFile %d \n", WSAGetLastError());
        return 1;
    }
    if (result == 2)
    {
        return 2;
    }
    delete[] packet;

    return 0;
}

// отправить сообщение в сокет
int SendPacket(std::string message)
{
    // Формируем сообщение, первые 4 символа - размер
    std::string sizePacket = std::to_string(strlen(message.c_str()));
    while (true)
    {
        if (strlen(sizePacket.c_str()) < 4)
        {
            sizePacket.insert(sizePacket.begin(), '0');
            continue;
        }
        break;
    }
    std::string resultPacket = sizePacket;
    resultPacket += message;

    if (SendAll(sock, (char*)resultPacket.c_str(), strlen(resultPacket.c_str())))
    {
        printf("Ошибка send: %d\n", WSAGetLastError());
        return 1;
    }
    return 0;
}

// работа соединения
int doWork(int sock)
{
    int resultSelect = 0;
    FD_SET readfds;
    FD_SET writefds;
    FD_SET erfds;

    struct timeval tmo;
    tmo.tv_sec = 3;
    tmo.tv_usec = 0;

    printf("Подключились\n");
    while (true)
    {
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_ZERO(&erfds);
        FD_SET(sock, &readfds);
        FD_SET(sock, &writefds);
        FD_SET(sock, &erfds);

        packet = "";

        resultSelect = select(NULL, &readfds, &writefds, &erfds, &tmo);

        if (resultSelect == 0)
        {
            //Нет данных
            continue;
        }

        if (resultSelect == SOCKET_ERROR)
        {
            printf("select() failed: %d \n", WSAGetLastError());
            break;
        }

        if (FD_ISSET(sock, &erfds)) // ошибки
        {
            printf("Socket error: %d \n", WSAGetLastError());
            break;
        }
        if (FD_ISSET(sock, &readfds)) // получение
        {
            
        }
        if (FD_ISSET(sock, &writefds)) // отправка
        {
            // Проверка на недокаченный файлы
            int result = searchTempFileUpdate(nameFolderUpdate);

            if (result == 0)
            {
                ///////////////////////////////////////////////////////////////////////
                // Запрос обновления
                // 
                // Отправка данных
                if (SendPacket("0;0"))
                {
                    printf("Ошибка sendPacket %d \n", WSAGetLastError());
                    break;
                }

                std::string nameFile, sizeFile;
                long long seek = 0;
                packet.clear();
                
                if (RecvPacketUpdate(nameFile, sizeFile, seek))
                {
                    printf("Ошибка RecvPacketUpdate %d \n", WSAGetLastError());
                    break;
                }

                continue;
                ///////////////////////////////////////////////////////////////////////
            }
            else
            {
                ///////////////////////////////////////////////////////////////////////
                // Запрос докачки файла обновления
                // 
                // Подготовка сообщения для отправки
                // Получаем размер файла темп
                long long sizeTempFile = 0;
                if (getSizeFile(nameFolderUpdate, nameFileUpdate + ".temp", sizeTempFile))
                {
                    printf("Ошибка getSizeFile 8\n");
                    break;
                }
                std::string mes = nameFileUpdate + ";" + std::to_string(sizeTempFile);

                // Отправляем инфу о недокаченном файле на сервер
                if (SendPacket(mes))
                {
                    printf("Ошибка SendPacket %d \n", WSAGetLastError());
                    break;
                }

                std::string nameFile, sizeFile;
                long long seek = 0;
                packet.clear();
                int result = RecvPacketUpdate(nameFile, sizeFile, seek);
                if (result == 1)
                {
                    printf("Ошибка RecvPacketUpdate %d \n", WSAGetLastError());
                    break;
                }
                if (result == 2)
                {
                    printf("Скачивание завершено!\n");
                    break;
                }

                continue;
                ///////////////////////////////////////////////////////////////////////
            }
        }
        Sleep(50);
    }
    closesocket(sock);
    printf("Разрыв соединения\n\n");
    return 0;
}

// Поиск temp файлов в обновлении
int searchTempFileUpdate(std::string exePath)
{
    std::vector<std::string> files;

    std::string extension = ".temp";
    std::experimental::filesystem::directory_iterator iterator(exePath);
    for (; iterator != std::experimental::filesystem::end(iterator); iterator++)
    {
        if (iterator->path().extension() == extension)
        {
            files.push_back(iterator->path().filename().string());
        }
    }

    if (!files.empty())
    {
        return 1;
    }

    return 0;
}

// Проверяем размер файла недокаченного обновления
int getSizeFile(std::string exePath, std::string nameFile, long long& sizeFile)
{
    // Исходный файл
    FILE* fin;

    #pragma warning(suppress : 4996) // fopen deprecated
    if (!(fin = fopen((exePath + nameFile).c_str(), "r")) == NULL)
    {
        // Перемещаем указатель на конец исходного файла
        _fseeki64(fin, 0, SEEK_END);

        // Получаем размер исходного файла
        sizeFile = _ftelli64(fin);

        fclose(fin);
    }
    else
    {
        return 1;
    }

    return 0;
}

// Запись файла
int writeFile(std::string exePath, std::string nameFile, char* buffer, std::string sizeFile, long long seek)
{
    // Файл результат
    FILE* fout;

    // Убираем temp из имени файла
    boost::replace_all(nameFile, ".temp", "");

    #pragma warning(suppress : 4996) // fopen deprecated
    if (!(fout = fopen((exePath + nameFile + ".temp").c_str(), "ab")) == NULL)
    {
        // запись по байтам seek
        fwrite(buffer, sizeof(char), seek, fout);
        fclose(fout);
    }
    else
    {
        return 1;
    }

    // Проверяем размер файла, если все скачали, переименовываем из temp
    long long sizeTempFile = 0;
    if (getSizeFile(exePath, nameFile + ".temp", sizeTempFile))
    {
        return 1;
    }
    if (sizeTempFile == atoll(sizeFile.c_str()))
    {
        // переименовываем файл
        if (MoveFileA((exePath + nameFile + ".temp").c_str(), (exePath + nameFile).c_str()) == 0)
        {
            return 1;
        }
        return 2;
    }

    return 0;
}

// Циклическое чтение из сокета
int RecvAll(SOCKET sock, char* buffer, int size)
{
    while (size > 0)
    {
        int RecvSize = recv(sock, buffer, size, 0);
        if (RecvSize <= 0)
        {
            return 1;
        }
        size -= RecvSize;
        buffer += RecvSize;
    }
    return 0;
}

// Циклическая запись в сокет
int SendAll(SOCKET sock, char* buffer, int size)
{
    while (size > 0)
    {
        int SendSize = send(sock, buffer, size, 0);
        if (SendSize <= 0)
        {
            return 1;
        }
        size -= SendSize;
        buffer += SendSize;
    }
    return 0;
}

int main()
{
    setlocale(LC_ALL, "ru-RU");

    if (initialization())
    {
        return 1;
    }

    std::string ip;
    int port;
    printf("Введите IP адрес сервера: ");
    std::cin >> ip;
    printf("Введите порт сервера: ");
    std::cin >> port;
    printf("Введите путь до папки скачивания без пробелов (слеш в конце обязателен!): ");
    std::cin >> nameFolderUpdate;
    printf("Введите имя файла без пробелов (должно совпадать с файлом на сервере!): ");
    std::cin >> nameFileUpdate;
    sock = connectToServer(ip, port);
    if (sock == 1)
    {
        return 1;
    }

    doWork(sock);

    system("pause");

    return 0;
}
