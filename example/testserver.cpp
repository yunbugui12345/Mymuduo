#include<muduo_mine/TcpServer.h>
#include<muduo_mine/Logger.h>

#include<string>
#include<functional>


class EchoServer
{
public:
	EchoServer(EventLoop* loop,
			const InetAddress&addr,
			const std::string name)
		: server_(loop,addr,name)
		, loop_(loop)
	{
		server_.setConnectionCallback(
			std::bind(&EchoServer::onConnection,this,std::placeholders::_1)
		);

		server_.setMessageCallback(
			std::bind(&EchoServer::onMessage,this,
			std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)
		);

		server_.setThreadNum(3);
	}

	void start()
	{
		server_.start();
	}

private:
	void onConnection(const TcpConnectionPtr& conn)
	{
		if(conn->connected())
		{
			LOG_INFO("Connection UP: %s",conn->peerAddress().ToIpPort().c_str());
		}
		else
		{
			LOG_INFO("Connection DONE: %s",conn->peerAddress().ToIpPort().c_str());
		}
	}

	void onMessage(const TcpConnectionPtr& conn,
				Buffer* buf,
				Timestamp time)
	{
		std::string msg=buf->retrieveAllAsString();
		conn->send(msg);
		conn->shutdown();
	}

	EventLoop* loop_;
	Tcpserver server_;
};

int main()
{
	EventLoop loop;
	InetAddress addr(8080);
	EchoServer server(&loop,addr,"EchoServer-01");
	server.start();
	loop.loop();

	//std::cout<<Timestamp::now().tostring()<<std::endl;

	return 0;
}