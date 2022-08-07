#include<iostream>

using namespace std;

class C{
public:
    C(){
        cout << "C的构造" << endl;
    }
    ~C(){
        cout << "C的析构" << endl;
    }
};
class A{
public:
    A(){
        cout << "A的构造" << endl;
    }
    ~A(){
        cout << "A的析构" << endl;
    }
};
class B{
public:
    B(){
        cout << "B的构造" << endl;
    }
    ~B(){
        cout << "B的析构" << endl;
    }
};
class D{
public:
    D(){
        cout << "D的构造" << endl;
    }
    ~D(){
        cout << "D的析构" << endl;
    }
};
C c;
int main()
{
    A* pa = new A[10];
    delete[] pa;
//    A* pa = new A();
//    B b;
//    static D d;
//    delete pa;
    return 0;
}
