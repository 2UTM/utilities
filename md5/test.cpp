#include <string>
#include "md5.h"
#include <iostream>
#include <cstdio>

int getMd5(std::string str, std::string& result)
{
    MD5 md5 = MD5(str);

    result = md5.hexdigest();

    return 0;
}

int main()
{
    std::string result;
    getMd5("asdasd", result);
    std::cout << result << std::endl;

    system("pause");

    return 0;
}
