// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_MATERIAL_H__
#define __KK_MATERIAL_H__

#include "Classes/kkColor.h"
#include "Classes/Strings/kkString.h"
#include "Classes/Containers/kkArray.h"
#include "Classes/Math/kkVector4.h"
#include "GraphicsSystem/kkTexture.h"

#include "Plugins/kkPluginID.h"


//Общие параметры для материалов.
class kkMaterialImplementation
{
protected:
	kkColor        m_diffuseColor = kkColorLightGray;
	float          m_opacity      = 1.f;
	kkImageContainerNode * m_diffuseTexture_ptr = nullptr;
	kkPluginID     m_pluginID;
public:
	kkMaterialImplementation(){}
	virtual ~kkMaterialImplementation(){}
	kkImageContainerNode * GetDiffuseTexture(){ return m_diffuseTexture_ptr;}
	void SetDiffuseTexture(kkImageContainerNode * t){ m_diffuseTexture_ptr = t;}

	kkColor& GetDiffuseColor(){return m_diffuseColor;}
	
	float GetOpacity(){ return m_opacity; }
	void SetOpacity(float o){ m_opacity = o; }

	const kkPluginID& GetPluginID(){ return m_pluginID; }
	void SetPluginID(const kkPluginID& id){ m_pluginID = id; }
};

class kkMaterial
{
protected:
	kkString     m_name;
public:
	kkMaterial(){}
	virtual ~kkMaterial(){}

	kkString& GetName(){return m_name;}
	void SetName(const char16_t* n){if(n) m_name = n;else m_name.clear();}

	kkMaterialImplementation * m_implementation = nullptr;
};

// для рисования картинки в редакторе материалов
// для output нужен ещё и kkImage
// возможно от этого можно избавиться
struct kkMaterialEditorImage
{
	kkMaterialEditorImage(){init(v2i(32,32));}
	kkMaterialEditorImage(const v2i& size){init(size);}
	~kkMaterialEditorImage(){if( texture )kkDestroy(texture);}

	void init( const v2i& size )
	{
		if( image.m_data8 ) return;
		image.m_width    = size.x;
		image.m_height   = size.y;
		image.m_bits     = 32;
		image.m_format   = kkImageFormat::R8G8B8A8;
		image.m_frames   = 1;
		image.m_mipCount = 1;
		image.m_pitch    = image.m_width * 4;
		image.m_dataSize = image.m_height * image.m_pitch;
		image.m_data8    = (u8*)kkMemory::allocate(image.m_dataSize);
	}

	kkImage image;                        // software
	kkTexture* texture = nullptr;         // hardware
};

///////////////////// !!!!!!!!!!!!!!   Видимо, ImageContainerNode нужно вынести наружу, чтобы его мог видеть плагин.
///////////////////// !!!!!!!!!!!!!!   Экземпляр должен быть внутри GUIElement
struct kkImageContainerNode
{
	kkImage   * m_image     = nullptr;
	kkTexture * m_texture   = nullptr;
	kkString    m_filePath;

	int m_refCounter = 0; // несколько нод могут грузить один и тот-же файл. Значит нужно избавиться от такого поведения.
};

// тип ноды
enum class kkMaterialEditorNodeType : u32
{
	_output,   // вывод. всегда одна нода на каждый материал
	_material, // материал/шейдер
	_parameter // параметр
};

// тип шрифта
enum class kkGUITextFontType
{
	Default, // стандартный KrGui
	Small,   // поменьше, тонкий
	Micro    // ещё меньще
};

// 
enum class kkMaterialEditorSocketType
{
	MaterialOut, // не использовать
	
	/*IntIn,
	IntOut,*/
	ValueIn,
	ValueOut,
	VectorIn,
	VectorOut,
	ColorIn,
	ColorOut
};

// объекты GUI, которые будут храниться в нодах
enum class kkMaterialEditorNodeGUIElementType
{
	ButtonIcon,
	Button,
	Text,
	RangeSliderFloat,
	Rectangle,
	PictureBox
};
struct kkMaterialEditorNodeGUIElementInfo
{
	kkMaterialEditorNodeGUIElementType m_type = kkMaterialEditorNodeGUIElementType::Button;
	const char16_t * m_text = nullptr;
	kkGUITextFontType m_font = kkGUITextFontType::Default;

	//kkImage   * m_image   = nullptr;
	//kkTexture * m_texture = nullptr;
	kkImageContainerNode* m_imageNodePtr = nullptr;

	kkColor m_color = kkColorWhite;
	kkColor * m_dynamicColor = nullptr;
	float   m_opacity = 1.f;
	v4f     m_rounding;

	v2f m_positionOffset; // смещение относительно центра ноды
	v2f m_size;

	float m_rangeFloat_min  = 0.f;
	float m_rangeFloat_max  = 100.f;
	float * m_rangeFloat_value = nullptr;
	bool  m_rangeHorizontal = true;

	bool (*m_callback)(kkMaterialEditorNodeGUIElementInfo*) = nullptr;
	bool (*m_textCallback)(kkMaterialEditorNodeGUIElementInfo*, kkString* ) = nullptr;
	int    m_id           = -1;
	void * m_userData     = nullptr;
	int    m_userDataSize = 0;
};
struct kkMaterialNodeSocket
{
	// тип. определяет цвет сокета и способ передачи данных
	kkMaterialEditorSocketType m_type = kkMaterialEditorSocketType::MaterialOut;
	
	// отступ от центра о оси Y
	float m_socketCenterOffset = 0.f;

	int m_id = -1;
};

enum kkMaterialEditorNodeSocketFlags
{
	kkMaterialEditorNodeSocketFlags_            = 0,
	kkMaterialEditorNodeSocketFlags_Texture     = 1,
	kkMaterialEditorNodeSocketFlags_BaseTexture = 2
};

class kkMaterialEditorNodeEditorNode
{
public:
	kkMaterialEditorNodeEditorNode(){}
	virtual ~kkMaterialEditorNodeEditorNode(){}
};

// общая информация о строении ноды в редакторе материалов
// пока, на основе этого работают ноды для параметров. Возможно, kkMaterialType нужно убрать и заменить этим.
class kkMaterialEditorNode
{
	int m_socketId = 0;
protected:
	// GUI рисуется в соответствии с этим списком
	kkArray<kkMaterialEditorNodeGUIElementInfo> m_GUIElements;

	// сокеты - кружки которые соединяются линиями
	kkArray<kkMaterialNodeSocket> m_sockets;

	// размер ноды
	v2f m_size;

	// картинка для output в _output ноде, и возможно для остальных нод требующих картинку
	kkMaterialEditorImage*      m_image    = nullptr;

	// тип ноды
	kkMaterialEditorNodeType    m_nodeType = kkMaterialEditorNodeType::_output;

	// возможно не нужно
	// если эта нода - материал, созданный нажатием на кнопку из списка Materials, то m_materialType != nullptr
//	kkMaterialType*             m_materialType = nullptr;

	// нода для рисования
	//Kr::Gui::NodeEditorNode  m_nodeEditorNode;
	kkMaterialEditorNodeEditorNode * m_nodeEditorNode = nullptr;

	// имя
	kkString m_name;

	kkMaterialImplementation * m_materialImpl = nullptr; // содержит это только если эта нода является материалом

	kkRenderer* m_renderer = nullptr; // указатель на рендерер, для которого прямо предназначен kkMaterialEditorNode

	kkColor  m_colorHead = kkColorLimeGreen;

	// каждая нода должна передавать хоть что-то в output
	float m_defaultFloat = 0.f;
	//int m_defaultInt = 0;
	kkColor m_defaultColor = kkColorWhite;
	v3f m_defaultVector;

	int m_flags = 0;

public:
	kkMaterialEditorNode(){}
	virtual ~kkMaterialEditorNode()
	{
		if( m_image ) kkDestroy(m_image);
		if( m_materialImpl ) kkDestroy(m_materialImpl);
		if( m_nodeEditorNode ) kkDestroy(m_nodeEditorNode);
	}

	void CreateImage(s32 x, s32 y){if(m_image)kkDestroy(m_image);m_image = kkCreate<kkMaterialEditorImage>(v2i(x,y));}
	void AddGUIElement(kkMaterialEditorNodeGUIElementInfo& info)
	{
		if( info.m_id == -2 ) // -2 for delete button
		{
			printf("Warning! kkMaterialEditorNodeGUIElementInfo::m_id == -2\n");
			info.m_id = -1;
		}
		m_GUIElements.push_back( info );
	}
	
	kkArray<kkMaterialNodeSocket>& GetSockets()
	{
		return m_sockets;
	}

	kkArray<kkMaterialEditorNodeGUIElementInfo>& GetGUIElements()
	{
		return m_GUIElements;
	}

	void   SetSize(float x, float y)           { m_size.set(x,y); }
	v2f&   GetSize()                           { return m_size; }
	//Kr::Gui::NodeEditorNode* GetKrGuiNode() { return &m_nodeEditorNode; }
	kkMaterialEditorNodeEditorNode* GetKrGuiNode() { return m_nodeEditorNode; }
	kkMaterialEditorNodeType  GetNodeType()    { return m_nodeType; }
	void SetNodeType(kkMaterialEditorNodeType t) { m_nodeType = t; }
	kkMaterialEditorImage*  GetImage()         { return m_image; }
	const char16_t*  GetName()                 { return m_name.data(); }
	void   SetName(const char16_t* n)          { m_name = n; }
	void   SetRenderer(kkRenderer* r)          { m_renderer = r; } // Используется программой.
	kkRenderer* GetRenderer()                  { return m_renderer; }

	// Вернётся указатель на материал, если эта нода является материалом.
	kkMaterialImplementation* GetMaterialImplementation() { return m_materialImpl; }
	// Использовать только при создании ноды для материала
	// auto materialType = kkCreate<MaterialDefaultType>();
	// materialType->SetMaterial(kkCreate<MaterialDefault>());
	void SetMaterialImplementation(kkMaterialImplementation * m)
	{
		if(m_materialImpl)
			kkDestroy(m_materialImpl);
		m_materialImpl = m;
	}

	void SetColorHead( const kkColor& color ){ m_colorHead = color;}
	const kkColor& GetColorHead(){return m_colorHead;}
	
	void AddSocket( kkMaterialEditorSocketType type, float centerOffsetY, int flags = 0 )
	{
		kkMaterialNodeSocket s;
		s.m_type = type;
		s.m_socketCenterOffset = centerOffsetY;
		s.m_id = m_socketId++;
		m_sockets.push_back(s);

		m_flags = flags;
	}

	// do not use
	// используется программой
	void SetGuiNodeEditorNode( kkMaterialEditorNodeEditorNode * n ){ m_nodeEditorNode = n; }
	kkMaterialEditorNodeEditorNode * GetGuiNodeEditorNode(){ return m_nodeEditorNode; }

	// каждая нода должна брать значения (input сокеты), делать необходимые вычисления, и передавать результат в свой output сокеты
	virtual void CalculateValues() = 0;

	// socketId - в каком порядке был добавлен сокет, такое значение и должно быть указано (начиная с нуля)
	virtual void SetInSocketValue( int socketId, float floatValue ) = 0;
	virtual void SetInSocketValue( int socketId, const v3f& vectorValue ) = 0;
	virtual void SetInSocketValue( int socketId, const kkColor& colorValue ) = 0;
	virtual void SetInSocketValue( int socketId, kkImageContainerNode* image ) = 0;

	virtual void GetOutSocketValue( int socketId, float& floatValue ) = 0;
	virtual void GetOutSocketValue( int socketId, v3f& vectorValue ) = 0;
	virtual void GetOutSocketValue( int socketId, kkColor& colorValue ) = 0;
	virtual void GetOutSocketValue( int socketId, kkImageContainerNode** image ) = 0;
	
	virtual void ResetSocketValue( int socketId ) = 0;
};



#endif