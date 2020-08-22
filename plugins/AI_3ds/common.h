// SPDX-License-Identifier: GPL-3.0-only
#ifndef __COMMON_H__
#define __COMMON_H__

struct ImportData
{
	kkPluginGUIWindow * import_window = nullptr;
	kkPluginGUIWindow * export_window = nullptr;

	kkPluginCommonInterface* common_interface = nullptr;

	kkString file_path;
};

#endif