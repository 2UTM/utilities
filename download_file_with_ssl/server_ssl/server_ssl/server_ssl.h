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
#pragma comment(lib,"ws2_32.lib") //���������� Winsock


// ������������� ������
int initialization(int port);

// ����� ��� ���������� � ���������
void clientWork(SSL* ssl, int sock);

// ����� ��������� ����� �����
int recvPacket(SSL* ssl, std::string& message);

// �������� ����������
int sendPacketUpdate(SSL* ssl, char* packet, std::string nameFile, std::string sizeFile, std::string bufferSize);

// ������ �����
int readFile(std::string nameFile, long long seek, long long& sizeFile, char* buffer, int& bufferSize);

// ��������� ������ ����� ������������� ����������
int getSizeFile(std::string nameFile, long long& sizeFile);

// ����������� ������ �� ������
int RecvAll(SSL* ssl, char* buffer, int size);

// ����������� ������ � �����
int SendAll(SSL* ssl, char* buffer, int size);

//�������� SSL
SSL_CTX* createContext();

// ������������ ���������
void configureContext(SSL_CTX* ctx);

//�������� ������� ��� ����� ������ �� ��������� �����
int pemPasswd(char* buf, int size, int rwflag, void* password);