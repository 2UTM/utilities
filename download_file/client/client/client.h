#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS


#include <stdio.h>
#include <string>
#include <vector>
#include <winsock2.h>
#include <boost/algorithm/string/replace.hpp>

#pragma comment(lib,"ws2_32.lib") //Библиотека Winsock

int initialization(); // инициалзация сокета
int connectToServer(std::string ip, int port); // подключение к серверу
int RecvPacket(std::string& packet); // прочитать сообщение из сокета
int RecvPacketUpdate(std::string& nameFile, std::string& sizeFile, int& seek); // прочитать сообщение из сокета про обновление
int SendPacket(std::string message); // отправить сообщение в сокет
int doWork(int sock); // работа соединения

// Поиск temp файлов в обновлении
int searchTempFileUpdate();

// Проверяем размер файла недокаченного обновления
int getSizeFile(std::string nameFile, long long& sizeFile);

// Запись файла
int writeFile(std::string nameFile, char* buffer, std::string sizeFile, int seek);

// Циклический чтение из сокета
int RecvAll(SOCKET sock, char* buffer, int size);
