#include "CurrentThread.h"

namespace CurrentThread
{
	__thread int t_cachedTid = 0;

	void cacheTid()
	{
		if (t_cachedTid == 0)
		{
			t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));	//用static_cast<T>是因为这个仅能做相关类型的转换，其他转换会报错
		}
	}

}