// 客户机
// 实现连接池管理器类
//
#include "03_pool.h"
#include "05_mngr.h"

// 创建连接池
acl::connect_pool *mngr_c::create_pool(char const *destaddr, size_t count, size_t index)
{
    // class pool_c : public acl::connect_pool 任何时候子类对象都可以当基类对象看
    return new pool_c(destaddr, count, index);
}
