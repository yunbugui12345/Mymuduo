#include "EventLoop.h"
#include "Logger.h"
#include "Poller.h"
#include "Channel.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <fcntl.h>
#include <error.h>
#include <memory>

__thread EventLoop *t_loopInThisThread = nullptr;

const int kPollTimeMs = 10000;

int createEventfd()
{
	int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (evtfd < 0)
	{
		LOG_FATAL("eventfd error:%d\n", errno);
	}
	return evtfd;
}

EventLoop::EventLoop()
	: looping_(false)
	, quit_(false)
	, callingPendingFunctors_(false)
	, threadId_(CurrentThread::tid())
	, poller_(Poller::newDefaultPoller(this))
	, wakeupfd_(createEventfd())
	, wakeupChannel_(new Channel(this, wakeupfd_))
{
	LOG_DEBUG("EventLoop create %p in thread %d\n", this, threadId_);
	if (t_loopInThisThread)
	{
		LOG_FATAL("Another EventLoop %p exits in this thread %d \n", t_loopInThisThread, threadId_);
	}
	else
	{
		t_loopInThisThread = this;
	}

	wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this)); // 绑定器，这个研究一下
	wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
	wakeupChannel_->disableAll();
	wakeupChannel_->remove();
	::close(wakeupfd_);
	t_loopInThisThread = nullptr;
}

void EventLoop::loop()
{
	looping_ = true;
	quit_ = false;

	LOG_INFO("EventLoop %p start looping \n", this);

	while (!quit_)
	{
		activeChannel_.clear();
		pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannel_);
		for (Channel *channel : activeChannel_)
		{
			channel->handleEvent(pollReturnTime_);
		}
		doPendingFunctors();
	}
	LOG_INFO("EventLoop %p stop looping. \n", this);
	looping_ = false;
}

void EventLoop::quit()
{
	quit_ = true;

	if (!isInLoopThread())
	{
		wakeup();
	}
}

void EventLoop::runInLoop(Functor cb)
{
	if (isInLoopThread())
	{
		cb();
	}
	else
	{
		queueInLoop(cb);
	}
}

void EventLoop::queueInLoop(Functor cb)
{
	{
		std::unique_lock<std::mutex> lock(mutex_);
		pendingFunctors_.emplace_back(cb);
	}

	if (!isInLoopThread() || callingPendingFunctors_)
	{
		wakeup();
	}
}

void EventLoop::handleRead()
{
	uint64_t one = 1;
	ssize_t n = read(wakeupfd_, &one, sizeof(one));
	if (n != sizeof(one))
	{
		LOG_ERROR("EventLoop::handleRead() reads %d bytes instead of 8", n);
	}
}

void EventLoop::wakeup()
{
	uint64_t one = 1;
	ssize_t n = write(wakeupfd_, &one, sizeof(one));
	if (n != sizeof(one))
	{
		LOG_ERROR("EventLoop::wakeup() writes %lu byte instead of 8 \n", n);
	}
}

void EventLoop::updateChannel(Channel *channel)
{
	poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel)
{
	poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel *channel)
{
	return poller_->hasChannel(channel);
}

void EventLoop::doPendingFunctors()
{
	std::vector<Functor> functors;
	callingPendingFunctors_=true;

	{
		std::unique_lock<std::mutex> lock(mutex_);
		functors.swap(pendingFunctors_);
	}

	for(const Functor &functors: functors)
	{
		functors();
	}
	
	callingPendingFunctors_=false;
}