// SPDX-License-Identifier: GPL-3.0-only
#ifndef __PLUGIN_OBJECT_CATEGORY_H__
#define __PLUGIN_OBJECT_CATEGORY_H__

#include "Classes/Strings/kkString.h"
#include "Classes/Containers/kkArray.h"

struct PluginObjectSubCategoryItem
{
	kkString m_name;
	kkPluginGUICallback m_callback = nullptr;
	s32 m_id = -1;

	void * m_data = nullptr;
};

class PluginObjectSubCategory : public kkPluginObjectSubCategory
{
public:
	PluginObjectSubCategory(const char16_t*);
	virtual ~PluginObjectSubCategory();

	void AddItem( const char16_t * name, kkPluginGUICallback callback, s32 id, void * data = nullptr );
	
	kkString m_name;
	kkArray<PluginObjectSubCategoryItem> m_items;
	bool m_expanded = true;
};

class PluginObjectCategory : public kkPluginObjectCategory
{
public:
	PluginObjectCategory(const char16_t*, const kkPluginID& id);
	virtual ~PluginObjectCategory();

	kkPluginObjectSubCategory* AddSubCategory( const char16_t * name );

	kkString m_name;
	kkArray<PluginObjectSubCategory*> m_subCategories;
	bool m_expanded = true;
};


#endif
