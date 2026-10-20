#pragma once
/*
		作为父类
	1、让子类无法拷贝和赋值
	2、让子类正常构造和析构
*/

class noncopyable{
public:
	noncopyable(const noncopyable&)=delete;				//拷贝构造禁用
	noncopyable& operator=(const noncopyable&)=delete;	//=赋值禁用				A a; A b=a;

protected:
	noncopyable()=default;
	~noncopyable()=default;
};
