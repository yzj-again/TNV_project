// ID服务器
// 声明ID数据库访问类
//
#pragma once

#include <mysql.h>
//
// 数据库访问类
//
class db_c
{
public:
  // 构造函数
  db_c();
  // 析构函数
  ~db_c();

  // 连接数据库
  int connect();

  // 获取ID当前值，同时产生下一个值
  int get(char const *key, int inc, long *value) const;

private:
  MYSQL *m_mysql; // MySQL对象
};
