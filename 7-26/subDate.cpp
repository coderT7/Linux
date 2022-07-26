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
    int time1,y1,m1,d1;
    int time2,y2,m2,d2;
    while(scanf("%d%d",&time1,&time2)!=EOF){
        if(time1 > time2){
            int temp = time1;
            time1 = time2;
            time2 = temp;

        }

        y1 = time1/10000;
        m1 = time1%10000/100;
        d1 = time1%100;
        y2 = time2/10000;
        m2 = time2%10000/100;
        d2 = time2%100;
        int ans = 1;//初始化结果，相差天数

        while((y1<y2) || (m1 < m2) || (d1 < d2)){
            //第一个日期没有达到第二个日期时循环
            d1++;//天数加一
            if(d1 == month[m1][isLeap(y1)]+1){//满当月天数
                m1++;//日期变为下个月的一号
                d1=1;
            }
            if(m1==13){//月份满十二个月
                y1++;//日期变为下一年的一月
                m1=1;
            }
            ans++;//相差天数+1
        }
        printf("%d\n",ans);

    }
    return 0;
}
