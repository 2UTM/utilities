#pragma once
#include <winsock2.h>
#include <thread>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <fstream> 
#pragma comment(lib,"ws2_32.lib") //���������� Winsock


// ������������� ������
int initialization(int port);

// ����� ��� ���������� � ���������
void clientWork(int sock);

// ����� ��������� ����� �����
int recvPacket(SOCKET s, std::string& message);

// �������� ����������
int sendPacketUpdate(SOCKET s, char* packet, std::string nameFile, std::string sizeFile, std::string bufferSize);

// ������ �����
int readFile(std::string nameFile, long long seek, long long& sizeFile, char* buffer, int& bufferSize);

// ��������� ������ ����� ������������� ����������
int getSizeFile(std::string nameFile, long long& sizeFile);

// ����������� ������ �� ������
int RecvAll(SOCKET sock, char* buffer, int size);

// ����������� ������ � �����
int SendAll(SOCKET sock, char* buffer, int size);
