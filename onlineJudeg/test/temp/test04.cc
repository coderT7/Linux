#include <boost/algorithm/string.hpp>
#include <string>

using namespace std;

int main()
{
    string str = "hello world ! my love !!";
    vector<string>res;
    boost::split(res, str, boost::is_any_of(" "), boost::algorithm::token_compress_on);
    for(auto val : res){
        cout << val << endl;
    }
    return 0;
}