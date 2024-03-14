// 公共模块
// 声明几个实用函数
#pragma once

#include <string>
#include <vector>

// long long 主机序转网络序
void llton(long long ll, char *n);

// long long 网络序转主机序
long long ntoll(char const *n);

// long 主机序转网络序
void lton(long ll, char *n);

// long 网络序转主机序
long ntol(char const *n);

// short 主机序转网络序
void ston(short s, char *n);

// long 网络序转主机序
short ntos(char const *n);

// 字符串是否合法
int valid(char const *str);

// 分号为分割符将一个字符串进行拆分
int split(char const *str, std::vector<std::string> &substrs);