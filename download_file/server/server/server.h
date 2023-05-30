#pragma once
#include <winsock2.h>
#include <thread>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <fstream> 
#pragma comment(lib,"ws2_32.lib") //Библиотека Winsock


#define BLOCK_SIZE 1500

// Инициализация сокета
int initialization(int port);

// Поток для соединения с клиентами
void clientWork(int sock);

// Прием сообщений через сокет
int recvPacket(SOCKET s, std::string& message);

// Отправка обновления
int sendPacketUpdate(SOCKET s, char* packet, std::string nameFile, std::string sizeFile, std::string bufferSize);

// Чтение файла
int readFile(std::string nameFile, int seek, long long& sizeFile, char* buffer, int& bufferSize);

// Проверяем размер файла недокаченного обновления
int getSizeFile(std::string nameFile, long long& sizeFile);

// Циклическая запись в сокет
int SendAll(SOCKET sock, char* buffer, int size);
