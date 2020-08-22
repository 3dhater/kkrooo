// SPDX-License-Identifier: GPL-3.0-only
#ifndef __COMMON_H__
#define __COMMON_H__

enum class face_type
{
	p,
	pu,
	pun,
	pn
};

struct simple_arr
{
	s32 data[0x100];
	u32 sz = 0;

	void push_back( s32 v )
	{
		data[ sz++ ] = v;
	}

	u32 size()
	{
		return sz;
	}

	void reset()
	{
		sz = 0;
	}
};

struct face
{
	simple_arr p, u, n;
	face_type ft = face_type::pun;
	void reset()
	{
		ft = face_type::pun;
		p.reset();
		u.reset();
		n.reset();
	}
};

struct ImportData
{
	kkPluginGUIWindow * import_window = nullptr;
	kkPluginGUIWindow * export_window = nullptr;

	kkPluginCommonInterface* common_interface = nullptr;

	kkString file_path;

	bool option_import_materials = false;
	bool option_write_materials  = true;
	bool option_triangulate      = false;
	bool option_triangulate_export = false;
	bool option_write_normals    = true;
	bool option_write_UVs        = true;
	bool fix_flip_normals        = false;
	bool fix_generate_normals    = false;
	bool fix_generate_flat_normals    = false;
	bool fix_z_up = false;
	bool optimize_positions = true;
	bool optimize_normals   = true;
	bool optimize_UVs       = true;
	bool selected_only      = true;
};

#endif