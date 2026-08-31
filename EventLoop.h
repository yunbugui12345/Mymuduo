#pragma once

#include <functional>
#include <vector>
#include <atomic>
#include <memory>
#include <mutex>

#include "noncopyable.h"
#include "Timestamp.h"
#include "CurrentThread.h"

class Channel;
class Poller;

// 事件循环类 主要包含两大模块 Channel Poller
class EventLoop : noncopyable
{
public:
	using Functor = std::function<void()>;

	EventLoop();
	~EventLoop();

	void loop();
	void quit();

	Timestamp pollReturnTime() { return pollReturnTime_; }

	void runInLoop(Functor cb);
	void queueInLoop(Functor cb);

	void wakeup();

	void updateChannel(Channel *channel);
	void removeChannel(Channel *channel);
	bool hasChannel(Channel *channel);

	bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }


private:
	void handleRead();
	void doPendingFunctors();

	using ChannelList = std::vector<Channel *>;

	std::atomic_bool looping_;
	std::atomic_bool quit_;

	const pid_t threadId_;

	Timestamp pollReturnTime_;
	std::unique_ptr<Poller> poller_;

	int wakeupfd_;
	std::unique_ptr<Channel> wakeupChannel_;

	ChannelList activeChannel_;

	std::atomic_bool callingPendingFunctors_;
	std::vector<Functor> pendingFunctors_;
	std::mutex mutex_; // 互斥锁，用来保护上面vector线程的安全操作
};