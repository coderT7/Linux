#include "compiler.hpp"
#include "runner.hpp"
using namespace ns_run;
using namespace ns_compile;
int main()
{
    Compiler::compile("test");
    Runner::run("test");
    return 0;
}