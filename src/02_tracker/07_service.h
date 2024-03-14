// 跟踪服务器
// 声明业务服务类
#pragma once
#include <lib_acl.hpp>
#include "01_types.h"
// 业务服务类
class service_c
{
private:
public:
    service_c(/* args */);
    // socket流
    bool bussiness(acl::socket_stream *conn, char const *head) const;
    ~service_c();
};
