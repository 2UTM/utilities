#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING // ��� #include <experimental/filesystem>
#define _CRT_SECURE_NO_WARNINGS

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <stdio.h>
#include <string>
#include <vector>
//#include <filesystem> // C++17
#include <experimental/filesystem> // ��� �++14 � ����
#include <winsock2.h>
#include <boost/algorithm/string/replace.hpp>
#include <iostream>
#include <cstdio>

#pragma comment(lib,"ws2_32.lib") //���������� Winsock

int initialization(); // ������������ ������
int connectToServer(std::string ip, int port); // ����������� � �������
int RecvPacket(std::string& packet); // ��������� ��������� �� ������
int RecvPacketUpdate(std::string& nameFile, std::string& sizeFile, long long& seek); // ��������� ��������� �� ������ ��� ����������
int SendPacket(std::string message); // ��������� ��������� � �����
int doWork(int sock); // ������ ����������

// ����� temp ������ � ����������
int searchTempFileUpdate(std::string exePath);

// ��������� ������ ����� ������������� ����������
int getSizeFile(std::string exePath, std::string nameFile, long long& sizeFile);

// ������ �����
int writeFile(std::string exePath, std::string nameFile, char* buffer, std::string sizeFile, long long seek);

// ����������� ������ �� ������
int RecvAll(SSL* ssl, char* buffer, int size);

// ����������� ������ � �����
int SendAll(SSL* ssl, char* buffer, int size);
