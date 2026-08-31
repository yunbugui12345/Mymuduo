#pragma once
/*
		作为父类
	1、让子类无法拷贝和赋值
	2、让子类正常构造和析构
*/

class noncopyable{
public:
	noncopyable(const noncopyable&)=delete;
	noncopyable& operator=(const noncopyable&)=delete;

protected:
	noncopyable()=default;
	~noncopyable()=default;
};