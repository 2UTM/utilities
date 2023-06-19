#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <winsock2.h>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <fstream> 
#include <iostream>
#include <cstdlib>
#pragma comment(lib,"ws2_32.lib") //Библиотека Winsock


// Инициализация сокета
int initialization(int port);

// Поток для соединения с клиентами
void clientWork(SSL* ssl, int sock);

// Прием сообщений через сокет
int recvPacket(SSL* ssl, std::string& message);

// Отправка обновления
int sendPacketUpdate(SSL* ssl, char* packet, std::string nameFile, std::string sizeFile, std::string bufferSize);

// Чтение файла
int readFile(std::string nameFile, long long seek, long long& sizeFile, char* buffer, int& bufferSize);

// Проверяем размер файла недокаченного обновления
int getSizeFile(std::string nameFile, long long& sizeFile);

// Циклический чтение из сокета
int RecvAll(SSL* ssl, char* buffer, int size);

// Циклическая запись в сокет
int SendAll(SSL* ssl, char* buffer, int size);

//Контекст SSL
SSL_CTX* createContext();

// Конфигурация контекста
void configureContext(SSL_CTX* ctx);

//обратная функция для ввода пароля от закрытого ключа
int pemPasswd(char* buf, int size, int rwflag, void* password);