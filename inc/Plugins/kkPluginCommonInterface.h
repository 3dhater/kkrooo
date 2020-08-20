// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_PL_COMMONINTERFACE_H__
#define __KK_PL_COMMONINTERFACE_H__

#include "Common/kkForward.h"

#include "Scene3D/kkScene3D.h"
#include "kkPluginID.h"

#include "kkPluginObjectCategory.h"

using kkMaterialNode_onCreateParameter = kkMaterialEditorNode*(*)();
using kkMaterialNode_onCreateType      = kkMaterialEditorNode*(*)();


class kkPluginCommonInterface
{
public:
	kkPluginCommonInterface(){}
	virtual ~kkPluginCommonInterface(){}

	// �������� ��� ����� ������ � ��� ������� ��������� � ���� �����
	// ��� �������� ������������ ��������� ������ �� ������ �������
	virtual kkScene3D*         GetScene() = 0;
	virtual kkGeometryCreator* GetGeometryCreator() = 0;
	virtual kkPluginGUI*       GetGUI() = 0;
	virtual kkMainSystem*      GetMainSystem() = 0;

	// �������� kkPluginObjectCategory ����� ������������� � kkObjectCategory
	virtual kkPluginObjectCategory*  CreateObjectCategory( const char16_t* name, const kkPluginID& id ) = 0;


	//virtual kkRenderer*  CreateRenderer( const char16_t* name,  ) = 0;
	virtual void  AddRenderer( kkRenderer* ) = 0; // ��������� kkRenderer* � ��������� ����� ��� ���� ��� �������

	// ������� �������� ��� ��������� ���������� (����).
	// ����������� ��������, �������� (��� ����������(������� ������ �����)), ������� ������� ������� ������ ������ � ���������� ��� 
	virtual void AddMaterialParameter(const char16_t * name, kkRenderer*, kkMaterialNode_onCreateParameter onCreation ) = 0;

	//  ������� ����� ��� ��������� (��� ��������� ����������)
	// ����� ���� ����� �������, ���������� kkMaterial ����� ������� � kkMaterialEditorNode::m_material
	virtual void AddMaterialType(const char16_t * name, kkRenderer*, kkMaterialNode_onCreateType onCreation ) = 0;
	//virtual kkMaterialType* CreateMaterialType( const char16_t* name, kkRenderer*, kkMaterial* materialImplementation, const kkPluginID& uid ) = 0;

	virtual bool    FSFileExist( const char16_t * ) = 0;
	virtual kkFile* FSOpenFile(const char16_t *) = 0;
	virtual kkFile* FSCreateFile(const char16_t *) = 0;
	virtual void    FSCloseFile(kkFile*) = 0;
	
	virtual u8* MemAlloc( u64 size ) = 0;
	virtual u8* MemReAlloc( void* block, u64 size ) = 0;
	virtual void  MemFree( void* block ) = 0;

	virtual kkImage* CreateImage() = 0;
	virtual void     DestroyImage(kkImage*) = 0;
	//virtual kkImage* LoadImage(const char16_t*) = 0;
	virtual kkImageContainerNode* LoadImage(const char16_t*) = 0;
	virtual void RemoveImage(kkImageContainerNode*) = 0; // --refCount
	virtual void ReloadImage(kkImageContainerNode*) = 0;
	virtual kkTexture* GetTexture(kkImage*) = 0;

	virtual void OpenImageFileDialog(kkString* outFilePath) = 0;
};

// �� ��������
class kkDefault_allocator_CI
{
	kkPluginCommonInterface * m_CI = nullptr;
public:
	kkDefault_allocator_CI(){}
	kkDefault_allocator_CI(kkPluginCommonInterface * ci)
		:
		m_CI(ci)
	{
	}

	~kkDefault_allocator_CI()
	{
	}

	void* allocate( u64 size )
	{
		return m_CI->MemAlloc(size);
	}
	void free( void * ptr )
	{
		m_CI->MemFree(ptr);
	}
};

#endif