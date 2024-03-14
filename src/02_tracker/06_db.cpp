// 跟踪服务器
// 实现数据库访问类 二次封装 以对象的形式描述世界，同时尽可能减少耦合
#include "01_globals.h"
#include "03_cache.h"
#include "05_db.h"
// 数据库访问类

// 构造函数
db_c::db_c() : m_mysql(mysql_init(NULL))
{
    // 创建MySQL对象 初始化表先于函数体执行
    if (!m_mysql)
        logger_error("create dao fail:%s", mysql_error(m_mysql));
}
// 析构函数
db_c::~db_c()
{
    // 销毁MySQL对象
    if (m_mysql)
        mysql_close(m_mysql);
    m_mysql = nullptr;
}
// 连接数据库
int db_c::connect()
{
    MYSQL *mysql = m_mysql; // 保存原始的mysql指针
    // mysql_real_connect(m_mysql, 服务器地址，用户名，口令，库名，0，NULL，0)
    // 遍历MySQL地址表，尝试连接数据库
    for (std::vector<std::string>::const_iterator maddr = g_maddrs.begin(); maddr != g_maddrs.end(); ++maddr)
        // 返回一个有连接的mysql地址
        if ((m_mysql = mysql_real_connect(mysql, maddr->c_str(), "root", "12345678", "tnv_trackerdb", 0, NULL, 0)))
            return OK;
    logger_error("connect database fail: %s", mysql_error(m_mysql = mysql));
    return ERROR;
}
// 根据用户ID查找其对应的组名
int db_c::get(char const *userid, std::string &groupname) const
{
    // 查数据库 先从缓存中获取
    cache_c cache;
    acl::string key;
    key.format("userid:%s", userid);
    acl::string value;
    if (cache.get(key, value) == OK)
    {
        groupname = value.c_str();
        return OK;
    }
    // tracker:userid:tnv001 -> group001
    // 缓存中没有，再查询数据库
    acl::string sql;
    sql.format("SELECT group_name FROM t_router WHERE userid='%s';", userid);
    if (mysql_query(m_mysql, sql.c_str()))
    {
        logger_error("query database fail: %s, sql: %s", mysql_error(m_mysql), sql.c_str());
        return ERROR;
    }
    // 获取查询结果
    MYSQL_RES *res = mysql_store_result(m_mysql);
    if (!res)
    {
        logger_error("result is null: %s, sql: %s", mysql_error(m_mysql), sql.c_str());
        return ERROR;
    }
    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row)
    {
        logger_warn("result is empty: %s, sql: %s", mysql_error(m_mysql), sql.c_str());
    }
    else
    {
        groupname = row[0];
        // 查到放缓存里
        cache.set(key, groupname.c_str());
    }
    return OK;
}
// 设置用户ID和组名的对应关系
int db_c::set(char const *appid, char const *userid, char const *groupname) const
{
    // 是否放入缓存，插入不一定查询
    // 插入记录
    acl::string sql;
    sql.format("INSERT INTO t_router SET appid='%s', userid='%s', group_name='%s';", appid, userid, groupname);
    if (mysql_query(m_mysql, sql.c_str()))
    {
        logger_error("insert database fail: %s, sql: %s", mysql_error(m_mysql), sql.c_str());
        return ERROR;
    }
    // 检查插入结果 结果集对象
    MYSQL_RES *res = mysql_store_result(m_mysql);
    if (!res && mysql_field_count(m_mysql))
    {
        // 0允许你取不到结果；非0,应该是能拿到，结果却拿不到，出错了
        logger_error("insert database fail: %s, sql: %s", mysql_error(m_mysql), sql.c_str());
        return ERROR;
    }

    return OK;
}
// 获取全部的组名
int db_c::get(std::vector<std::string> &groupnames) const
{
    // 查询全部组名
    acl::string sql;
    sql.format("SELECT group_name FROM t_groups_info;");
    if (mysql_query(m_mysql, sql.c_str()))
    {
        logger_error("query database fail: %s, sql: %s", mysql_error(m_mysql), sql.c_str());
        return ERROR;
    }
    // 获取查询结果
    MYSQL_RES *res = mysql_store_result(m_mysql);
    if (!res)
    {
        logger_error("result is null: %s, sql: %s", mysql_error(m_mysql), sql.c_str());
        return ERROR;
    }
    // 获取结果记录
    int nrows = mysql_num_rows(res);
    for (int i = 0; i < nrows; ++i)
    {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (!row)
            break;
        // 拿组名字段
        groupnames.push_back(row[0]);
    }
    return OK;
}
