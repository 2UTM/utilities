#include "server.h"
#include <winsock2.h>

std::string file = "win7x64_08.11.2014_Главный_Не_удалять!!!.tib";
long long BLOCK_SIZE = 1500;
long long old_BLOCK_SIZE = 0;

// Инициализация сокета
int initialization(int port)
{
    WSADATA wsadata;
    int ret = WSAStartup(0x101, &wsadata);
    if (ret != 0) {
        printf("WSAStartup() failed with: %d!\n", GetLastError());
        exit(EXIT_FAILURE);
    }

    int s;
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htons(INADDR_ANY);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("Unable to create socket");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Unable to bind");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    if (listen(s, 1) < 0) {
        perror("Unable to listen");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    return s;
}

// Поток для соединения с клиентами
void clientWork(int sock)
{
	printf("Клиент подключился\n");

	int resultSelect = 0;
	FD_SET readfds;
	FD_SET writefds;
	FD_SET erfds;

	struct timeval tmo;
	tmo.tv_sec = 3;
	tmo.tv_usec = 0;

	while (true)
	{
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_ZERO(&erfds);
		FD_SET(sock, &readfds);
		FD_SET(sock, &writefds);
		FD_SET(sock, &erfds);

		resultSelect = select(NULL, &readfds, &writefds, &erfds, &tmo);

		if (resultSelect == 0)
		{
			//Нет данных
			continue;
		}

		if (resultSelect == SOCKET_ERROR)
		{
			printf("Ошибка select(): %d\n", WSAGetLastError());
			break;
		}

		if (FD_ISSET(sock, &erfds)) // ошибки
		{
			printf("Ошибка socket: %d\n", WSAGetLastError());
			break;
		}
		if (FD_ISSET(sock, &readfds)) // получение
		{
			std::string buf;
			if (recvPacket(sock, buf))
			{
				printf("Ошибка recvPacket: %d\n", WSAGetLastError());
				break;
			}
			if (buf == "0;0") // если 0;0, значит скачивание
			{

				// Отправка файла обновления
				long long sizeFile = 0;
				int buffSize = 0;
				char* buffer = new char[BLOCK_SIZE];
				if (readFile(file, 0, sizeFile, buffer, buffSize))
				{
					printf("Ошибка readFile\n");
					break;
				}
				if (sendPacketUpdate(sock, buffer, file.c_str(), std::to_string(sizeFile), std::to_string(buffSize)))
				{
					printf("Ошибка sendPacketUpdate %d \n", WSAGetLastError());
					break;
				}
				delete[] buffer;

				continue;
			}
			else
			{

				// Обработка запроса на докачку файла обновления
				
				std::vector<std::string> vClient;
				// делим строку от клиента по точке с запятой
				boost::split(vClient, buf, boost::is_any_of(";"));

				printf("Клиенту требуется докачка файла %s, текущий размер файла у клиента %s байт\n", vClient[0].c_str(), vClient[1].c_str());

				// Отправка файла обновления
				long long sizeFile = 0;
				int buffSize = 0;

				// Выбор размера блока отправки
				// Если клиент считал все, что отправили, увеличиваем на 1500 байт
				// Если клиент считал меньше, не меняем
				// Если BLOCK_SIZE после увеличения больше или равен чем 999999999, ставим 999999999
				if (atoll(vClient[1].c_str()) - old_BLOCK_SIZE == BLOCK_SIZE)
				{
					BLOCK_SIZE += 1500;
					if (BLOCK_SIZE >= 999999999)
					{
						BLOCK_SIZE = 999999999;
					}
				}
				old_BLOCK_SIZE = atoll(vClient[1].c_str());

				char* buffer = new char[BLOCK_SIZE];
				printf("Текущий размер блока %lld байт\n\n", BLOCK_SIZE);

				if (readFile(file, atoll(vClient[1].c_str()), sizeFile, buffer, buffSize))
				{
					printf("Ошибка readFile\n");
					break;
				}
				if (sendPacketUpdate(sock, buffer, file.c_str(), std::to_string(sizeFile), std::to_string(buffSize)))
				{
					printf("Ошибка sendPacketUpdate %d \n", WSAGetLastError());
					break;
				}
				delete[] buffer;

				continue;
			}
		}
		if (FD_ISSET(sock, &writefds)) // отправка
		{
			
		}
		Sleep(50);
	}
}

// Прием сообщений через сокет
int recvPacket(SOCKET s, std::string& message)
{
	// Читаем размер, первые 4 символа
	std::vector<char> tmpSize(4);
	if (RecvAll(s, &tmpSize.front(), 4))
	{
		return 1;
	}
	std::string sSize(tmpSize.begin(), tmpSize.end());

	// Читаем остальное
	int iSize = atoi(sSize.c_str());
	if (iSize == 0)
	{
		return 1;
	}
	std::vector<char> tmp(iSize);
	if (RecvAll(s, &tmp.front(), iSize))
	{
		return 1;
	}
	std::string result(tmp.begin(), tmp.end());
	message += result;

	return 0;
}

// Отправка обновления
int sendPacketUpdate(SOCKET s, char* packet, std::string nameFile, std::string sizeFile, std::string bufferSize)
{
	// Формируем пакет, первые 9 символов - размер пакета, далее 256 символов - имя файла,
	// далее 12 символов - размер файла

	// размер файла
	std::string resultSizeFile = sizeFile;
	while (true)
	{
		if (strlen(resultSizeFile.c_str()) < 12)
		{
			// размер пакета
			resultSizeFile.insert(resultSizeFile.begin(), '0');
			continue;
		}
		break;
	}

	// имя файла
	std::string resultNameFile = nameFile;
	while (true)
	{
		if (strlen(resultNameFile.c_str()) < 256)
		{
			// размер пакета
			resultNameFile.insert(resultNameFile.begin(), '?');
			continue;
		}
		break;
	}

	// размер пакета
	while (true)
	{
		if (strlen(bufferSize.c_str()) < 9)
		{
			// размер пакета
			bufferSize.insert(bufferSize.begin(), '0');
			continue;
		}
		break;
	}

	// Собираем сообщение
	std::string resultPacket = bufferSize;
	resultPacket += resultNameFile;
	resultPacket += resultSizeFile;

	// Отправляем
	if (SendAll(s, (char*)resultPacket.c_str(), strlen(resultPacket.c_str())))
	{
		return 1;
	}
	if (SendAll(s, packet, atoi(bufferSize.c_str())))
	{
		return 1;
	}

	return 0;
}

// Чтение файла
int readFile(std::string nameFile, long long seek, long long& sizeFile, char* buffer, int& bufferSize)
{
	std::string pathUpdate = "D:\\";

	// Убираем temp из имени файла
	boost::replace_all(nameFile, ".temp", "");

	// Исходный файл
	FILE* fin;

	// Убираем точку с запятой из имени файла
	boost::replace_all(nameFile, ";", "");
	#pragma warning(suppress : 4996) // fopen deprecated
	if (!(fin = fopen((pathUpdate + nameFile).c_str(), "rb")) == NULL)
	{

		// Перемещаем указатель на конец исходного файла
		_fseeki64(fin, 0, SEEK_END);

		// Получаем размер исходного файла
		sizeFile = _ftelli64(fin);

		// Если смещение больше, чем размер файла, делаем блок до конца файла
		if (seek + BLOCK_SIZE > sizeFile)
		{
			BLOCK_SIZE = (seek + BLOCK_SIZE) - sizeFile;
		}

		// Перемещаем указатель на смещение seek исходного файла
		_fseeki64(fin, seek, SEEK_SET);

		// Чтение по байтам "BLOCK_SIZE"
		int readed = fread(buffer, sizeof(char), BLOCK_SIZE, fin);
		if (readed == 0)
		{
			return 1;
		}
		bufferSize = readed;

		fclose(fin);
	}

	return 0;
}

// Проверяем размер файла недокаченного обновления
int getSizeFile(std::string nameFile, long long& sizeFile)
{

	std::string exePath = "d:\\";

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
		size = size - RecvSize;
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
		size = size - SendSize;
		buffer += SendSize;
	}
	return 0;
}

int main()
{
	setlocale(LC_ALL, "ru-RU");

	printf("Сервер запущен\n");

    int sock = initialization(44444);

    struct sockaddr_in addr;
    int len = sizeof(addr);

    int client = accept(sock, (struct sockaddr*)&addr, &len);
    if (client < 0)
    {
        printf("Ошибка accept\n");
        return 1;
    }

    
    // подключение
    clientWork(client);

    return 0;
}
