#include "kkrooo.engine.h"
#include "Common.h"

#include "Classes/Math/kkVector4.h"
#include "Classes/Math/kkRay.h"
#include "Classes/Math/kkMath.h"
#include "Functions.h"
#include "Viewport/Viewport.h"
#include "Application.h"

#include <math.h>

/*
I am very tired of OOP style
*/

bool g_cursorInViewport = false;
void kkCursorInViewport(bool v)
{
	g_cursorInViewport = v;
}
bool kkIsCursorInViewport()
{
	return g_cursorInViewport;
}
void kkGSDrawModelEdge(kkMesh* mesh,const kkMatrix4& mat, const kkColor& edgeCol)
{
    return kkSingleton<Application>::s_instance->GSDrawModelEdge(mesh, mat, edgeCol);
}
void kkGSDrawModel(kkMesh* mesh, const kkMatrix4& mat, const kkColor& difCol, kkImageContainerNode* m_diffTex)
{
    return kkSingleton<Application>::s_instance->GSDrawModel(mesh, mat, difCol, m_diffTex);
}
void kkGSDrawObb( const kkObb& obb, const kkColor& color)
{
    return kkSingleton<Application>::s_instance->GSDrawObb(obb, color);
}
void kkGSDrawAabb( const kkAabb& aabb, const kkColor& color)
{
    return kkSingleton<Application>::s_instance->GSDrawAabb(aabb, color);
}
bool * kkGetGlobalInputBlock()
{
    return kkSingleton<Application>::s_instance->GetGlobalInputBlock();
}
AppState_main* kkGetAppState_main()
{
    return kkSingleton<Application>::s_instance->GetAppState_main();
}
AppState_keyboard* kkGetAppState_keyboard()
{
    return kkSingleton<Application>::s_instance->GetAppState_keyboard();
}
v2i* kkGetCursorPosition()
{
    return kkSingleton<Application>::s_instance->GetCursorPosition();
}
ShortcutManager* kkGetShortcutManager()
{
    return kkSingleton<Application>::s_instance->GetShortcutManager();
}
bool kkIsKeyDown(kkKey k)
{
    return kkSingleton<Application>::s_instance->IsKeyDown(k);
}
void* kkGetGUI()
{
    return kkSingleton<Application>::s_instance->GetGUI();
}
void kkDrawAll()
{
    return kkSingleton<Application>::s_instance->DrawAllEvent();
}
Scene3D** kkGetScene3D()
{
    return kkSingleton<Application>::s_instance->getScene3D();
}

kkGraphicsSystem* kkGetGS()
{
    return kkSingleton<Application>::s_instance->getGS();
}

void kkGSSetDepth(bool v)
{
	kkSingleton<Application>::s_instance->GSSetDepth(v);
}

void kkGSSetViewport(s32 x, s32 y, s32 z, s32 w)
{
	kkSingleton<Application>::s_instance->GSSetViewport(x,y,z,w);
}

v2i  kkGetWindowClientSize()
{
	return kkSingleton<Application>::s_instance->getWindowClientSize();
}

bool kkIsLmbDownOnce(){return kkSingleton<Application>::s_instance->IsLmbDownOnce();}
bool kkIsLmbDown(){return kkSingleton<Application>::s_instance->IsLmbDown();}
bool kkIsLmbUp(){return kkSingleton<Application>::s_instance->IsLmbUp();}
bool kkIsRmbDownOnce(){return kkSingleton<Application>::s_instance->IsRmbDownOnce();}
bool kkIsRmbDown(){return kkSingleton<Application>::s_instance->IsRmbDown();}
bool kkIsRmbUp(){return kkSingleton<Application>::s_instance->IsRmbUp();}
bool kkIsMmbDownOnce(){return kkSingleton<Application>::s_instance->IsMmbDownOnce();}
bool kkIsMmbDown(){return kkSingleton<Application>::s_instance->IsMmbDown();}
bool kkIsMmbUp(){return kkSingleton<Application>::s_instance->IsMmbUp();}

namespace kkrooo
{
	
	const char16_t*   getIconFontString(IconFontSymbol s)
	{
		switch (s)
		{
		case IconFontSymbol::NextMenu: return u"0";
		case IconFontSymbol::NewObject: return u"1";
		case IconFontSymbol::EditObject: return u"2";
		case IconFontSymbol::EditUV: return u"3";
		case IconFontSymbol::ExpandCategory: return u"4";
		case IconFontSymbol::CollapseCategory: return u"5";
		case IconFontSymbol::CheckBoxCheck: return u"6";
		case IconFontSymbol::CheckBoxUncheck: return u"7";

		case IconFontSymbol::Cross1: return u"A";
		case IconFontSymbol::Add1:   return u"B";
		case IconFontSymbol::Sub1:   return u"C";
		case IconFontSymbol::Eq1:    return u"D";
		case IconFontSymbol::ArrowR1:    return u"E";
		case IconFontSymbol::ArrowT1:    return u"F";
		case IconFontSymbol::ArrowL1:    return u"G";
		case IconFontSymbol::ArrowB1:    return u"H";
		case IconFontSymbol::PushCircle: return u"I";
		case IconFontSymbol::AssignMaterial: return u"J";
		case IconFontSymbol::Save: return u"K";
		case IconFontSymbol::ClearOutputImage: return u"L";
		case IconFontSymbol::Undo: return u"M";
		case IconFontSymbol::Redo: return u"N";
		case IconFontSymbol::SelectByName: return u"O";
		case IconFontSymbol::SelectModeSelect: return u"P";
		case IconFontSymbol::ActiveButtonLTB: return u"Q";
		case IconFontSymbol::TransformMove: return u"R";
		case IconFontSymbol::TransformRotate: return u"S";
		case IconFontSymbol::TransformScale: return u"T";
		case IconFontSymbol::EditVertex: return u"U";
		case IconFontSymbol::EditEdge: return u"V";
		case IconFontSymbol::EditFace: return u"W";
		default:
			break;
		}
		return u"A";
	}
	char16_t   getIconFontChar(IconFontSymbol s)
	{
		return getIconFontString(s)[0];
	}
	bool pointInRect( const v2i& pt, const v4f& rect ){if( pt.x > rect.x ){if( pt.x < rect.z ){if( pt.y > rect.y ){if( pt.y < rect.w ){return true;}}}}return false;}
	bool pointInRect( const v2i& pt, const v4i& rect ){if( pt.x > rect.x ){if( pt.x < rect.z ){if( pt.y > rect.y ){if( pt.y < rect.w ){return true;}}}}return false;}

	void screenToClient(const v2i& screen_coord, const v4f& client_rect, v2i& out )
	{
		out.x = screen_coord.x - (s32)client_rect.x;
		out.y = screen_coord.y - (s32)client_rect.y;
	}

	kkVector4 screenToWorld( const v2i& coord, const v4f& rc, kkCamera* camera, f32 Z )
	{
		v2i point; screenToClient(coord,rc,point);
		kkVector4 v;
		if( camera )
		{
			auto P = camera->getProjectionMatrix();
			auto V = camera->getViewMatrix();
			auto PV = P * V;
			PV.invert();
			kkVector4 coords;
			coords.KK_X = (2.0f * ((float)(point.x) / (rc.z - rc.x))) - 1.0f;
			coords.KK_Y = 1.0f  - (2.0f * ((float)(point.y) / (rc.w - rc.y)));
			coords.KK_Z = Z;
			coords.KK_W = 1.0;

			v = math::mul( coords, PV );
			
			v.KK_W = 1.0f / v.KK_W;
			v.KK_X *= v.KK_W;
			v.KK_Y *= v.KK_W;
			v.KK_Z *= v.KK_W;
		}

		return v;
	}

	// Модифицированная функция взятая из Bullet Physics
	//kkVector4 getRayTo( const v2i& coord, const v4f& rc, kkCamera* camera, const kkVector4& target )
	//{
	//	auto point = screenToClient(coord,rc);
	//	auto wsz= rc.getWidthAndHeight();

	//	auto V = camera->getViewMatrix();
	//	//V.invert();

	//	float top = 1.f;
	//	float bottom = -1.f;
	//	float nearPlane = 1.f;
	//	float tanFov = (top - bottom) * 0.5f / nearPlane;
	//	float fov = camera->getFOV();

	//	kkVector4 camPos, camTarget;
	//	camPos    = -camera->getPositionInSpace();
	//	camTarget = target;

	//	kkVector4 rayFrom = camPos;
	//	kkVector4 rayForward = (camTarget - camPos);
	//	rayForward.normalize();
	//	float farPlane = camera->getFar();
	//	rayForward *= farPlane;

	//	kkVector4 rightOffset;
	//	kkVector4 cameraUp = kkVector4(0, 1, 0);

	//	kkVector4 vertical = cameraUp;
	//	vertical.KK_W = 1.f;
	//	vertical = math::mul( vertical, V);

	//	kkVector4 hor;
	//	hor = rayForward.cross(vertical);
	//	hor.normalize();
	//	vertical = hor.cross(rayForward);
	//	vertical.normalize();

	//	float tanfov = tanf(0.5f * fov);

	//	hor *= 2.f * farPlane * tanfov;
	//	vertical *= 2.f * farPlane * tanfov;

	//	float aspect;
	//	float width = wsz.x;
	//	float height = wsz.y;

	//	aspect = width / height;

	//	hor *= aspect;

	//	kkVector4 rayToCenter = rayFrom + rayForward;
	//	kkVector4 dHor = hor * 1.f / width;
	//	kkVector4 dVert = vertical * 1.f / height;
	//	
	//	kkVector4 rayTo = rayToCenter - kkVector4(0.5f) * hor + kkVector4(0.5f) * vertical;
	//	rayTo += float(point.x) * dHor;
	//	rayTo -= float(point.y) * dVert;

	//	return rayTo;
	//}


	// думаю можно как-то модифицировать эту функцию для работы в perspective
	void getRay_ortho( kkRay& ray, const v2i& coord, const v4f& rc, kkCamera* camera )
	{
		v2i point; screenToClient(coord,rc,point);
		auto wsz    = rc.getWidthAndHeight();
		auto camPos = camera->getPositionInSpace();

		// координаты курсора от -1 до +1
		float pt_x = ((float)point.x / wsz.x) * 2.f - 1.f;
		float pt_y = - ((float)point.y / wsz.y) * 2.f + 1.f;

		auto P = camera->getProjectionMatrix();
		P.invert();
		auto V = camera->getViewMatrix();
		V.invert();
		auto VP = V * P;

		//                                           0.f - for d3d
		ray.m_origin = math::mul(kkVector4(pt_x,pt_y,1.f,1.f), VP);
		ray.m_end   = math::mul(kkVector4(pt_x,pt_y,-1.f,1.f), VP);

		ray.m_origin.KK_W = 1.0f / ray.m_origin.KK_W;
		ray.m_origin.KK_X *= ray.m_origin.KK_W;
		ray.m_origin.KK_Y *= ray.m_origin.KK_W;
		ray.m_origin.KK_Z *= ray.m_origin.KK_W;

		ray.m_end.KK_W = 1.0f / ray.m_end.KK_W;
		ray.m_end.KK_X *= ray.m_end.KK_W;
		ray.m_end.KK_Y *= ray.m_end.KK_W;
		ray.m_end.KK_Z *= ray.m_end.KK_W;
	}

	void getRay( kkRay& ray, const v2i& coord, const v4f& rc, kkCamera* camera )
	{
		v2i point; screenToClient(coord,rc,point);
		auto wsz    = rc.getWidthAndHeight();

		// координаты курсора от -1 до +1
		float pt_x = ((float)point.x / wsz.x) * 2.f - 1.f;
		float pt_y = - ((float)point.y / wsz.y) * 2.f + 1.f;

		auto P = camera->getProjectionMatrix();
		P.invert();
		auto V = camera->getViewMatrix();
		V.invert();
		auto VP = V * P;

		//                                           0.f - for d3d
		ray.m_origin = math::mul(kkVector4(pt_x,pt_y,-1.f,1.f), VP);
		ray.m_end   = math::mul(kkVector4(pt_x,pt_y,1.f,1.f), VP);

		ray.m_origin.KK_W = 1.0f / ray.m_origin.KK_W;
		ray.m_origin.KK_X *= ray.m_origin.KK_W;
		ray.m_origin.KK_Y *= ray.m_origin.KK_W;
		ray.m_origin.KK_Z *= ray.m_origin.KK_W;

		ray.m_end.KK_W = 1.0f / ray.m_end.KK_W;
		ray.m_end.KK_X *= ray.m_end.KK_W;
		ray.m_end.KK_Y *= ray.m_end.KK_W;
		ray.m_end.KK_Z *= ray.m_end.KK_W;
	}
	void getRay( kkRay& ray, const v2i& coord, const v4f& rc, const v2f& rc_sz, const kkMatrix4& VPinvert )
	{
		v2i point; screenToClient(coord,rc,point);
		// координаты курсора от -1 до +1
		float pt_x = ((float)point.x / rc_sz.x) * 2.f - 1.f;
		float pt_y = - ((float)point.y / rc_sz.y) * 2.f + 1.f;

		//                                           0.f - for d3d
		ray.m_origin = math::mul(kkVector4(pt_x,pt_y,-1.f,1.f), VPinvert);
		ray.m_end   = math::mul(kkVector4(pt_x,pt_y,1.f,1.f), VPinvert);

		ray.m_origin.KK_W = 1.0f / ray.m_origin.KK_W;
		ray.m_origin.KK_X *= ray.m_origin.KK_W;
		ray.m_origin.KK_Y *= ray.m_origin.KK_W;
		ray.m_origin.KK_Z *= ray.m_origin.KK_W;

		ray.m_end.KK_W = 1.0f / ray.m_end.KK_W;
		ray.m_end.KK_X *= ray.m_end.KK_W;
		ray.m_end.KK_Y *= ray.m_end.KK_W;
		ray.m_end.KK_Z *= ray.m_end.KK_W;
	}

	bool rayTriangleIntersection( const kkVector4& rayOrigin, const kkVector4& rayDir, const kkVector4& v0, const kkVector4& v1, const kkVector4& v2, kkVector4& out_intersectionPoint, f32& out_len )
	{
		kkVector4 e1 = v1 - v0;
		kkVector4 e2 = v2 - v0;
		
		kkVector4  pvec;
		rayDir.cross(e2, pvec);
		f32 det  = e1.dot(pvec);

		
		if( det < 0.0000001f && det > -0.0000001f ) return false;

		kkVector4 tvec = rayOrigin - v0;

		f32 inv_det = 1.f / det;
		f32 u       = tvec.dot(pvec) * inv_det;

		if( u < 0.f || u > 1.f ) return false;

		kkVector4  qvec;
		tvec.cross(e1, qvec);
		f32 v    = rayDir.dot(qvec) * inv_det;
		
		if( v < 0.f || u + v > 1.f ) return false;

		/*out_len = e2.dot(qvec) * inv_det;
		
		if( out_len > 0.0000001f )
		{
			out_intersectionPoint = ray.m_begin + rayDir * out_len;
			return true;
		}*/

		return true;
	}


	bool rayIntersection_obb( const kkRay& ray, const kkObb& obb )
	{
		static kkVector4 ip;
		static f32 l;
		kkVector4 rayDir = ray.m_end - ray.m_origin;
		rayDir.normalize();

		if( kkrooo::rayTriangleIntersection(ray.m_origin,rayDir, obb.v1, obb.v8, obb.v4, ip, l  ) ) return true;
		if( kkrooo::rayTriangleIntersection(ray.m_origin,rayDir, obb.v1, obb.v5, obb.v8, ip, l  ) ) return true;
		if( kkrooo::rayTriangleIntersection(ray.m_origin,rayDir, obb.v7, obb.v2, obb.v6, ip, l  ) ) return true;
		if( kkrooo::rayTriangleIntersection(ray.m_origin,rayDir, obb.v7, obb.v6, obb.v3, ip, l  ) ) return true;
		if( kkrooo::rayTriangleIntersection(ray.m_origin,rayDir, obb.v3, obb.v5, obb.v1, ip, l  ) ) return true;
		if( kkrooo::rayTriangleIntersection(ray.m_origin,rayDir, obb.v3, obb.v6, obb.v5, ip, l  ) ) return true;
		if( kkrooo::rayTriangleIntersection(ray.m_origin,rayDir, obb.v4, obb.v8, obb.v2, ip, l  ) ) return true;
		if( kkrooo::rayTriangleIntersection(ray.m_origin,rayDir, obb.v4, obb.v2, obb.v7, ip, l  ) ) return true;
		if( kkrooo::rayTriangleIntersection(ray.m_origin,rayDir, obb.v1, obb.v4, obb.v7, ip, l  ) ) return true;
		if( kkrooo::rayTriangleIntersection(ray.m_origin,rayDir, obb.v1, obb.v7, obb.v3, ip, l  ) ) return true;
		if( kkrooo::rayTriangleIntersection(ray.m_origin,rayDir, obb.v5, obb.v6, obb.v2, ip, l  ) ) return true;
		if( kkrooo::rayTriangleIntersection(ray.m_origin,rayDir, obb.v5, obb.v2, obb.v8, ip, l  ) ) return true;

		return false;
	}

	/*
	x1    L1P1.x
	y1    L1P1.y
	x2    L1P2.x
	y2    L1P2.y
	x3    L2P1.x
	y3    L2P1.y
	x4    L2P2.x
	y4    L2P2.y
	*/
	bool lineLineCollision2D(const v2f& L1P1, const v2f& L1P2, const v2f& L2P1, const v2f& L2P2)
	{
		f32 _x4sx3 = L2P2.x - L2P1.x;
		f32 _y1sy3 = L1P1.y - L2P1.y;
		f32 _y4sy3 = L2P2.y - L2P1.y;
		f32 _x1sx3 = L1P1.x - L2P1.x;
		f32 _x2sx1 = L1P2.x - L1P1.x;
		f32 _y2sy1 = L1P2.y - L1P1.y;

		auto _yx = _y4sy3*_x2sx1;
		auto _xy = _x4sx3*_y2sy1;

		f32 uA = (_x4sx3*_y1sy3 - _y4sy3*_x1sx3) / (_yx - _xy);
		f32 uB = (_x2sx1*_y1sy3 - _y2sy1*_x1sx3) / (_yx - _xy);

		if( uA >= 0.f && uA <= 1.f && uB >= 0.f && uB <= 1.f )
			return true;

		return false;
	}

	bool pointOnFrontSideCamera(const kkVector4& point, const kkMatrix4& viewProjection)
	{
		auto p = math::mul(point, viewProjection);
		p._f32[2] /= p._f32[3];
		if( p._f32[2] > -1.f && p._f32[2] < 1.f )
			return true;
		return false;
	}

	v2i worldToScreen( const kkMatrix4& WVP, const kkVector4& point3D, const v2f& viewportSize, const v2f& offset )
	{
		kkVector4 point = point3D;
		point.KK_W = 1.f;

		point = math::mul(point,WVP);

		return v2i
		(
			(s32)((viewportSize.x * 0.5f + point.KK_X * viewportSize.x * 0.5f / point.KK_W) + offset.x)
			,
			(s32)((viewportSize.y - ( viewportSize.y * 0.5f + point.KK_Y * viewportSize.y * 0.5f / point.KK_W ))+offset.y)
		);
	}

	v2i worldToScreen2( const kkMatrix4& WVP, const kkVector4& point3D, const v2f& viewportSize )
	{
		kkVector4 point = point3D;
		point.KK_W = 1.f;

		point = math::mul(point,WVP);

		return v2i
		(
			(s32)(viewportSize.x * 0.5f + point.KK_X * viewportSize.x * 0.5f / point.KK_W)
			,
			(s32)(viewportSize.y - ( viewportSize.y * 0.5f + point.KK_Y * viewportSize.y * 0.5f / point.KK_W ))
		);
	}

	kkVector4 lineIntersection( const kkVector4& planePoint, const kkVector4& planeNormal, const kkVector4& linePoint, const kkVector4& lineDirection) 
	{
		auto LD = lineDirection;
		LD.normalize();
		
		if( planeNormal.dot(LD) == 0 )
		{
			return kkVector4();
		}

		LD *= (planeNormal.dot(planePoint) - planeNormal.dot(linePoint)) / planeNormal.dot(LD);

		return linePoint + LD;
	}

	bool pointInTriangle2D(const v2f& s, const v2f& a, const v2f& b, const v2f& c)
	{
		f32 as_x = s.x - a.x;
		f32 as_y = s.y - a.y;
		bool s_ab = (b.x - a.x) * as_y - (b.y - a.y) * as_x > 0.f;
		if( (c.x - a.x) * as_y - (c.y - a.y) * as_x > 0.f == s_ab ) return false;
		if( (c.x - b.x) * (s.y - b.y) - (c.y - b.y) * (s.x - b.x) > 0.f != s_ab ) return false;
		return true;
	}
}