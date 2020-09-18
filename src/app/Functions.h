#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

Scene3D** kkGetScene3D();
kkGraphicsSystem* kkGetGS();
void kkGSSetDepth(bool);
void kkGSSetViewport(s32 x, s32 y, s32 z, s32 w);
v2i  kkGetWindowClientSize();
bool kkIsLmbDownOnce();
bool kkIsLmbDown();
bool kkIsLmbUp();
bool kkIsRmbDownOnce();
bool kkIsRmbDown();
bool kkIsRmbUp();
bool kkIsMmbDownOnce();
bool kkIsMmbDown();
bool kkIsMmbUp();

enum class IconFontSymbol
{
	NextMenu,
	NewObject,
	EditObject,
	EditUV,
	ExpandCategory,
	CollapseCategory,

	Cross1,
	Add1,
	Sub1,
	Eq1,
	ArrowR1,
	ArrowL1,
	ArrowT1,
	ArrowB1,
	PushCircle,
	AssignMaterial,
	Save,
	ClearOutputImage,
	Undo,
	Redo,
	SelectByName,
	SelectModeSelect,
	ActiveButtonLTB,
	TransformMove,
	TransformScale,
	TransformRotate,
	EditVertex,
	EditEdge,
	EditFace,
	CheckBoxCheck,
	CheckBoxUncheck,
};

namespace kkrooo
{
	char16_t   getIconFontChar(IconFontSymbol s);
	const char16_t*   getIconFontString(IconFontSymbol s);

	// попадает ли курсор мыши в область
	bool       pointInRect( const v2i& pt, const v4f& rect );
	bool       pointInRect( const v2i& pt, const v4i& rect );

	// координату курсора в 3Д координату
	// работает в perspective
	kkVector4  screenToWorld( const v2i& coord, const v4f& rc, kkCamera* camera, f32 Z = -1.f  );

	// получить дальнюю точку - конец луча. Начало - позиция камеры.
	// работает в perspective
	//kkVector4  getRayTo( const v2i& coord, const v4f& rc, kkCamera* camera, const kkVector4& target );

	// получить луч для ортогонального вьюпорта
	void       getRay_ortho( kkRay&, const v2i& coord, const v4f& rc, kkCamera* camera );

	// проверить пересекает ли луч треугольник
	// возврат истина если пересекает
	// если пересекает то 
	//  - out_intersectionPoint - точка пересечения
	//  - out_len расстояние от начала луча до точки пересечения
	bool rayTriangleIntersection( const kkVector4& rayOrigin, const kkVector4& rayDir, const kkVector4& v0, const kkVector4& v1, const kkVector4& v2, kkVector4& out_intersectionPoint, f32& out_len );
	//bool rayTriangleIntersection( const kkRay& ray, const kkVector4& v0, const kkVector4& v1, const kkVector4& v2, kkVector4& out_intersectionPoint, f32& out_len );

	void getRay( kkRay& ray, const v2i& coord, const v4f& rc, kkCamera* camera );
	void getRay( kkRay& ray, const v2i& coord, const v4f& rc, const v2f& rc_sz, const kkMatrix4& VPinvert );
	
	bool rayIntersection_obb( const kkRay& ray, const kkObb& obb );

	void screenToClient(const v2i& screen_coord, const v4f& client_rect, v2i& out );

	v2i worldToScreen( const kkMatrix4& WVP, const kkVector4& point3D, const v2f& viewportSize, const v2f& offset );
	v2i worldToScreen2( const kkMatrix4& WVP, const kkVector4& point3D, const v2f& viewportSize );
	bool lineLineCollision2D(const v2f& a, const v2f& b, const v2f& c, const v2f& d);
	bool pointInTriangle2D(const v2f& point, const v2f& a, const v2f& b, const v2f& c);

	kkVector4 lineIntersection( const kkVector4& planePoint, const kkVector4& planeNormal, const kkVector4& linePoint, const kkVector4& lineDirection);

	// don't forget to set point.setW(1.f)
	bool pointOnFrontSideCamera(const kkVector4& point, const kkMatrix4& viewProjection);
}


#endif