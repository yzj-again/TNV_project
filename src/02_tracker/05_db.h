// 跟踪服务器
// 声明数据库访问类 二次封装 以对象的形式描述世界，同时尽可能减少耦合
#pragma once
#include <string>
#include <vector>
#include <mysql.h>
// 数据库访问类

class db_c
{
public:
    // 从外到内设计类 为了实现这些功能需要其他功能
    // 构造函数
    db_c();
    // 析构函数
    ~db_c();
    // 连接数据库
    int connect();
    // 根据用户ID查找其对应的组名
    int get(char const *userid, std::string &groupname) const;
    // 设置用户ID和组名的对应关系
    int set(char const *appid, char const *userid, char const *groupname) const;
    // 获取全部的组名
    int get(std::vector<std::string> &groupnames) const;

private:
    // MySQL对象 需要初始化 销毁
    MYSQL *m_mysql;
};
