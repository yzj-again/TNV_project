// 公共模块
// 定义所有模块都会用到的宏和数据类型
#pragma once

#include <netinet/in.h>

// 函数返回值
#define OK 0            // 成功
#define ERROR -1        // 本地错误
#define SOCKET_ERROR -2 // 套接字通信错误
#define STATUS_ERROR -3 // 服务器状态异常

// redis缓存-内存级别
// 多个服务器公用一个redis缓存，造成键冲突
// eg: tracker:userid storage:userid
#define TRACKER_REDIS_PREFIX "tracker" // 跟踪服务器redis前缀
#define STORAGE_REDIS_PREFIX "storage" // 存储服务器redis前缀

// 存储服务器状态
// 离线 在线 活动
typedef enum storage_status
{
    STORAGE_STATUS_OFFLINE,
    STORAGE_STATUS_ONLINE,
    STORAGE_STATUS_ACTIVE
} storage_status_t;

// 存储服务器加入和信息
// 存储服务器信息各不相同，用自定义结构存
#define STORAGE_VERSION_MAX 6    // 版本最大字符数
#define STORAGE_GROUPNAME_MAX 16 // 组名最大字符数
#define STORAGE_HOSTNAME_MAX 128 // 主机名最大字符数
#define STORAGE_ADDR_MAX 16      // 地址最大字符数

typedef struct storage_join
{
    char sj_version[STORAGE_VERSION_MAX + 1];     // 版本
    char sj_groupname[STORAGE_GROUPNAME_MAX + 1]; // 组名
    char sj_hostname[STORAGE_HOSTNAME_MAX + 1];   // 主机名
    in_port_t sj_port;                            // 存储服务器侦听客户机的端口号，将来要给跟踪服务器
    time_t sj_stime;                              // 启动时间
    time_t sj_jtime;                              // 加入时间
} storage_join_t;                                 // 存储服务器加入

typedef struct storage_info
{
    char si_version[STORAGE_VERSION_MAX + 1]; // 版本
    // 组名 组名作为键，键里有这里可以不要
    char si_hostname[STORAGE_HOSTNAME_MAX + 1]; // 主机名
    char si_addr[STORAGE_ADDR_MAX + 1];         // IP地址
    in_port_t si_port;                          // 存储服务器侦听客户机的端口号，将来要给跟踪服务器
    time_t si_stime;                            // 启动时间
    time_t si_jtime;                            // 加入时间
    time_t si_btime;                            // 心跳时间
    storage_status_t si_status;                 // 状态
} storage_info_t;

// ID键值对
#define ID_KEY_MAX 64 // 键最大字符数
typedef struct id_pair
{
    char id_key[ID_KEY_MAX + 1]; // 键
    long id_value;               // 值 阶跃
    int id_offset;               // 偏移 累加
} id_pair_t;                     // ID键值对

// 存储服务器读写磁盘文件缓冲区
#define STORAGE_RCVWR_SIZE (512 * 1024) // 接收写入缓冲区字节数
#define STORAGE_RDSND_SIZE (512 * 1024) // 读取发送缓冲区字节数