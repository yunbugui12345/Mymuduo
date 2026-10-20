#include<muduo_mine/Timestamp.h>

#include<time.h>

using namespace std;

int main()
{
	// int64_t m=time(nullptr);
	// std::cout<<m<<std::endl;
	int64_t a;
	tm* Time=localtime(&a);
	std::cout<<Timestamp::now().tostring()<<std::endl;
	cout<<Time->tm_year+1900<<" "<<Time->tm_mon+1<<" "<<Time->tm_mday<<endl;
}