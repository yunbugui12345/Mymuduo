#pragma once

#include <unistd.h>
#include <sys/syscall.h>

namespace CurrentThread	//用命名空间的原因是一方面避免类里面的成员与其他地方命名相同，另一方面其他地方想调用就可以直接调用无需实例化对象
{
	extern __thread int t_cachedTid;

	void cacheTid();

	inline int tid()
	{
		if (__builtin_expect(t_cachedTid == 0, 0))//此处完全可以改成if( t_cachedTid==0 )
		{
			cacheTid();
		}
		return t_cachedTid;
	}
}