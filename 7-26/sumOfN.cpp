class Sum{
public:
    static int sum,i;
    Sum(){
        sum += i;
        i++;
    }
};

int Sum::sum = 0;
int Sum::i = 1;
class Solution {
public:
    int Sum_Solution(int n) {
        Sum s[n];
        return Sum::sum;
    }
};
