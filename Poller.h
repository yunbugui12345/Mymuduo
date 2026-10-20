#pragma once

#include "noncopyable.h"
#include "Timestamp.h"

#include <vector>
#include <unordered_map>

class EventLoop;
class Channel;

class Poller : noncopyable
{
public:
	using ChannelList = std::vector<Channel *>;

	Poller(EventLoop *Loop);
	virtual ~Poller() = default;

	virtual Timestamp poll(int timeout, ChannelList *activeChannels) = 0;	//纯虚函数
	virtual void updateChannel(Channel *channel) = 0;
	virtual void removeChannel(Channel *channel) = 0;

	bool hasChannel(Channel *channel) const;		//插入哈希表

	static Poller *newDefaultPoller(EventLoop *loop);

protected:
	using ChannelMap = std::unordered_map<int, Channel *>;	// <fd,channel*>
	ChannelMap channels_;

private:
	EventLoop *ownerLoop_;
};