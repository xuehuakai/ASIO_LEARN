/*mutable_buffer-写服务 const_buffer-读服务*/
#include<string>
#include<iostream>
using namespace std;
int main(){
    string a{"daw"};
    string b{'1',3};
    string c('2',4);
    string d = {'8',9};
    cout<<a
   <<endl <<b<<endl<<c<<d<<endl;
    return 0;
}