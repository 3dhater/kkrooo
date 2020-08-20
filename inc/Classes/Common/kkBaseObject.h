// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKROO_BASE_OBJECT_H__
#define __KKROO_BASE_OBJECT_H__

class kkBaseObject
{
protected:
	kkBaseObject(){}
public:
	virtual ~kkBaseObject(){}
	virtual void _f(){}
};

#endif