#ifndef __KK_KRGUI_NODEEDITORSOCKETCONNECTION_H__
#define __KK_KRGUI_NODEEDITORSOCKETCONNECTION_H__

namespace Kr
{
	namespace Gui
	{
		struct NodeEditorSocketConnection
		{
			NodeEditorNodeSocket * first  = nullptr;
			NodeEditorNodeSocket * second = nullptr;

			// можно попробовать удалять ненужные connection делая проверку по node
			NodeEditorNode * node1 = nullptr;
			NodeEditorNode * node2 = nullptr;
		};
	}
}

#endif