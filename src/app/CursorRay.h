#ifndef __CURSOR_RAY_H__
#define __CURSOR_RAY_H__

#include "Classes/Math/kkRay.h"

struct CursorRay
{
	kkRay m_center;
	kkRay m_N;
	kkRay m_S;
	kkRay m_E;
	kkRay m_W;
};


#endif