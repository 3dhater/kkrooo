// SPDX-License-Identifier: GPL-3.0-only
#include "PluginSDK.h"

#include "Common.h"
#include "Renderer.h"

#include <vector>

void screenToClient(const v2i& screen_coord, const v4f& client_rect, v2i& out )
{
	out.x = screen_coord.x - (s32)client_rect.x;
	out.y = screen_coord.y - (s32)client_rect.y;
}

void getRay( kkRay& ray, const v2i& coord, const v4f& rc, const v2f& rc_sz, const kkMatrix4& VPinvert, f32 x_offset, f32 y_offset )
{
	v2i point; screenToClient(coord,rc,point);
	// координаты курсора от -1 до +1
	float pt_x = ((float)point.x / rc_sz.x) * 2.f - 1.f;
	float pt_y = - ((float)point.y / rc_sz.y) * 2.f + 1.f;

	//                                                                  0.f - for d3d
	ray.m_origin = math::mul(kkVector4(pt_x + x_offset,pt_y + y_offset,-1.f,1.f), VPinvert);
	ray.m_end   = math::mul(kkVector4(pt_x + x_offset,pt_y + y_offset,1.f,1.f), VPinvert);

	ray.m_origin.KK_W = 1.0f / ray.m_origin.KK_W;
	ray.m_origin.KK_X *= ray.m_origin.KK_W;
	ray.m_origin.KK_Y *= ray.m_origin.KK_W;
	ray.m_origin.KK_Z *= ray.m_origin.KK_W;

	ray.m_end.KK_W = 1.0f / ray.m_end.KK_W;
	ray.m_end.KK_X *= ray.m_end.KK_W;
	ray.m_end.KK_Y *= ray.m_end.KK_W;
	ray.m_end.KK_Z *= ray.m_end.KK_W;
}

Renderer::Renderer(){}
Renderer::~Renderer(){}

kkVector4 lightPos(1.3f,5.8f,2.18f,1.f);

void Renderer::RenderValidMaterial(const kkTriangleRayTestResult& tri, kkColor* currentPixel)
{
	kkColor pixelColor = kkColorBlack;
	kkColor pixelDiffuseColor = tri.material->GetDiffuseColor();

	if( tri.material->GetDiffuseTexture() )
	{
//		auto image = tri->material->GetDiffuseImage();
		auto DText = tri.material->GetDiffuseTexture();
		if( DText->m_image )
		{
			pixelDiffuseColor = DText->m_image->getPixelColorUV(tri.pointTcoord._f32[0],tri.pointTcoord._f32[1]);
		}
	}

	auto lightDir = lightPos;
	lightDir.normalize(true);

	float lightIntensity = math::clamp( lightDir.dot(tri.pointNormal), 0.f, 1.f);
	lightIntensity *= 1.2f;
	pixelDiffuseColor.m_data[ 0 ] *= lightIntensity;
	pixelDiffuseColor.m_data[ 1 ] *= lightIntensity;
	pixelDiffuseColor.m_data[ 2 ] *= lightIntensity;

	pixelDiffuseColor.m_data[ 0 ] = math::clamp( pixelDiffuseColor.m_data[ 0 ], 0.f, 1.f);
	pixelDiffuseColor.m_data[ 1 ] = math::clamp( pixelDiffuseColor.m_data[ 1 ], 0.f, 1.f);
	pixelDiffuseColor.m_data[ 2 ] = math::clamp( pixelDiffuseColor.m_data[ 2 ], 0.f, 1.f);
				

	auto oldpixelColor = pixelColor;

	pixelColor.m_data[ 0 ] = pixelDiffuseColor.m_data[0];
	pixelColor.m_data[ 1 ] = pixelDiffuseColor.m_data[1];
	pixelColor.m_data[ 2 ] = pixelDiffuseColor.m_data[2];
	/*pixelColor.m_data[ 0 ] += oldpixelColor.m_data[0] * ( 1.f - tri1->material->GetOpacity());
	pixelColor.m_data[ 1 ] += oldpixelColor.m_data[1] * ( 1.f - tri1->material->GetOpacity());
	pixelColor.m_data[ 2 ] += oldpixelColor.m_data[2] * ( 1.f - tri1->material->GetOpacity());
	pixelColor.m_data[ 0 ] = math::clamp( pixelColor.m_data[ 0 ], 0.f, 1.f);
	pixelColor.m_data[ 1 ] = math::clamp( pixelColor.m_data[ 1 ], 0.f, 1.f);
	pixelColor.m_data[ 2 ] = math::clamp( pixelColor.m_data[ 2 ], 0.f, 1.f);*/

	/*currentPixel->r = pixelColor.getAsByteRed();
	currentPixel->g = pixelColor.getAsByteGreen();
	currentPixel->b = pixelColor.getAsByteBlue();*/

	*currentPixel = pixelColor;
}

void Renderer::RenderNotValidMaterial(const kkTriangleRayTestResult& tri, kkColor* currentPixel)
{
	kkColor pixelColor = kkColorBlack;
	kkColor pixelDiffuseColor = tri.material->GetDiffuseColor();

	if( tri.material->GetDiffuseTexture() )
	{
//		auto image = tri->material->GetDiffuseImage();
		auto DText = tri.material->GetDiffuseTexture();
		if( DText->m_image )
		{
			pixelDiffuseColor = DText->m_image->getPixelColorUV(tri.pointTcoord._f32[0],tri.pointTcoord._f32[1]);
		}
	}

	auto lightDir = lightPos;
	lightDir.normalize(true);

	float lightIntensity = math::clamp( lightDir.dot(tri.pointNormal), 0.f, 1.f);
	lightIntensity *= 1.2f;
	pixelDiffuseColor.m_data[ 0 ] *= lightIntensity;
	pixelDiffuseColor.m_data[ 1 ] *= lightIntensity;
	pixelDiffuseColor.m_data[ 2 ] *= lightIntensity;

	pixelDiffuseColor.m_data[ 0 ] = math::clamp( pixelDiffuseColor.m_data[ 0 ], 0.f, 1.f);
	pixelDiffuseColor.m_data[ 1 ] = math::clamp( pixelDiffuseColor.m_data[ 1 ], 0.f, 1.f);
	pixelDiffuseColor.m_data[ 2 ] = math::clamp( pixelDiffuseColor.m_data[ 2 ], 0.f, 1.f);
				

	auto oldpixelColor = pixelColor;

	pixelColor.m_data[ 0 ] = pixelDiffuseColor.m_data[0];
	pixelColor.m_data[ 1 ] = pixelDiffuseColor.m_data[1];
	pixelColor.m_data[ 2 ] = pixelDiffuseColor.m_data[2];
	/*pixelColor.m_data[ 0 ] += oldpixelColor.m_data[0] * ( 1.f - tri1->material->GetOpacity());
	pixelColor.m_data[ 1 ] += oldpixelColor.m_data[1] * ( 1.f - tri1->material->GetOpacity());
	pixelColor.m_data[ 2 ] += oldpixelColor.m_data[2] * ( 1.f - tri1->material->GetOpacity());
	pixelColor.m_data[ 0 ] = math::clamp( pixelColor.m_data[ 0 ], 0.f, 1.f);
	pixelColor.m_data[ 1 ] = math::clamp( pixelColor.m_data[ 1 ], 0.f, 1.f);
	pixelColor.m_data[ 2 ] = math::clamp( pixelColor.m_data[ 2 ], 0.f, 1.f);*/

	/*currentPixel->r = pixelColor.getAsByteRed();
	currentPixel->g = pixelColor.getAsByteGreen();
	currentPixel->b = pixelColor.getAsByteBlue();*/
	*currentPixel = pixelColor;
}

const char16_t* Renderer::GetName()
{
	return u"Default";
}

struct ThreadInfo
{
	std::vector<pixel*> lines;
	std::vector<u32> h;
	bool run = true;
};
using threadRender_t = void(*)(Renderer * renderer, kkRenderInfo* renderInfo, ThreadInfo* ti);
void threadRender(Renderer * renderer, kkRenderInfo* renderInfo, ThreadInfo* ti)
{
	const int numSamples = 4;
	float rayOffset[numSamples][2]={
		0.f, 0.f, 
		0.f, 0.f, 
		0.f, 0.f, 
		0.f, 0.f
	};
	{
		//kkRay ray1;
		//kkRay ray2;
		//getRay(ray1,v2i((s32)0,0),v4f(0.f, 0.f, (float)renderInfo->image->m_width, (float)renderInfo->image->m_height),
		//	v2f((float)renderInfo->image->m_width, (float)renderInfo->image->m_height), renderInfo->VPInvert, 0.f, 0.f);
		//getRay(ray2,v2i((s32)1,0),v4f(0.f, 0.f, (float)renderInfo->image->m_width, (float)renderInfo->image->m_height),
		//	v2f((float)renderInfo->image->m_width, (float)renderInfo->image->m_height), renderInfo->VPInvert, 0.f, 0.f);
		//f32 len = ray1.m_origin.distance(ray2.m_origin); // расстояние между двумя лучами соседних пикселей
		//f32 offset = (len * 0.5f) - 0.1f; // расстояние от центра пикселя до его края с каким-то значением отступа
		f32 offset = 0.001f;
		rayOffset[0][0] = -offset;
		rayOffset[0][1] = -offset;

		rayOffset[1][0] = offset;
		rayOffset[1][1] = -offset;

		rayOffset[2][0] = -offset;
		rayOffset[2][1] = offset;

		rayOffset[3][0] = offset;
		rayOffset[3][1] = offset;
	}

	bool aa = false;
	if( renderInfo->settings )
	{
		aa = renderInfo->settings->antialiasing;
	}
	aa = false;
	for( size_t li = 0, sz = ti->lines.size(); li < sz; ++li )
	{
		s32 h = (s32)ti->h.at(li);
		pixel * currentPixel = ti->lines.at(li); 
		for( unsigned int w = 0; w < renderInfo->image->m_width; ++w )
		{
			int nos = numSamples;
			//if( aa )
			//{
			//
			//	currentPixel->r = 0;
			//	currentPixel->g = 0;
			//	currentPixel->b = 0;
			//	currentPixel->a = 0;

			//	if( renderInfo->settings )
			//	{
			//		currentPixel->r = renderInfo->settings->backgroundColor.m_data[0];
			//		currentPixel->g = renderInfo->settings->backgroundColor.m_data[1];
			//		currentPixel->b = renderInfo->settings->backgroundColor.m_data[2];
			//		/*if( renderInfo->settings->backgroundIsAlpha )
			//		{
			//			currentPixel->a = 0;
			//		}*/
			//	}

			//	//pixel pixel_all;
			//	kkColor color_all;
			//	color_all.setAlpha(0.f);
			//	for( int i = 0; i < numSamples; ++i )
			//	{
			//		kkRay ray;
			//		getRay(ray,v2i((s32)w,h),v4f(0.f, 0.f, (float)renderInfo->image->m_width, (float)renderInfo->image->m_height),
			//			v2f((float)renderInfo->image->m_width, (float)renderInfo->image->m_height), renderInfo->VPInvert, rayOffset[i][0], rayOffset[i][1]);
			//		
			//		/*ray.m_origin.setX(ray.m_origin.getX()+rayOffset[i][0]);
			//		ray.m_origin.setZ(ray.m_origin.getZ()+rayOffset[i][1]);
			//		ray.m_origin.setY(ray.m_origin.getY()+rayOffset[i][1]);*/

			//		ray.update();


			//		std::vector<kkTriangleRayTestResult> triangles;
			//		triangles.reserve(50);
			//		for( auto o : *renderInfo->objects )
			//		{
			//			o->RayTest( triangles, ray, o->GetMaterialImplementation() );
			//			//o->RayTestGrid( triangles, v2i(w, h), ray, o->GetMaterialImplementation() );
			//		}



			//		kkColor color;
			//		//color.setAlpha(0.f);

			//		if( triangles.size() )
			//		{
			//			std::sort( triangles.begin(),triangles.end(),
			//				[](const kkTriangleRayTestResult& first, const kkTriangleRayTestResult& second)
			//			{
			//				return first.length > second.length;
			//			} );

			//			pixel p;
			//			for( size_t ti = 0, tisz = triangles.size(); ti < tisz; ++ti )
			//			{
			//				auto & tri = triangles[ ti ];
			//				if( tri.material )
			//				{
			//			
			//					if( IsValidMaterial( tri.material->GetPluginID() ) )
			//					{
			//						auto pluginID = tri.material->GetPluginID();
			//						StandartRendererIDStruct * ids = (StandartRendererIDStruct*)pluginID.m_data;
			//						switch(ids->materialType)
			//						{
			//						case MaterialType::Toon:
			//						case MaterialType::Default:
			//						default:
			//							//printf("valid material\n");
			//							renderer->RenderValidMaterial(tri, &color);
			//							break;
			//						}
			//					}
			//					else
			//					{
			//						//printf("not valid material\n");
			//						renderer->RenderNotValidMaterial(tri, &color);
			//					}
			//				}
			//			}

			//			//pixel_all = p;
			//		}
			//		else
			//		{
			//			color = renderInfo->settings->backgroundColor;
			//			//pixel_all = *currentPixel;
			//			if( renderInfo->settings )
			//			{
			//				if( renderInfo->settings->backgroundIsAlpha )
			//				{
			//					//currentPixel->a = 0;
			//					color.setAlpha(0.f);
			//					color.setRed(0.f);
			//					color.setBlue(0.f);
			//					color.setGreen(0.f);
			//					--nos;
			//					if(!nos)
			//						nos = 1;
			//				}
			//			}
			//		}

			//		color_all += color;
			//	}
			//	//*currentPixel = pixel_all;// / 4;
			//	color_all *= 1.f/(f32)nos;
			//	currentPixel->fromColor(color_all);
			//}
			//else
			{
				kkRay ray;
				getRay(ray,v2i((s32)w,h),v4f(0.f, 0.f, (float)renderInfo->image->m_width, (float)renderInfo->image->m_height),
					v2f((float)renderInfo->image->m_width, (float)renderInfo->image->m_height), renderInfo->VPInvert, 0.f, 0.f);
				ray.update();
			
				std::vector<kkTriangleRayTestResult> triangles;
				triangles.reserve(50);

				for( auto o : *renderInfo->objects )
				{
					o->RayTest( triangles, ray, o->GetMaterialImplementation() );
					//o->RayTestGrid( triangles, v2i(w, h), ray, o->GetMaterialImplementation() );
				}

				currentPixel->r = 0;
				currentPixel->g = 0;
				currentPixel->b = 0;
				currentPixel->a = 255;

				if( renderInfo->settings )
				{
					currentPixel->r = renderInfo->settings->backgroundColor.m_data[0];
					currentPixel->g = renderInfo->settings->backgroundColor.m_data[1];
					currentPixel->b = renderInfo->settings->backgroundColor.m_data[2];
				}

				kkColor color;
				if( triangles.size() )
				{
					std::sort( triangles.begin(),triangles.end(),
						[](const kkTriangleRayTestResult& first, const kkTriangleRayTestResult& second)
					{
						return first.length > second.length;
					} );

					for( size_t ti = 0, tisz = triangles.size(); ti < tisz; ++ti )
					{
						auto & tri = triangles[ ti ];
						if( tri.material )
						{
						
							if( IsValidMaterial( tri.material->GetPluginID() ) )
							{
								auto pluginID = tri.material->GetPluginID();
								StandartRendererIDStruct * ids = (StandartRendererIDStruct*)pluginID.m_data;
								switch(ids->materialType)
								{
								case MaterialType::Toon:
								case MaterialType::Default:
								default:
									//printf("valid material\n");
									renderer->RenderValidMaterial(tri, &color);
									break;
								}
							}
							else
							{
								//printf("not valid material\n");
								renderer->RenderNotValidMaterial(tri, &color);
							}
						}
					}
				}
				else
				{
					if( renderInfo->settings )
					{
						if( renderInfo->settings->backgroundIsAlpha )
						{
							currentPixel->a = 0;
						}
					}
				}
				currentPixel->fromColor(color);
			}


			++currentPixel; 
		}
		if( renderInfo->needToStop )
			break;
	}

	ti->run = false;
}




struct Threads
{
	Threads(u32 numOfThreads, threadRender_t f, Renderer * renderer, kkRenderInfo* renderInfo, ThreadInfo* lines )
	{
		for( u32 i = 0; i < numOfThreads; ++i )
		{
			threads.push_back( new std::thread(f, renderer, renderInfo, &lines[i] ) );
		}
	}
	~Threads()
	{
		join();
		for( auto t : threads )
		{
			delete t;
		}
	}

	void join()
	{
		for( auto t : threads )
		{
			t->join();
		}
	}

	std::vector<std::thread*> threads;
};

void Renderer::Render( kkRenderInfo* renderInfo )
{
	//printf("threads [%u]\n", std::thread::hardware_concurrency());

	auto numOfThreads = std::thread::hardware_concurrency() * 2;
	if(!numOfThreads)
		numOfThreads = 1;

	if( renderInfo->settings )
	{
		if( !renderInfo->settings->multithreading )
			numOfThreads = 1;
	}

	u8* data = renderInfo->image->m_data8;

	// Build BHV
	for( u64 i = 0, sz = renderInfo->objects->size(); i < sz; ++i )
		renderInfo->objects->at(i)->PrepareForRaytracing(renderInfo);
	
	ThreadInfo* threadsInfo = new ThreadInfo[numOfThreads];
	u32 threadIndex = 0;
	for( u32 h = 0; h < renderInfo->image->m_height; ++h )
	{
		threadsInfo[ threadIndex ].lines.push_back((pixel*)data);
		threadsInfo[ threadIndex ].h.push_back(h);
		data += renderInfo->image->m_pitch;
		++threadIndex;
		if( threadIndex == numOfThreads )
			threadIndex = 0;
	}

	Threads threads(numOfThreads, threadRender, this, renderInfo, threadsInfo);
	while(true)
	{
		bool stop = true;
		for( u32 i = 0; i < numOfThreads; ++i )
		{
			if( threadsInfo[i].run )
				stop = false;
		}

		if( stop )
			break;
	}

	delete[] threadsInfo;

	for( auto o : *renderInfo->objects )
		o->FinishRaytracing();

	renderInfo->isStopped = true;
}