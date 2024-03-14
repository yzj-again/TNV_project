// 公共模块
// 定义几个实用函数

#include "01_types.h"
#include "03_utils.h"
#include <string.h>

// long long 主机序转网络序
void llton(long long ll, char *n)
{
    for (size_t i = 0; i < sizeof(ll); ++i)
    {
        n[i] = ll >> (sizeof(ll) - i - 1) * 8;
    }
}

// long long 网络序转主机序
long long ntoll(char const *n)
{
    long long ll = 0;
    for (size_t i = 0; i < sizeof(ll); ++i)
    {
        ll |= (long long)(unsigned char)n[i] << (sizeof(ll) - i - 1) * 8;
    }
    return ll;
}

// long 主机序转网络序
void lton(long l, char *n)
{
    for (size_t i = 0; i < sizeof(l); ++i)
    {
        n[i] = l >> (sizeof(l) - i - 1) * 8;
    }
}

// long 网络序转主机序
long ntol(char const *n)
{
    long l = 0;
    for (size_t i = 0; i < sizeof(l); ++i)
    {
        l |= (long)(unsigned char)n[i] << (sizeof(l) - i - 1) * 8;
    }
    return l;
}

// short 主机序转网络序
void ston(short s, char *n)
{
    for (size_t i = 0; i < sizeof(s); ++i)
    {
        n[i] = s >> (sizeof(s) - i - 1) * 8;
    }
}

// long 网络序转主机序
short ntos(char const *n)
{
    short s = 0;
    for (size_t i = 0; i < sizeof(s); ++i)
    {
        s |= (short)(unsigned char)n[i] << (sizeof(s) - i - 1) * 8;
    }
    return s;
}

// 字符串是否合法
int valid(char const *str)
{
    // 空指针判断
    if (!str)
        return ERROR;
    // 空串判断
    size_t len = strlen(str);
    if (!len)
        return ERROR;
    // 检查容易判断
    for (size_t i = 0; i < len; ++i)
        if (!(('a' <= str[i] && str[i] <= 'z') ||
              ('A' <= str[i] && str[i] <= 'Z') ||
              ('0' <= str[i] && str[i] <= '9')))
            return ERROR;
    return OK;
}

// 分号为分割符将一个字符串进行拆分
int split(char const *str, std::vector<std::string> &substrs)
{
    if (!str)
        return ERROR;

    size_t len = strlen(str);
    if (!len)
        return ERROR;
    // 拷贝字符串数组
    char *buf = new char[len + 1];
    strcpy(buf, str);

    char const *sep = ";";
    for (char *substr = strtok(buf, sep); substr;
         substr = strtok(NULL, sep))
        // NULL，strtok会保留上一次结果
        substrs.push_back(substr);

    delete[] buf;

    return OK;
}