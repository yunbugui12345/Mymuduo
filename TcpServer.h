#pragma once

#include "EventLoop.h"
#include "Acceptor.h"
#include "InetAddress.h"
#include "noncopyable.h"
#include "EventLoopThreadPool.h"
#include "Callbacks.h"

#include <functional>
#include <string>
#include <memory>
#include <atomic>
#include <unordered_map>

// 对外的服务器编程使用的类，换句话说就是整个项目最重要的类
class Tcpserver : noncopyable
{
public:
	using ThreadInitCallback = std::function<void(EventLoop *)>;

	enum Option
	{
		kNoReusePort,
		kReusePort,
	};

	Tcpserver(EventLoop *loop, const InetAddress &listenAddr, const std::string &namearg, Option option = kNoReusePort);
	~Tcpserver();

	void setThreadInitCallback(const ThreadInitCallback &cb) { threadInitCallback_ = cb; }
	void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }
	void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
	void setWriteCompleteCallback(const WriteCompleteCallback &cb) { writeCompleteCallback_ = cb; }

	void setThreadNum(int numThreads);

	void start();

private:
	void newConnection(int sockfd, const InetAddress &peeraddr);
	void removeConnection(const TcpConnectionPtr &conn);
	void removeConnectionInLoop(const TcpConnectionPtr &conn);

	using ConnectionMap = std::unordered_map<std::string, TcpConnectionPtr>;

	EventLoop *loop_;
	const std::string ipPort_;
	const std::string name_;
	std::unique_ptr<Acceptor> acceptor_;
	std::shared_ptr<EventLoopThreadPool> threadPool_;

	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	WriteCompleteCallback writeCompleteCallback_;

	ThreadInitCallback threadInitCallback_;
	std::atomic_int started_;

	int nextConnId_;
	ConnectionMap connections_;
};