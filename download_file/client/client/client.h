#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS


#include <stdio.h>
#include <string>
#include <vector>
#include <winsock2.h>
#include <boost/algorithm/string/replace.hpp>

#pragma comment(lib,"ws2_32.lib") //���������� Winsock

int initialization(); // ������������ ������
int connectToServer(std::string ip, int port); // ����������� � �������
int RecvPacket(std::string& packet); // ��������� ��������� �� ������
int RecvPacketUpdate(std::string& nameFile, std::string& sizeFile, int& seek); // ��������� ��������� �� ������ ��� ����������
int SendPacket(std::string message); // ��������� ��������� � �����
int doWork(int sock); // ������ ����������

// ����� temp ������ � ����������
int searchTempFileUpdate();

// ��������� ������ ����� ������������� ����������
int getSizeFile(std::string nameFile, long long& sizeFile);

// ������ �����
int writeFile(std::string nameFile, char* buffer, std::string sizeFile, int seek);

// ����������� ������ �� ������
int RecvAll(SOCKET sock, char* buffer, int size);
