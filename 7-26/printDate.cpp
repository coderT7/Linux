#include <iostream>
using namespace std;
int months[13][2]={//平年和闰年的天数
    {0,0},{31,31},{28,29},{31,31},{30,30},{31,31},{30,30},{31,31},{31,31},
    {30,30},{31,31},{30,30},{31,31}
};
bool isLeapYear(int year){
    if((year % 4 == 0 && year % 100 != 0) || year % 400 == 0){
        return true;

    }else{
        return false;

    }

}
int main(){
    int year,month,day;
    while(cin >> year >> day){
        month = 1;
        if(day-months[month][isLeapYear(year)] <= 0){
            printf("%4d-%02d-%02d\n",year,month,day);

        }else{
            while(day - months[month][isLeapYear(year)] > 0){
                if(month == 2 && isLeapYear(year)){
                    day -= 29;

                }else{
                    day -= months[month][isLeapYear(year)];

                }
                month++;
                if(month == 13){
                    month = 1;
                    year++;

                }

            }
            printf("%4d-%02d-%02d\n",year,month,day);

        }

    }
    return 0;

}
