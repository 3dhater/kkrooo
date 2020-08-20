// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_EVENT_SYSTEM_H__
#define __KK_EVENT_SYSTEM_H__


class kkEventSystem
{
public:
	kkEventSystem(){}
	virtual ~kkEventSystem(){}

	virtual void clearEvents() = 0;
	virtual void addEvent( const kkEvent& event ) = 0;
	virtual bool pollEvent( kkEvent& event ) = 0;

	virtual void setEventConsumer( kkEventConsumer * ec ) = 0;
};

#endif