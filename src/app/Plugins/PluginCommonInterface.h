// SPDX-License-Identifier: GPL-3.0-only
#ifndef __PLUGIN_COMMINT_H__
#define __PLUGIN_COMMINT_H__


#include "Classes/Strings/kkString.h"
#include "Classes/Containers/kkArray.h"
#include "Plugins/kkPluginCommonInterface.h"

class Scene3D;
class GeometryCreator;
class PluginGUI;
class PluginObjectCategory;
class MaterialEditor;

class PluginCommonInterface : public kkPluginCommonInterface
{
	Scene3D *         m_scene3D     = nullptr;
	GeometryCreator * m_geomCreator = nullptr;
	PluginGUI *       m_pluginGUI   = nullptr;
	MaterialEditor *  m_materialEditor = nullptr;

	friend class Application;
public:
	PluginCommonInterface();
	virtual ~PluginCommonInterface();

	kkScene3D*         GetScene();
	kkGeometryCreator* GetGeometryCreator();
	kkPluginGUI*       GetGUI();
	kkMainSystem*      GetMainSystem();
	
	bool FSFileExist( const char16_t * );
	kkFile* FSOpenFile(const char16_t *);
	kkFile* FSCreateFile(const char16_t *);
	void FSCloseFile(kkFile*);

	u8* MemAlloc( u64 size );
	u8* MemReAlloc( void* block, u64 size );
	void  MemFree( void* block );

	kkImage* CreateImage();
	void     DestroyImage(kkImage*);
	//kkImage* LoadImage(const char16_t*);
	kkImageContainerNode* LoadImage(const char16_t*);
	void RemoveImage(kkImageContainerNode*);
	void ReloadImage(kkImageContainerNode*);
	kkTexture* GetTexture(kkImage*);

	kkPluginObjectCategory*  CreateObjectCategory( const char16_t* name, const kkPluginID& id );
	void AddRenderer( kkRenderer* );
	void AddMaterialParameter(const char16_t * name, kkRenderer*, kkMaterialNode_onCreateParameter onCreation );
	void AddMaterialType(const char16_t * name, kkRenderer*, kkMaterialNode_onCreateType onCreation );
	//kkMaterialType* CreateMaterialType( const char16_t* name, kkRenderer* renderer, kkMaterial* materialImplementation, const kkPluginID& uid );

	kkString  m_filePath;


	void OpenImageFileDialog(kkString* outFilePath);
	//kkStringA  m_filePath_a;
};

#endif