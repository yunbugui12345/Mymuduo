#include "TcpServer.h"
#include "Logger.h"
#include "TcpConnection.h"

#include <functional>
#include <strings.h>

static EventLoop *CheckLoopNotNull(EventLoop *loop)
{
	if (loop == nullptr)
	{
		LOG_FATAL("%s:%s:%d mainLoop is null \n", __FILE__, __FUNCTION__, __LINE__);
	}
	return loop;
}

Tcpserver::Tcpserver(EventLoop *loop,
					 const InetAddress &listenAddr,
					 const std::string &namearg,
					 Option option)
	: loop_(CheckLoopNotNull(loop)), ipPort_(listenAddr.ToIpPort()), name_(namearg), acceptor_(new Acceptor(loop, listenAddr, option == kReusePort)), threadPool_(new EventLoopThreadPool(loop, name_)), connectionCallback_(), messageCallback_(), nextConnId_(1), started_(0)
{
	acceptor_->setNewConnectionCallback(std::bind(&Tcpserver::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

Tcpserver::~Tcpserver()
{
	for(auto &item : connections_)
	{
		TcpConnectionPtr conn(item.second);
		item.second.reset();

		conn->getLoop()->runInLoop(
			std::bind(&TcpConnection::connectionDestoryed,conn)
		);
	}
}

void Tcpserver::setThreadNum(int numThreads)
{
	threadPool_->setThreadNum(numThreads);
}

void Tcpserver::start()
{
	if (started_++ == 0)
	{
		threadPool_->start(threadInitCallback_);
		loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
	}
}

void Tcpserver::newConnection(int sockfd, const InetAddress &peeraddr)
{
	EventLoop *ioLoop = threadPool_->getNextLoop();
	char buf[64] = {0};
	snprintf(buf, sizeof(buf), "-%s#%d", ipPort_.c_str(), nextConnId_);
	++nextConnId_;
	std::string connName = name_ + buf;

	LOG_INFO("TcpServer::neConnextion [%s] - new connection [%s] form %s \n",
			 name_.c_str(), connName.c_str(), peeraddr.ToIpPort().c_str());

	sockaddr_in local;
	::bzero(&local, sizeof(local));
	socklen_t addrlen = sizeof(local);
	if (::getsockname(sockfd, (sockaddr *)&local, &addrlen) < 0)
	{
		LOG_ERROR("sockets::getLocalAddr");
	}
	InetAddress localAddr(local);

	TcpConnectionPtr conn(new TcpConnection(ioLoop, connName, sockfd, localAddr, peeraddr));
	connections_[connName] = conn;
	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->setWriteCompleteCallback(writeCompleteCallback_);

	conn->setCloseCallback(
		std::bind(&Tcpserver::removeConnection,this,std::placeholders::_1)
	);
	ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished,conn));
}

void Tcpserver::removeConnection(const TcpConnectionPtr &conn)
{
	loop_->runInLoop(
		std::bind(&Tcpserver::removeConnectionInLoop,this,conn)
	);
}

void Tcpserver::removeConnectionInLoop(const TcpConnectionPtr &conn)
{
	LOG_INFO("Tcpserver::removeConnectionInLoop [%s] - connection %s \n",name_.c_str(),conn->name().c_str());

	size_t n=connections_.erase(conn->name());
	EventLoop *ioLoop=conn->getLoop();
	ioLoop->queueInLoop(
		std::bind(&TcpConnection::connectionDestoryed,conn)
	);
}

