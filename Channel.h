#pragma once

#include "noncopyable.h"
#include "Timestamp.h"

#include <functional>
#include <memory>

class EventLoop; // 提前声明

class Channel : noncopyable		//用fd和Eventloop* loop构造的一个打包好的通道
{
public:
	using EventCallback = std::function<void()>;
	using ReadEventCallback = std::function<void(Timestamp)>;

	Channel(EventLoop *loop, int fd);							// 1
	~Channel();													// 2

	void handleEvent(Timestamp receiveTime);					// 3

	void setReadCallback(ReadEventCallback cb) { readCallback_ = std::move(cb); }
	void setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }
	void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }
	void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }

	// 防止channel被手动remove后channel还在执行回调操作
	void tie(const std::shared_ptr<void> &);					// 4

	int fd() const { return fd_; }
	int events() const { return events_; }
	void set_revents(int reve) { revents_ = reve; }

	void enableReading()
	{
		events_ |= kReadEvent;
		update();
	}
	void disableReading()
	{
		events_ &= ~kNoneEvent;
		update();
	}
	void enableWriting()
	{
		events_ |= kWriteEvent;
		update();
	}
	void disableWriting()
	{
		events_ &= ~kWriteEvent;
		update();
	}
	void disableAll()
	{
		events_ = kNoneEvent;
		update();
	}
	bool isWriting() const { return events_ & kWriteEvent; }
	bool isReading() const { return events_ & kReadEvent; }
	bool isNoneEvent() const { return events_ == kNoneEvent; }

	int index() { return index_; }
	void set_index(int idx) { index_ = idx; }

	EventLoop *ownerLoop() { return loop_; }
	void remove();										// 5

private:
	void update();										// 6
	void handleEventWithGuard(Timestamp receiveTime);	// 7

	static const int kNoneEvent;
	static const int kReadEvent;
	static const int kWriteEvent;

	EventLoop *loop_;
	const int fd_;
	int events_;
	int revents_;			//用于记录读写事件（就像是linux中读写运行权限一样）
	int index_;				//用于记录channel在epoll里的状态：-1 未进epoll 1 在epoll中 -1 已被epoll移除

	std::weak_ptr<void> tie_;	//用于监管生命周期（Tcpconnection）
	bool tied_;

	ReadEventCallback readCallback_;
	EventCallback writeCallback_;
	EventCallback closeCallback_;
	EventCallback errorCallback_;
};