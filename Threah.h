#pragma once

#include "noncopyable.h"

#include <functional>
#include <thread>
#include <memory>
#include <unistd.h>
#include <string>
#include <atomic>

class Thread : noncopyable
{
public:
	using ThreadFunc = std::function<void()>; // 除了function之外,bind()绑定也要看

	explicit Thread(ThreadFunc, const std::string &name = std::string());	//允许不传名字
	~Thread();

	void start();
	void join();

	bool started() const { return started_; }
	pid_t tid() const { return tid_; }
	const std::string &name() const { return name_; }

	static int numCreated() { return numCreated_; }

private:
	void setDefaultName();

	bool started_;							//用于确定线程是否开始运行
	bool joined_;							//用于确定这个线程的join函数是否被调用过
	std::shared_ptr<std::thread> thread_;	//用智能指针管理生命周期
	pid_t tid_;
	ThreadFunc func_;
	std::string name_;						//线程的名字
	static std::atomic_int numCreated_;		//线程数量计数器
};