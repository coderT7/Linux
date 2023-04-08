#include "selectServer.hpp"
#include <memory>
int main()
{
    std::unique_ptr<selectServer> svr(new selectServer());
    svr->start();
    return 0;
}