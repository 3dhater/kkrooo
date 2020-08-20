// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKROO__SINGLETON_H__
#define __KKROO__SINGLETON_H__

template<typename type>
class kkSingleton
{
	kkSingleton()
	{
	}
public:

	~kkSingleton()
	{
	}

	static type * s_instance;

};

#endif