#include "kkrooo.engine.h"
#include "../Common.h"
//#include "Classes/Containers/kkArray.h"
//
//#include "Material/kkMaterial.h"
//#include "kkMaterialTypeImpl.h"
//
//kkMaterialTypeImpl::kkMaterialTypeImpl(){}
//kkMaterialTypeImpl::~kkMaterialTypeImpl(){}
//
//void kkMaterialTypeImpl::init( const char16_t* name, kkRenderer* renderer, kkMaterial* materialImplementation, const kkPluginID& uid )
//{
//	m_name     = name;
//	m_uid      = uid;
//	m_renderer = renderer;
//	m_materialImplementation = materialImplementation;
//}
//
//const kkPluginID& kkMaterialTypeImpl::GetPluginID(){return m_uid;}
//const char16_t* kkMaterialTypeImpl::GetName(){return m_name.data();}
//kkRenderer* kkMaterialTypeImpl::GetRenderer(){return m_renderer;}
//kkMaterial* kkMaterialTypeImpl::GetMaterialImplementation(){return m_materialImplementation;}
//void kkMaterialTypeImpl::SetSize( float x, float y ){m_size.set(x,y);}
//const v2f& kkMaterialTypeImpl::GetSize(){return m_size;}
//
//void kkMaterialTypeImpl::AddSocket( kkMaterialEditorSocketType type, float centerOffsetY )
//{
//	m_socketType.push_back(type);
//	m_socketCenterOffset.push_back(centerOffsetY);
//}
//
//u64 kkMaterialTypeImpl::GetSocketCount()
//{
//	return m_socketType.size();
//}
//
//kkMaterialEditorSocketType kkMaterialTypeImpl::GetSocketType( u64 index )
//{
//	return m_socketType[index];
//}
//
//float kkMaterialTypeImpl::GetSocketCenterOffset( u64 index )
//{
//	return m_socketCenterOffset[index];
//}
//
//void kkMaterialTypeImpl::AddGUIElement(const kkMaterialEditorNodeGUIElementInfo& info)
//{
//	m_GUIElements.push_back( info );
//}
//
//const kkMaterialEditorNodeGUIElementInfo& kkMaterialTypeImpl::GetGUIElement(u64 i)
//{
//	return m_GUIElements[i];
//}
//
//u64 kkMaterialTypeImpl::GetGUIElementCount()
//{
//	return m_GUIElements.size();
//}