#ifndef __KK_KRGUI_NODEEDITOR_H__
#define __KK_KRGUI_NODEEDITOR_H__

namespace Kr
{
	namespace Gui
	{
		struct NodeEditor
		{
			Vec2f m_size;
			Style* m_style = nullptr;
			Vec4f m_clipRect;
		
			float   m_zoomValue = 1.f;

			Vec2f m_eyePosition; // for camera moving
			//bool m_zoomAndMove = true;

			enum callbackReason
			{
				_pickSocket,
				_connectSockets,
				_socketsConnected,
				_removeConnection
			};
			bool(*callback)(NodeEditor* nodeEditor, callbackReason reason, NodeEditorNode* firstNode, NodeEditorNodeSocket* socket1, NodeEditorNode* secondNode, NodeEditorNodeSocket* socket2) = nullptr;

			// read only
			bool    m_inRect = false;
			Vec2f m_center;
			std::vector<NodeEditorNode*> m_nodes;
			//int     m_nodeCounter = 0; // ++m_nodeCounter every beginNode. reset every beginNodeEditor. for z-position
			//int     m_numOfNodesInPrevFrame = 0; // non-zero based

			std::vector<NodeEditorSocketConnection> m_socketConnections;
		
			// please, set this manually.
			// add node for z-ordering
			void addNodeZOrdering(NodeEditorNode* node)
			{
				m_nodes.push_back(node);
			}
			// call it if you want to delete node
			void removeNodeZOrdering(NodeEditorNode* node)
			{
				auto it = std::find(m_nodes.begin(),m_nodes.end(),node);
				if( it != m_nodes.end() )
				{
					std::swap(*it, m_nodes.back());
					m_nodes.pop_back();
				}
			}


			// если нода соединена с другими нодами, и потом удаляется, то нужно очистить список и построить заново (вручную)
			// или если не перестраивать, то нужно вручную удалить нужный NodeEditorSocketConnection из массива
			void clearSocketConnections()
			{
				m_socketConnections.clear();
			}
			void addSocketConnection( const NodeEditorSocketConnection& connection )
			{
				for( auto & o : m_socketConnections )
				{
					if( o.first == connection.first && o.second == connection.second )
						return;
					if( o.first == connection.second && o.second == connection.first )
						return;
				}
				m_socketConnections.push_back(connection);
			}
			bool removeSocketConnection(NodeEditorNodeSocket* socket)
			{
				bool result = false;
				for( size_t i = 0, sz = m_socketConnections.size(); i < sz; ++i )
				{
					auto & c = m_socketConnections.at(i);
					if( c.first == socket || c.second == socket )
					{
						m_socketConnections.erase(m_socketConnections.begin()+i);
						result = true;
						--i;
						--sz;
					}
				}

				if( result && callback )
				{
					callback(this, callbackReason::_removeConnection, nullptr, socket, nullptr, nullptr );
				}
				return result;
			}
			NodeEditorSocketConnection* getSocketConnection(NodeEditorNodeSocket* socket)
			{
				for( size_t i = 0, sz = m_socketConnections.size(); i < sz; ++i )
				{
					if( m_socketConnections[i].first == socket ) return &m_socketConnections[i];
				}
				return nullptr;
			}

		private:
			void _updateZOrdering(NodeEditorNode* topNode, int oldZValue )
			{
				assert(topNode);

				if( oldZValue == -1 )
				{
					for( size_t i = 0, sz = m_nodes.size(); i < sz; ++i )
					{
						auto node = m_nodes[i];
						if( node != topNode && node->m_z_position != -1 )
						{
							++node->m_z_position;
						}

					}
				}
				else
				{
					for( size_t i = 0, sz = m_nodes.size(); i < sz; ++i )
					{
						auto node = m_nodes[i];
						if( node != topNode && node->m_z_position != -1 )
						{
							if( node->m_z_position < oldZValue )
								++node->m_z_position;
						}
					
					}
				}

			}

			friend class GuiSystem;
		};
	}
}

#endif