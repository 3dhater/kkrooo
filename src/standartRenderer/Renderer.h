// SPDX-License-Identifier: GPL-3.0-only
#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "PluginSDK.h"

class Renderer : public kkRenderer
{
public:
	Renderer();
	virtual ~Renderer();

	const char16_t* GetName();
	void Render( kkRenderInfo* );

	/*void RenderNotValidMaterial(const kkTriangleRayTestResult& tri, pixel  * );
	void RenderValidMaterial(const kkTriangleRayTestResult& tri, pixel* );*/
	void RenderNotValidMaterial(const kkTriangleRayTestResult& tri, kkColor* );
	void RenderValidMaterial(const kkTriangleRayTestResult& tri, kkColor* );

};


#endif