// SPDX-License-Identifier: GPL-3.0-only
#include "kkrooo.engine.h"


#include "KrGui.h"

#include "Plugins/kkPlugin.h"
#include "Plugins/kkPluginCommonInterface.h"
#include "FileSystem/kkFileSystem.h"
#include "GraphicsSystem/kkTexture.h"

#include "../Geometry/GeometryCreator.h"
#include "../Scene3D/Scene3D.h"

#include "Renderer/kkRenderer.h"

#include "Material/kkMaterial.h"
#include "../Materials/kkMaterialTypeImpl.h"

#include "PluginManager.h"
#include "PluginCommonInterface.h"
#include "PluginGUIWindow.h"
#include "PluginGUI.h"

#include "PluginObjectCategory.h"

#include "../Application.h"
#include "../Materials/MaterialEditor.h"


#ifdef LoadImage
#undef LoadImage
#endif

PluginCommonInterface::PluginCommonInterface(){}
PluginCommonInterface::~PluginCommonInterface()
{
	
}

kkScene3D* PluginCommonInterface::GetScene()                  { return m_scene3D;         }
kkGeometryCreator* PluginCommonInterface::GetGeometryCreator(){ return m_geomCreator;     }
kkPluginGUI* PluginCommonInterface::GetGUI()                  { return m_pluginGUI;       }
kkMainSystem*      PluginCommonInterface::GetMainSystem()     { return kkGetMainSystem(); }

bool PluginCommonInterface::FSFileExist( const char16_t * path )  { assert(path);return kkFileSystem::existFile( path ); }
kkFile* PluginCommonInterface::FSOpenFile(const char16_t * path ) { assert(path);return util::openFileForReadBinShared( path ); }
kkFile* PluginCommonInterface::FSCreateFile(const char16_t * path){ assert(path); return util::createFileForWriteBinShared( path ); }
void PluginCommonInterface::FSCloseFile(kkFile* f)                { assert(f); kkDestroy(f); }

u8* PluginCommonInterface::MemAlloc( u64 size )                { assert(size>0); return (u8*)kkMemory::allocate(size); }
u8* PluginCommonInterface::MemReAlloc( void* block, u64 size ) { assert(block); return (u8*)kkMemory::reallocate(block,size); }
void  PluginCommonInterface::MemFree( void* block )                { assert(block); kkMemory::free(block); }

kkImage* PluginCommonInterface::CreateImage()           { return kkCreate<kkImage>();}
void     PluginCommonInterface::DestroyImage(kkImage* i){ assert(i); kkDestroy(i);}
//kkImage* PluginCommonInterface::LoadImage(const char16_t* f){ return m_materialEditor->loadImage(f); }
kkImageContainerNode* PluginCommonInterface::LoadImage(const char16_t* f){ assert(f); return m_materialEditor->loadImage(f); }
void PluginCommonInterface::RemoveImage(kkImageContainerNode* i){assert(i);m_materialEditor->removeImage(i);}
void PluginCommonInterface::ReloadImage(kkImageContainerNode* i){assert(i);m_materialEditor->reloadImage(i);}
kkTexture* PluginCommonInterface::GetTexture(kkImage* i){ assert(i); return m_materialEditor->getTexture(i); }

kkPluginObjectCategory*  PluginCommonInterface::CreateObjectCategory( const char16_t* name, const kkPluginID& id )
{
	assert(name);
	return kkSingleton<Application>::s_instance->createObjectCategory(name, id);
}

void  PluginCommonInterface::AddRenderer( kkRenderer* renderer )
{
	assert(renderer);
	kkSingleton<Application>::s_instance->addRenderer( renderer );
}

void PluginCommonInterface::AddMaterialParameter(const char16_t * name, kkRenderer* r, kkMaterialNode_onCreateParameter onCreation )
{
	assert(name);
	assert(r);
	assert(onCreation);
	m_materialEditor->addMaterialParameter(name, r, onCreation);
}

void PluginCommonInterface::AddMaterialType(const char16_t * name, kkRenderer* r, kkMaterialNode_onCreateType onCreation )
{
	assert(name);
	assert(r);
	assert(onCreation);
	m_materialEditor->addMaterialType(name, r, onCreation);
}

void PluginCommonInterface::OpenImageFileDialog(kkString* outFilePath)
{
	assert(outFilePath);
	kkSingleton<Application>::s_instance->openImageFilePathDialog( outFilePath );
}
