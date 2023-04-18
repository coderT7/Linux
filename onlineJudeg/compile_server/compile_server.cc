#include "compiler.hpp"
#include "runner.hpp"
using namespace ns_run;
using namespace ns_compile;
int main()
{
    Compiler::compile("test");
    Runner::run("test", 1, 1000);
    return 0;
}