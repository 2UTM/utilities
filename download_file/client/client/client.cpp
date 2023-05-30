#pragma once
#include "client.h"

int sock;
std::string packet;
std::string infoUpdate;

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
    if (recv(sock, &tmpSize.front(), 4, 0) <= 0)
    {
        printf("Ошибка recv: %d\n", WSAGetLastError());
        return 1;
    }
    std::string sSize(tmpSize.begin(), tmpSize.end());

    // Читаем остальное
    int iSize = atoi(sSize.c_str());
    std::vector<char> tmp(iSize);
    if (recv(sock, &tmp.front(), iSize, 0) <= 0)
    {
        printf("Ошибка recv: %d\n", WSAGetLastError());
        return 1;
    }
    std::string result(tmp.begin(), tmp.end());
    packet += result;

    return 0;
}

// прочитать сообщение из сокета про обновление
int RecvPacketUpdate(std::string& nameFile, std::string& sizeFile, int& seek)
{
    // Читаем реальный размер буффера, первые 4 символа
    std::vector<char> tmpSizePacket(4);
    if (recv(sock, &tmpSizePacket.front(), 4, 0) <= 0)
    {
        printf("Ошибка recv: %d\n", WSAGetLastError());
        return 1;
    }
    std::string sSizePacket(tmpSizePacket.begin(), tmpSizePacket.end());

    // Читаем размер, далее 4 символа
    std::vector<char> tmpSize(4);
    if (recv(sock, &tmpSize.front(), 4, 0) <= 0)
    {
        printf("Ошибка recv: %d\n", WSAGetLastError());
        return 1;
    }
    std::string sSize(tmpSize.begin(), tmpSize.end());

    // Читаем имя файла, далее 256 символов
    std::vector<char> tmpNameFile(256);
    if (recv(sock, &tmpNameFile.front(), 256, 0) <= 0)
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
    if (recv(sock, &tmpSizeFile.front(), 12, 0) <= 0)
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
    if (writeFile(nameFile, packet, sizeFile, seek))
    {
        printf("Ошибка writeFile %d \n", WSAGetLastError());
        return 1;
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

    int len = send(sock, resultPacket.c_str(), strlen(resultPacket.c_str()), 0);
    if (len <= 0)
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
            //packet.clear();
            //if (RecvPacket(packet) == 0 || packet == "")
            //{
            //    continue;
            //}
        }
        if (FD_ISSET(sock, &writefds)) // отправка
        {
            // Проверка на недокаченный файлы
            int result = searchTempFileUpdate();

            if (result == 3)
            {
                printf("Скачка не требуеться!\n");
                break;
            }
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
                int seek = 0;
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
                if (getSizeFile("DrvFR_5.16_886_x32.exe.temp", sizeTempFile))
                {
                    printf("Ошибка getSizeFile 8\n");
                    break;
                }
                std::string mes = "DrvFR_5.16_886_x32.exe.temp;" + std::to_string(sizeTempFile);

                // Отправляем инфу о недокаченном файле на сервер
                if (SendPacket(mes))
                {
                    printf("Ошибка SendPacket %d \n", WSAGetLastError());
                    break;
                }

                std::string nameFile, sizeFile;
                int seek = 0;
                packet.clear();
                if (RecvPacketUpdate(nameFile, sizeFile, seek))
                {
                    printf("Ошибка RecvPacketUpdate %d \n", WSAGetLastError());
                    break;
                }

                continue;
                ///////////////////////////////////////////////////////////////////////
            }
        }
        Sleep(100);
    }
    closesocket(sock);
    printf("Разрыв соединения\n\n");
    return 0;
}

// Поиск temp файлов в обновлении
int searchTempFileUpdate()
{
    std::string exePath = "d:\\test_download\\";
    CreateDirectoryA(exePath.c_str(), 0);
    exePath = exePath + "*"; // добавляем шаблон поиска

    // Получаем список файлов
    WIN32_FIND_DATAA FindFileData;
    HANDLE hf;
    hf = FindFirstFileA(exePath.c_str(), &FindFileData);
    if (hf == INVALID_HANDLE_VALUE)
    {
        return 1;
    }

    exePath = exePath.erase(exePath.size() - 1);
    do
    {
        // Если это не ини и не лог файл, добавляем в результирующий вектор
        std::string tmp(FindFileData.cFileName);
        if (tmp.find(".temp") != std::string::npos)
        {
            return 2;
        }
        if (tmp.find("DrvFR_5.16_886_x32.exe") != std::string::npos)
        {
            return 3;
        }
    } while (FindNextFileA(hf, &FindFileData) != 0);

    FindClose(hf);

    return 0;
}

// Проверяем размер файла недокаченного обновления
int getSizeFile(std::string nameFile, long long& sizeFile)
{
    
    std::string exePath = "d:\\test_download\\";

    // Исходный файл
    FILE* fin;

    #pragma warning(suppress : 4996) // fopen deprecated
    if (!(fin = fopen((exePath + nameFile).c_str(), "r")) == NULL)
    {
        // Перемещаем указатель на конец темпового файла
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
int writeFile(std::string nameFile, char* buffer, std::string sizeFile, int seek)
{
    std::string exePathUpdate = "d:\\test_download\\";

    // Файл результат
    FILE* fout;

    // Убираем temp из имени файла
    boost::replace_all(nameFile, ".temp", "");

    // Добавляем к имени файла .temp
    #pragma warning(suppress : 4996) // fopen deprecated
    if (!(fout = fopen((exePathUpdate + nameFile + ".temp").c_str(), "ab")) == NULL)
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
    if (getSizeFile(nameFile + ".temp", sizeTempFile))
    {
        return 1;
    }
    if (sizeTempFile == atoll(sizeFile.c_str()))
    {
        // переименовываем файл
        if (MoveFileA((exePathUpdate + nameFile + ".temp").c_str(), (exePathUpdate + nameFile).c_str()) == 0)
        {
            return 1;
        }
    }

    return 0;
}

// Циклическое чтение из сокета
int RecvAll(SOCKET sock, char* buffer, int size)
{
    while (size > 0)
    {
        int RecvSize = recv(sock, buffer, size, 0);
        if (SOCKET_ERROR == RecvSize)
        {
            return 1;
        }
        size = size - RecvSize;
        buffer += RecvSize;
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

    sock = connectToServer("127.0.0.1", 44444);
    if (sock == 1)
    {
        return 1;
    }

    doWork(sock);

    return 0;
}
