// 跟踪服务器
// 定义全局变量，并初始化

#include "01_globals.h"

// 配置信息
char *cfg_appids; // 应用ID表，应用限制
char *cfg_maddrs; // MySQL地址表
char *cfg_raddrs; // Rdedis地址表
acl::master_str_tbl cfg_str[] = {
    {"tnv_apps_id", "tnvideo", &cfg_appids},
    {"mysql_addrs", "127.0.0.1", &cfg_maddrs},
    {"redis_addrs", "127.0.0.1:6379", &cfg_raddrs},
    {0, 0, 0},
}; // 字符串配置表 键和对应的变量关联起来，将来键所对应的值会读取到变量里

int cfg_interval; // 存储服务器状态检查间隔秒数
int cfg_mtimeout; // MySQL读写超时
int cfg_maxconns; // Redis连接池最大连接数
int cfg_ctimeout; // Redis连接超时
int cfg_rtimeout; // Redis读写超时
int cfg_ktimeout; // Redis键超时 创建键值对要在b+树
acl::master_int_tbl cfg_int[] = {
    {"check_active_interval", 120, &cfg_interval, 0, 0},
    {"mysql_rw_timeout", 30, &cfg_mtimeout, 0, 0},
    {"redis_max_conn_num", 600, &cfg_maxconns, 0, 0},
    {"redis_conn_timeout", 10, &cfg_ctimeout, 0, 0},
    {"redis_rw_timeout", 10, &cfg_rtimeout, 0, 0},
    {"redis_key_timeout", 60, &cfg_ktimeout, 0, 0},
    {0, 0, 0, 0, 0},
}; // 整型配置表

std::vector<std::string> g_appids; // 应用ID表
std::vector<std::string> g_maddrs; // MySQL地址表
std::vector<std::string> g_raddrs; // Redis地址表
acl::redis_client_pool *g_rconns;  // Redis连接池
std::string g_hostname;
// string <-> list<storage_info_t>的映射组表 多线程 map非线程安全
// 上面是只读，下面是可读可写
std::map<std::string, std::list<storage_info_t>> g_groups;
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER; // 互斥锁
