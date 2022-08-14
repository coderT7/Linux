#include<vector>
#include<unordered_map>
using namespace std;
class Solution {

    public:
        int singleNumber(vector<int>& nums) {
            unordered_map<int,int>record;
            for(auto val : nums){
                record[val]++;

            }
            for(auto val : record){
                if(val.second == 1){
                    return val.first;

                }

            }
            return -1;

        }

};
