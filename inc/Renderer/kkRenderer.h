// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_RENDERER_H__
#define __KK_RENDERER_H__

//#include "Classes/kkColor.h"
//#include "Classes/Strings/kkString.h"
//#include "GraphicsSystem/kkTexture.h"

#include "Classes/Containers/kkArray.h"

#include "Scene3D/kkScene3DObject.h"

struct kkImage;

struct kkRenderSettings
{
	bool multithreading     = true;
	bool antialiasing       = true;
	bool backgroundIsAlpha  = true;
	kkColor backgroundColor = kkColorBlack;
};

// Информация о сцене и кадре для рендеринга
struct kkRenderInfo
{
	kkImage*                   image   = nullptr;
	kkArray<kkScene3DObject*>* objects = nullptr;

	kkMatrix4 V;
	kkMatrix4 P;
	kkMatrix4 VP;
	kkMatrix4 VPInvert;

	bool needToStop = false; // если при рендеринге нажали на Stop то это значение будет true и нужно прекратить рисование
	bool isStopped = false; // когда рендеринг закончен, нужно утановить true/ при выходе из функции Render
	bool isStarted = false; // true когда запускается
	void resetStates()
	{
		needToStop = false;
		isStopped  = false;
		isStarted  = false;
	}

	kkRenderSettings* settings = nullptr;
};

// Класс для предоставления функций для визуализации объектов
// Реализация должна быть в плагине.
class kkRenderer
{
public:
	kkRenderer(){}
	virtual ~kkRenderer(){}

	virtual const char16_t* GetName() = 0;
	virtual void Render( kkRenderInfo* ) = 0;
};

#endif