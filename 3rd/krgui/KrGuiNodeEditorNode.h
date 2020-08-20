#ifndef __KK_KRGUI_NODEEDITORNODE_H__
#define __KK_KRGUI_NODEEDITORNODE_H__

namespace Kr
{
	namespace Gui
	{
		struct NodeEditorNode
		{
			std::vector<NodeEditorNodeSocket> m_sockets;
			int m_id = -1;
			Vec2f m_position;
			Vec2f m_size;
			//Vec4f color = Color
		
			void * m_userData = nullptr;
			unsigned long long m_userDataSize = 0;
		

			Vec4f m_rounding = Vec4f(5.f,5.f,5.f,5.f);

			DrawCommandsGroup* m_parentDrawGroup = nullptr;

			// for z-ordering
			int m_z_position = -1; // read-only

			Vec4f m_inputRect_add = Vec4f(10.f,10.f,10.f,10.f); // add area for m_inputRect (f.e. for sockets)
			Vec4f m_inputRect; // read-only

			void addSocket(const Vec2f& position, const Vec2f& size, int id, int userFlags, Style * style = nullptr)
			{
				NodeEditorNodeSocket newSocket;
				newSocket.size = size;
				newSocket.id   = id;
				newSocket.position = position;
				newSocket.parentNode = this;
				newSocket.style = style;
				newSocket.flags = userFlags;

				m_sockets.push_back(newSocket);
			}

			void addSocket(const NodeEditorNodeSocket& newSocket )
			{
				m_sockets.push_back(newSocket);
			}
		};
	}
}

#endif