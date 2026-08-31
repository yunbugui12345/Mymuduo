#include<string.h>

#include"InetAddress.h"

InetAddress::InetAddress(uint16_t port,std::string ip)
{
	memset(&addr_,0,sizeof(addr_));
	addr_.sin_family=AF_INET;
	addr_.sin_port=htons(port);
	addr_.sin_addr.s_addr=inet_addr(ip.c_str());
}

std::string InetAddress::ToIp() const
{
	char buf[64]={0};
	//从addr_.sin_addr中读取网络字节序并转成用户字节序
	::inet_ntop(AF_INET,&addr_.sin_addr,buf,sizeof(buf));
	return buf;
}

std::string InetAddress::ToIpPort() const
{
	//ip : port
	char buf[64]={0};
	::inet_ntop(AF_INET,&addr_.sin_addr,buf,sizeof(buf));
	size_t end=strlen(buf);
	uint16_t port =ntohs(addr_.sin_port);
	sprintf(buf+end,": %u",port);//sprintf中不用填长度 snprintf要填长度
	return buf;
}

uint16_t InetAddress::ToPort() const
{
	return ntohs(addr_.sin_port);
}


/* #include<iostream>
int main(){
	InetAddress addr(8080);
	std::cout<<addr.ToIpPort()<<std::endl;

	return 0;
} */