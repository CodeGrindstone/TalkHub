//
// Created by 31435 on 2024/9/4.
//

#ifndef LOGICSYSTEM_H
#define LOGICSYSTEM_H


#include "../Singleton.h"
#include "../../const.h"
#include "../../RedisMgr/RedisMgr.h"

#include <map>
#include <functional>
#include <json/reader.h>
#include <json/value.h>
#include <json/json.h>

class HttpConn;
typedef std::function<void(std::shared_ptr<HttpConn>)> HttpHandler;
class LogicSystem:public Singleton<LogicSystem>,
public std::enable_shared_from_this<LogicSystem>
{
    friend class Singleton<LogicSystem>;
public:
    ~LogicSystem();
    bool HandleGet(std::string url, std::shared_ptr<HttpConn> conn);
    bool HandlePost(std::string url, std::shared_ptr<HttpConn> conn);
    void RegGet(std::string, HttpHandler);
    void RegPost(std::string, HttpHandler);
private:
    LogicSystem();

private:
    std::map<std::string, HttpHandler> m_PostHandlers;
    std::map<std::string, HttpHandler> m_GetHandlers;
};



#endif //LOGICSYSTEM_H
