#include "kkrooo.engine.h"
#include "../Common.h"


#include "Plugins/kkPluginObjectCategory.h"
#include "PluginObjectCategory.h"

PluginObjectCategory::PluginObjectCategory(const char16_t* n, const kkPluginID& id)
{
	m_name = n;
	m_pluginID = id;
}

PluginObjectCategory::~PluginObjectCategory()
{
	for( auto sc : m_subCategories )
	{
		kkDestroy(sc);
	}
}

kkPluginObjectSubCategory* PluginObjectCategory::AddSubCategory( const char16_t * name )
{
	for( auto sc : m_subCategories )
	{
		if( sc->m_name == kkString(name) )
			return sc;
	}
	auto sc = kkCreate<PluginObjectSubCategory>(name);
	m_subCategories.push_back(sc);
	return sc;
}


// -----------------------------

PluginObjectSubCategory::PluginObjectSubCategory(const char16_t* n)
{
	m_name = n;
}

PluginObjectSubCategory::~PluginObjectSubCategory()
{
}

void PluginObjectSubCategory::AddItem( const char16_t * name, kkPluginGUICallback callback, s32 id, void * data )
{
	PluginObjectSubCategoryItem item;
	item.m_name     = name;
	item.m_callback = callback;
	item.m_data     = data;
	item.m_id       = id;
	m_items.push_back(item);
}