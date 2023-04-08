#include "pollServer.hpp"
#include <memory>
int main()
{
    std::unique_ptr<pollServer> psvr(new pollServer());
    psvr->start();
    return 0;
}