// 客户机
// 实现客户机类
//
#include <lib_acl.h>
#include "01_types.h"
#include "03_utils.h"
#include "01_conn.h"
#include "03_pool.h"
#include "05_mngr.h"
#include "07_client.h"

#define MAX_SOCKERRS 10 // 套接字通信错误最大次数

// 加上作用域限定，否则成全局的了
acl::connect_manager *client_c::s_mngr = NULL;
std::vector<std::string> client_c::s_taddrs;
int client_c::s_scount = 8;

// 初始化
// 只初始化一次
int client_c::init(char const *taddrs, int tcount, int scount)
{
    if (s_mngr)
        return OK;

    // 跟踪服务器地址表
    if (!taddrs || !strlen(taddrs))
    {
        logger_error("tracker addresses is null");
        return ERROR;
    }
    // 拆分taddrs
    split(taddrs, s_taddrs);
    if (s_taddrs.empty())
    {
        logger_error("tracker addresses is empty");
        return ERROR;
    }

    // 跟踪服务器连接数上限
    if (tcount <= 0)
    {
        logger_error("invalid tracker connection pool count %d <= 0", tcount);
        return ERROR;
    }

    // 存储服务器连接数上限
    if (scount <= 0)
    {
        logger_error("invalid storage connection pool count %d <= 0", scount);
        return ERROR;
    }
    s_scount = scount;

    // 创建连接池管理器
    if (!(s_mngr = new mngr_c))
    {
        logger_error("create connection pool manager fail: %s", acl_last_serror());
        return ERROR;
    }

    // 初始化跟踪服务器连接池 拆分地址表，依次调用create_pool 键值对
    // 调用基类acl::connect_manager的init
    s_mngr->init(NULL, taddrs, tcount);

    return OK;
}

// 终结化
void client_c::deinit()
{
    if (s_mngr)
    {
        delete s_mngr;
        s_mngr = NULL;
    }
    // vector清空
    s_taddrs.clear();
}

// 从跟踪服务器获取存储服务器地址列表
int client_c::saddrs(char const *appid, char const *userid, char const *fileid, std::string &saddrs)
{
    if (s_taddrs.empty())
    {
        logger_error("tracker addresses is empty");
        return ERROR;
    }

    int result = ERROR;

    // 生成有限随机数
    srand(time(NULL));
    int ntaddrs = s_taddrs.size();
    int nrand = rand() % ntaddrs;

    for (int i = 0; i < ntaddrs; ++i)
    {
        // 随机抽取跟踪服务器地址
        char const *taddr = s_taddrs[nrand].c_str();
        // 围绕取余
        nrand = (nrand + 1) % ntaddrs;

        // 获取跟踪服务器连接池
        // (pool_c *)s_mngr向下造型
        /**
         * 根据服务端地址获得该服务器的连接池
         * @param addr {const char*} redis 服务器地址(ip:port)
         * @param exclusive {bool} 是否需要互斥访问连接池数组，当需要动态
         *  管理连接池集群时，该值应为 true
         * @param restore {bool} 当该服务结点被置为不可用时，该参数决定是否
         *  自动将之恢复为可用状态
         * @return {connect_pool*} 返回空表示没有此服务
         */
        // connect_pool* get(const char* addr, bool exclusive = true,bool restore = false);
        /*
        1.首先，s_mngr->get(taddr) 会被执行，即调用 s_mngr 对象的 get 方法，并传递参数 taddr。
        2.get 方法根据传入的 taddr 参数执行相应的操作，并返回一个指向 pool_c 类型对象的指针。
        3.接下来，将返回的指针进行类型转换，使用 (pool_c *) 进行显式的类型转换。这将把指针从connect_pool*类型转换为 pool_c 类型指针。
        4.最终，得到的经过类型转换的指针 (pool_c *)s_mngr->get(taddr) 将作为整个表达式的结果。
        */
        pool_c *tpool = (pool_c *)s_mngr->get(taddr);
        if (!tpool)
        {
            logger_warn("tracker connection pool is null, taddr: %s", taddr);
            continue;
        }
        // 从连接池获取连接试错
        for (int sockerrs = 0; sockerrs < MAX_SOCKERRS; ++sockerrs)
        {
            // 获取跟踪服务器连接
            conn_c *tconn = (conn_c *)tpool->peek();
            if (!tconn)
            {
                logger_warn("tracker connection is null, taddr: %s", taddr);
                break;
            }

            // 从跟踪服务器获取存储服务器地址列表
            result = tconn->saddrs(appid, userid, fileid, saddrs);

            if (result == SOCKET_ERROR)
            {
                logger_warn("get storage addresses fail: %s", tconn->errdesc());
                // 还回连接池
                tpool->put(tconn, false);
            }
            else
            {
                if (result == OK)
                    tpool->put(tconn, true);
                else
                {
                    logger_error("get storage addresses fail: %s", tconn->errdesc());
                    tpool->put(tconn, false);
                }
                return result;
            }
        }
    }

    return result;
}

// 从跟踪服务器获取组列表
int client_c::groups(std::string &groups)
{
    if (s_taddrs.empty())
    {
        logger_error("tracker addresses is empty");
        return ERROR;
    }

    int result = ERROR;

    // 生成有限随机数
    srand(time(NULL));
    int ntaddrs = s_taddrs.size();
    int nrand = rand() % ntaddrs;

    for (int i = 0; i < ntaddrs; ++i)
    {
        // 随机抽取跟踪服务器地址
        char const *taddr = s_taddrs[nrand].c_str();
        nrand = (nrand + 1) % ntaddrs;

        // 获取跟踪服务器连接池
        pool_c *tpool = (pool_c *)s_mngr->get(taddr);
        if (!tpool)
        {
            logger_warn("tracker connection pool is null, taddr: %s", taddr);
            continue;
        }

        for (int sockerrs = 0; sockerrs < MAX_SOCKERRS; ++sockerrs)
        {
            // 获取跟踪服务器连接
            conn_c *tconn = (conn_c *)tpool->peek();
            if (!tconn)
            {
                logger_warn("tracker connection is null, taddr: %s", taddr);
                break;
            }

            // 从跟踪服务器获取组列表
            result = tconn->groups(groups);

            if (result == SOCKET_ERROR)
            {
                logger_warn("get groups fail: %s", tconn->errdesc());
                tpool->put(tconn, false);
            }
            else
            {
                if (result == OK)
                    tpool->put(tconn, true);
                else
                {
                    logger_error("get groups fail: %s", tconn->errdesc());
                    tpool->put(tconn, false);
                }
                return result;
            }
        }
    }

    return result;
}

// 向存储服务器上传文件
int client_c::upload(char const *appid, char const *userid, char const *fileid, char const *filepath)
{
    // 检查参数
    if (!appid || !strlen(appid))
    {
        logger_error("appid is null");
        return ERROR;
    }
    if (!userid || !strlen(userid))
    {
        logger_error("userid is null");
        return ERROR;
    }
    if (!fileid || !strlen(fileid))
    {
        logger_error("fileid is null");
        return ERROR;
    }
    if (!filepath || !strlen(filepath))
    {
        logger_error("filepath is null");
        return ERROR;
    }

    // 从跟踪服务器获取存储服务器地址列表
    int result;
    std::string ssaddrs;
    if ((result = saddrs(appid, userid, fileid, ssaddrs)) != OK)
        return result;
    std::vector<std::string> vsaddrs;
    split(ssaddrs.c_str(), vsaddrs);
    if (vsaddrs.empty())
    {
        logger_error("storage addresses is empty");
        return ERROR;
    }

    result = ERROR;

    for (std::vector<std::string>::const_iterator saddr = vsaddrs.begin(); saddr != vsaddrs.end(); ++saddr)
    {
        // 获取存储服务器连接池
        pool_c *spool = (pool_c *)s_mngr->get(saddr->c_str());
        if (!spool)
        {
            // 第一次出现，新建一个连接池，交给连接池管理器管理
            s_mngr->set(saddr->c_str(), s_scount);
            if (!(spool = (pool_c *)s_mngr->get(saddr->c_str())))
            {
                logger_warn("storage connection pool is null, saddr: %s", saddr->c_str());
                continue;
            }
        }

        for (int sockerrs = 0; sockerrs < MAX_SOCKERRS; ++sockerrs)
        {
            // 获取存储服务器连接
            conn_c *sconn = (conn_c *)spool->peek();
            if (!sconn)
            {
                logger_warn("storage connection is null, saddr: %s", saddr->c_str());
                break;
            }

            // 向存储服务器上传文件
            result = sconn->upload(appid, userid, fileid, filepath);

            if (result == SOCKET_ERROR)
            {
                logger_warn("upload file fail: %s", sconn->errdesc());
                spool->put(sconn, false);
            }
            else
            {
                if (result == OK)
                    spool->put(sconn, true);
                else
                {
                    logger_error("upload file fail: %s", sconn->errdesc());
                    spool->put(sconn, false);
                }
                return result;
            }
        }
    }

    return result;
}

// 向存储服务器上传文件
int client_c::upload(char const *appid, char const *userid, char const *fileid, char const *filedata, long long filesize)
{
    // 检查参数
    if (!appid || !strlen(appid))
    {
        logger_error("appid is null");
        return ERROR;
    }
    if (!userid || !strlen(userid))
    {
        logger_error("userid is null");
        return ERROR;
    }
    if (!fileid || !strlen(fileid))
    {
        logger_error("fileid is null");
        return ERROR;
    }
    if (!filedata || !filesize)
    {
        logger_error("filedata is null");
        return ERROR;
    }

    // 从跟踪服务器获取存储服务器地址列表
    int result;
    std::string ssaddrs;
    if ((result = saddrs(appid, userid, fileid, ssaddrs)) != OK)
        return result;
    std::vector<std::string> vsaddrs;
    split(ssaddrs.c_str(), vsaddrs);
    if (vsaddrs.empty())
    {
        logger_error("storage addresses is empty");
        return ERROR;
    }

    result = ERROR;

    for (std::vector<std::string>::const_iterator saddr = vsaddrs.begin(); saddr != vsaddrs.end(); ++saddr)
    {
        // 获取存储服务器连接池
        pool_c *spool = (pool_c *)s_mngr->get(saddr->c_str());
        if (!spool)
        {
            s_mngr->set(saddr->c_str(), s_scount);
            if (!(spool = (pool_c *)s_mngr->get(saddr->c_str())))
            {
                logger_warn("storage connection pool is null, saddr: %s", saddr->c_str());
                continue;
            }
        }

        for (int sockerrs = 0; sockerrs < MAX_SOCKERRS; ++sockerrs)
        {
            // 获取存储服务器连接
            conn_c *sconn = (conn_c *)spool->peek();
            if (!sconn)
            {
                logger_warn("storage connection is null, saddr: %s", saddr->c_str());
                break;
            }

            // 向存储服务器上传文件
            result = sconn->upload(
                appid, userid, fileid, filedata, filesize);

            if (result == SOCKET_ERROR)
            {
                logger_warn("upload file fail: %s", sconn->errdesc());
                spool->put(sconn, false);
            }
            else
            {
                if (result == OK)
                    spool->put(sconn, true);
                else
                {
                    logger_error("upload file fail: %s", sconn->errdesc());
                    spool->put(sconn, false);
                }
                return result;
            }
        }
    }

    return result;
}

// 向存储服务器询问文件大小
int client_c::filesize(char const *appid, char const *userid, char const *fileid, long long *filesize)
{
    // 检查参数
    if (!appid || !strlen(appid))
    {
        logger_error("appid is null");
        return ERROR;
    }
    if (!userid || !strlen(userid))
    {
        logger_error("userid is null");
        return ERROR;
    }
    if (!fileid || !strlen(fileid))
    {
        logger_error("fileid is null");
        return ERROR;
    }

    // 从跟踪服务器获取存储服务器地址列表
    int result;
    std::string ssaddrs;
    if ((result = saddrs(appid, userid, fileid, ssaddrs)) != OK)
        return result;
    std::vector<std::string> vsaddrs;
    split(ssaddrs.c_str(), vsaddrs);
    if (vsaddrs.empty())
    {
        logger_error("storage addresses is empty");
        return ERROR;
    }

    result = ERROR;

    for (std::vector<std::string>::const_iterator saddr = vsaddrs.begin(); saddr != vsaddrs.end(); ++saddr)
    {
        // 获取存储服务器连接池
        pool_c *spool = (pool_c *)s_mngr->get(saddr->c_str());
        if (!spool)
        {
            s_mngr->set(saddr->c_str(), s_scount);
            if (!(spool = (pool_c *)s_mngr->get(saddr->c_str())))
            {
                logger_warn("storage connection pool is null, saddr: %s", saddr->c_str());
                continue;
            }
        }

        for (int sockerrs = 0; sockerrs < MAX_SOCKERRS; ++sockerrs)
        {
            // 获取存储服务器连接
            conn_c *sconn = (conn_c *)spool->peek();
            if (!sconn)
            {
                logger_warn("storage connection is null, saddr: %s", saddr->c_str());
                break;
            }

            // 向存储服务器询问文件大小
            result = sconn->filesize(appid, userid, fileid, filesize);

            if (result == SOCKET_ERROR)
            {
                logger_warn("get filesize fail: %s", sconn->errdesc());
                spool->put(sconn, false);
            }
            else
            {
                if (result == OK)
                    spool->put(sconn, true);
                else
                {
                    logger_error("get filesize fail: %s", sconn->errdesc());
                    spool->put(sconn, false);
                }
                return result;
            }
        }
    }

    return result;
}

// 从存储服务器下载文件
int client_c::download(char const *appid, char const *userid, char const *fileid, long long offset, long long size, char **filedata, long long *filesize)
{
    // 检查参数
    if (!appid || !strlen(appid))
    {
        logger_error("appid is null");
        return ERROR;
    }
    if (!userid || !strlen(userid))
    {
        logger_error("userid is null");
        return ERROR;
    }
    if (!fileid || !strlen(fileid))
    {
        logger_error("fileid is null");
        return ERROR;
    }

    // 从跟踪服务器获取存储服务器地址列表
    int result;
    std::string ssaddrs;
    if ((result = saddrs(appid, userid, fileid, ssaddrs)) != OK)
        return result;
    std::vector<std::string> vsaddrs;
    split(ssaddrs.c_str(), vsaddrs);
    if (vsaddrs.empty())
    {
        logger_error("storage addresses is empty");
        return ERROR;
    }

    result = ERROR;

    for (std::vector<std::string>::const_iterator saddr = vsaddrs.begin(); saddr != vsaddrs.end(); ++saddr)
    {
        // 获取存储服务器连接池
        pool_c *spool = (pool_c *)s_mngr->get(saddr->c_str());
        if (!spool)
        {
            s_mngr->set(saddr->c_str(), s_scount);
            if (!(spool = (pool_c *)s_mngr->get(saddr->c_str())))
            {
                logger_warn("storage connection pool is null, saddr: %s", saddr->c_str());
                continue;
            }
        }

        for (int sockerrs = 0; sockerrs < MAX_SOCKERRS; ++sockerrs)
        {
            // 获取存储服务器连接
            conn_c *sconn = (conn_c *)spool->peek();
            if (!sconn)
            {
                logger_warn("storage connection is null, saddr: %s", saddr->c_str());
                break;
            }

            // 从存储服务器下载文件
            result = sconn->download(appid, userid, fileid, offset, size, filedata, filesize);

            if (result == SOCKET_ERROR)
            {
                logger_warn("download file fail: %s", sconn->errdesc());
                spool->put(sconn, false);
            }
            else
            {
                if (result == OK)
                    spool->put(sconn, true);
                else
                {
                    logger_error("download file fail: %s", sconn->errdesc());
                    spool->put(sconn, false);
                }
                return result;
            }
        }
    }

    return result;
}

// 删除存储服务器上的文件
int client_c::del(char const *appid, char const *userid, char const *fileid)
{
    // 检查参数
    if (!appid || !strlen(appid))
    {
        logger_error("appid is null");
        return ERROR;
    }
    if (!userid || !strlen(userid))
    {
        logger_error("userid is null");
        return ERROR;
    }
    if (!fileid || !strlen(fileid))
    {
        logger_error("fileid is null");
        return ERROR;
    }

    // 从跟踪服务器获取存储服务器地址列表
    int result;
    std::string ssaddrs;
    if ((result = saddrs(appid, userid, fileid, ssaddrs)) != OK)
        return result;
    std::vector<std::string> vsaddrs;
    split(ssaddrs.c_str(), vsaddrs);
    if (vsaddrs.empty())
    {
        logger_error("storage addresses is empty");
        return ERROR;
    }

    result = ERROR;

    for (std::vector<std::string>::const_iterator saddr = vsaddrs.begin(); saddr != vsaddrs.end(); ++saddr)
    {
        // 获取存储服务器连接池
        pool_c *spool = (pool_c *)s_mngr->get(saddr->c_str());
        if (!spool)
        {
            s_mngr->set(saddr->c_str(), s_scount);
            if (!(spool = (pool_c *)s_mngr->get(saddr->c_str())))
            {
                logger_warn("storage connection pool is null, saddr: %s", saddr->c_str());
                continue;
            }
        }

        for (int sockerrs = 0; sockerrs < MAX_SOCKERRS; ++sockerrs)
        {
            // 获取存储服务器连接
            conn_c *sconn = (conn_c *)spool->peek();
            if (!sconn)
            {
                logger_warn("storage connection is null, saddr: %s", saddr->c_str());
                break;
            }

            // 删除存储服务器上的文件
            result = sconn->del(appid, userid, fileid);

            if (result == SOCKET_ERROR)
            {
                logger_warn("delete file fail: %s", sconn->errdesc());
                spool->put(sconn, false);
            }
            else
            {
                if (result == OK)
                    spool->put(sconn, true);
                else
                {
                    logger_error("delete file fail: %s", sconn->errdesc());
                    spool->put(sconn, false);
                }
                return result;
            }
        }
    }

    return result;
}
