#include "epollServer.hpp"
#include <memory>

void approach(std::string data)
{
    logMessage(DEBUG, "处理方法# %s", data.c_str());
}
int main()
{
    std::unique_ptr<epollServer> esvr(new epollServer(approach));
    esvr->start();
    return 0;
}