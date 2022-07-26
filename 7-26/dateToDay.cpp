#include <iostream>
using namespace std;
static int days[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
int main(){
        int year,month,day;
            cin >> year >> month >> day;
                int count = 0;
                    month -= 1;
                    while(month){
                        if(month == 2 && year % 4 == 0 && year % 100 != 0 || year % 400 == 0 && month == 2){
                                        count += 29;
                                                
                        }else{
                                        count += days[month];
                                                
                        }
                                month--;
                                    
                    }
                        count += day;
                            cout << count << endl;
                                return 0;

}
