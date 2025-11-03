#include<iostream>
#include<string>
using namespace std;
class car{
  private:
   string brand;
   string model;
   int year;
public:
car(string b,string m,int y)  {
  brand=b;
  model=m;
  year=y;
cout<<"constractor print car's details"<<" "<<brand<<" "<<model<<" "<<year<<endl;


} 
~car(){
  cout<<"car was deleted"<<endl;

}
void display(){
  cout<<"brand:"<<brand<<" "<<"model:"<<model<<" "<<"year"<<year<<endl;
}
};
int main(){
  car c1("toyota","corola",2020);
  c1.display();
  
  car c2("bmw","x5",2022);
  c2.display();
  return 0;
}