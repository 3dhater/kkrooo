// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KK_PL_OBJECT_CATEGORY_H__
#define __KK_PL_OBJECT_CATEGORY_H__

#include "Common/kkForward.h"

#include "kkPluginID.h"
#include "kkPlugin.h"

class kkPluginObjectSubCategory
{
public:
	kkPluginObjectSubCategory(){}
	virtual ~kkPluginObjectSubCategory(){}

	// id - некий пользовательский id
	// data - данные которые будут параметром в callback
	virtual void AddItem( const char16_t * name, kkPluginGUICallback callback, s32 id, void * data = nullptr ) = 0;
};

class kkPluginObjectCategory
{
protected:
	kkPluginID     m_pluginID;
public:
	kkPluginObjectCategory(){}
	virtual ~kkPluginObjectCategory(){}

	virtual kkPluginObjectSubCategory* AddSubCategory( const char16_t * name ) = 0;

//	virtual void SetPluginID(){ m_pluginID = id; }
	virtual const kkPluginID& GetPluginID(){ return m_pluginID; }
};

#endif