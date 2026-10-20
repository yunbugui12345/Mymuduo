#pragma once

#include "Poller.h"

#include <vector>
#include <sys/epoll.h>

class Channel;

class EPollPoller : public Poller	//对epoll_create epoll_wait epoll_ctl的打包、处理Poller中的Channel以及更新poller中channel的状态
{
public:
	EPollPoller(EventLoop *loop);
	~EPollPoller() override;

	Timestamp poll(int timeoutMs, ChannelList *activeChannels) override;
	void updateChannel(Channel *channel) override;
	void removeChannel(Channel *channel) override;

private:
	static const int kInitEventListsize = 16;

	void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
	void update(int operation, Channel *channel);

	using EventList = std::vector<epoll_event>;

	int epollfd_;
	EventList events_;
};