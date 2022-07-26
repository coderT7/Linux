#include <iostream>
#include <cstdio>

using namespace std;

int month[13][2]={//平年和闰年的天数
    {0,0},{31,31},{28,29},{31,31},{30,30},{31,31},{30,30},{31,31},{31,31},
    {30,30},{31,31},{30,30},{31,31}
};

bool isLeap(int y){//判断是否是闰年
    return (y%4==0 && y%100 != 0 )||(y%400==0);
}
int main(){
    int n,y1,m1,d1,count;
    cin >> n;
    while(n--){
        cin >> y1 >> m1 >> d1 >> count;

        while(count--){//第一个日期没有达到第二个日期时循环
            d1++;//天数加一
            if(d1 == month[m1][isLeap(y1)]+1){//满当月天数
                m1++;//日期变为下个月的一号
                d1=1;
            }
            if(m1==13){//月份满十二个月
                y1++;//日期变为下一年的一月
                m1=1;
            }
        }
        printf("%4d-%02d-%02d\n",y1,m1,d1);

    }
    return 0;

}
