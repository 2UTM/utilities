#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING // для #include <experimental/filesystem>
#define _CRT_SECURE_NO_WARNINGS

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <stdio.h>
#include <string>
#include <vector>
//#include <filesystem> // C++17
#include <experimental/filesystem> // для с++14 и ниже
#include <winsock2.h>
#include <boost/algorithm/string/replace.hpp>
#include <iostream>
#include <cstdio>

#pragma comment(lib,"ws2_32.lib") //Библиотека Winsock

int initialization(); // инициалзация сокета
int connectToServer(std::string ip, int port); // подключение к серверу
int RecvPacket(std::string& packet); // прочитать сообщение из сокета
int RecvPacketUpdate(std::string& nameFile, std::string& sizeFile, long long& seek); // прочитать сообщение из сокета про обновление
int SendPacket(std::string message); // отправить сообщение в сокет
int doWork(int sock); // работа соединения

// Поиск temp файлов в обновлении
int searchTempFileUpdate(std::string exePath);

// Проверяем размер файла недокаченного обновления
int getSizeFile(std::string exePath, std::string nameFile, long long& sizeFile);

// Запись файла
int writeFile(std::string exePath, std::string nameFile, char* buffer, std::string sizeFile, long long seek);

// Циклический чтение из сокета
int RecvAll(SSL* ssl, char* buffer, int size);

// Циклическая запись в сокет
int SendAll(SSL* ssl, char* buffer, int size);
