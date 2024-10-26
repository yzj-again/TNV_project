// HTTP服务器
// 声明业务服务类
//
#pragma once

#include <lib_acl.hpp>
//
// 业务服务类
//
// http服务由acl控制 处理http业务 ->回调虚函数
class service_c : public acl::HttpServlet
{
public:
    service_c(acl::socket_stream *conn, acl::session *sess);

protected:
    bool doGet(acl::HttpServletRequest &req,
               acl::HttpServletResponse &res);
    bool doPost(acl::HttpServletRequest &req,
                acl::HttpServletResponse &res);
    bool doOptions(acl::HttpServletRequest &req,
                   acl::HttpServletResponse &res);
    bool doError(acl::HttpServletRequest &req,
                 acl::HttpServletResponse &res);
    bool doOther(acl::HttpServletRequest &req,
                 acl::HttpServletResponse &res, char const *method);

private:
    // 处理文件路由，便于区分
    bool files(acl::HttpServletRequest &req,
               acl::HttpServletResponse &res);
};
