#include "KrGui.h"

using namespace Kr;

bool Gui::GuiSystem::beginNodeEditor( Gui::NodeEditor * nodeEditor )
{
	assert(nodeEditor);

	_newId();

	m_currentNodeEditor = nodeEditor;
	m_guiZoom_old = m_guiZoom;

	m_topNode = nullptr;
	int topNodeZValue = 9999;
	for( size_t i = 0, sz = nodeEditor->m_nodes.size(); i < sz; ++i )
	{
		auto n = nodeEditor->m_nodes[i];
		if( _internal::pointInRect( m_cursorCoords.x, m_cursorCoords.y, n->m_inputRect ) )
		{
			if( n->m_z_position != -1 && n->m_z_position < topNodeZValue )
			{
				topNodeZValue = n->m_z_position;
				m_topNode = n;
				m_blockInputGlobal = true;
			}
		}
	}
	//if( m_topNode )
	//{
	//	//printf("%i\n", m_topNode->m_z_position);
	//}

	if( !m_currentNodeEditor->m_style ) m_currentNodeEditor->m_style = &m_defaultStyle;
	//m_useParentClipRect = true;

	_setCurrentClipRect(m_drawPointer, m_currentNodeEditor->m_size);

	m_currentNodeEditor->m_clipRect = m_currentClipRect;

	//m_parentClipRect = m_currentNodeEditor->m_clipRect;
	m_parentClipRects[++m_parentClipRectCount] = m_currentClipRect;

	bool inRect = false;
	if( _internal::pointInRect( m_cursorCoords.x, m_cursorCoords.y, m_currentClipRect ) )
	{
		inRect = true;
		_updateMouseInputInRect(mouseButton::MMB);
	}
		
	m_currentNodeEditor->m_inRect = inRect;


	m_guiZoom_old = m_guiZoom;
	m_guiZoom     = m_currentNodeEditor->m_zoomValue;

	m_firstColor  = m_currentNodeEditor->m_style->nodeEditorColor1;
	m_secondColor = m_currentNodeEditor->m_style->nodeEditorColor2;
	
	auto buildRect = m_currentClipRect;
	
	_addRectangle(m_currentClipRect, buildRect);

	m_currentNodeEditor->m_center.x = (m_currentClipRect.z - m_currentClipRect.x)/2;
	m_currentNodeEditor->m_center.y = (m_currentClipRect.w - m_currentClipRect.y)/2;
	m_currentNodeEditor->m_center.x += m_drawPointer.x;
	m_currentNodeEditor->m_center.y += m_drawPointer.y;

	if( m_pressedItemIdMMB == m_uniqueIdCounter )
	{
		m_currentNodeEditor->m_eyePosition.x += (float)m_mouseDelta.x / m_currentNodeEditor->m_zoomValue;
		m_currentNodeEditor->m_eyePosition.y += (float)m_mouseDelta.y / m_currentNodeEditor->m_zoomValue;
		//printf("%i %i\n", m_mouseDelta.x, m_mouseDelta.y);
	}

	for( size_t i = 0, sz = m_currentNodeEditor->m_socketConnections.size(); i < sz; ++i )
	{
		auto * sc = &m_currentNodeEditor->m_socketConnections[ i ];
		drawLine( sc->first->_realPosition, sc->second->_realPosition, 1.2f, Gui::ColorWhite, Gui::ColorDarkGray );
	}

	return true;
}

void Gui::GuiSystem::nodeEditorZoomIn(Gui::NodeEditor * nodeEditor, float v)
{
	nodeEditor->m_zoomValue += v;
	if( nodeEditor->m_zoomValue > 4.f ){ nodeEditor->m_zoomValue = 4.1f; }
}

void Gui::GuiSystem::nodeEditorZoomOut(Gui::NodeEditor * nodeEditor, float v)
{
	nodeEditor->m_zoomValue -= v;
	if( nodeEditor->m_zoomValue < 0.1f ){nodeEditor->m_zoomValue = 0.1f;}
}

void Gui::GuiSystem::endNodeEditor()
{
	m_guiZoom = m_guiZoom_old;
	
	//m_useParentClipRect = false;
	--m_parentClipRectCount;

	// update Draw Group Priority
	//int min_zPosition = 0xffff; // find minimal z value in node editor // MOST TOP
	//int min_drawGroupPrior = 0xffff; // find minimal priority group
	int currentDrawGroupPriority = m_currentDrawGroup->m_priority;
	
	//auto numOfNodes = m_currentNodeEditor->m_nodes.size();
	for( size_t i = 0, sz = m_currentNodeEditor->m_nodes.size(); i < sz; ++i )
	{
		auto node = m_currentNodeEditor->m_nodes[i];
		node->m_parentDrawGroup->m_priority = currentDrawGroupPriority + ((int)sz - node->m_z_position);
		
		//printf("%i [%i]\n", node->m_parentDrawGroup->m_priority, node->m_z_position);
		//printf("%i\n", node->m_z_position);

		/*if( node->m_parentDrawGroup->m_priority < min_drawGroupPrior ){
			min_drawGroupPrior = node->m_parentDrawGroup->m_priority;
		}*/
		/*if( node->m_z_position < min_zPosition ){
			min_zPosition = node->m_z_position;
			min_drawGroupPrior = node->m_parentDrawGroup->m_priority;
		}*/
	}
	//printf("\n");
	if( m_currentNodeEditor->m_inRect  )
	{
		float v = 0.04f + (0.1f * m_currentNodeEditor->m_zoomValue);

		if( m_wheel_delta > 0 )
		{
			nodeEditorZoomIn(m_currentNodeEditor, v);
		}
		if( m_wheel_delta < 0 )
		{
			nodeEditorZoomOut(m_currentNodeEditor, v);
		}
	}

	setDrawPosition(m_currentNodeEditor->m_clipRect.z, m_currentNodeEditor->m_clipRect.y);
	
	//m_maxItemHeightOnCurrentRow = m_currentNodeEditor->m_clipRect.w - m_currentNodeEditor->m_clipRect.y;
	_checkItemHeight(m_currentNodeEditor->m_size.y);

	//_updateDrawGroupPriority();
	m_blockInputGlobal = false;

	m_currentNodeEditor = nullptr;
}

bool Gui::GuiSystem::beginNode(Gui::NodeEditorNode * node)
{
	assert(node);

	m_blockInputGlobal_old = m_blockInputGlobal;
	if( m_topNode == node )
	{
		m_disableBlockInputGlobal = true;
		m_blockInputGlobal = false;
	}

	m_currentNodePosition = &node->m_position;

	//auto prevPriority = m_drawGroupCurrent->m_priority;
	_setNewDrawGroup(false);
	//m_drawGroupCurrent->m_priority = ++prevPriority;
	
	node->m_parentDrawGroup = m_currentDrawGroup;


	if( node->m_z_position == -1 ) // need to set new z-position for this node
	{
		// 0 = top
		node->m_z_position = 0;
		m_currentNodeEditor->_updateZOrdering(node, -1);
	}

	//if( !m_blockInputGlobal )
	{
		++m_uniqueIdCounter;
		m_lastItemId = m_uniqueIdCounter;
	}

	auto size = node->m_size;
	if( size.x == 0 ) size.x = 30;
	if( size.y == 0 ) size.y = 20;

	auto halfSize = size / 2;

	if( m_pressedItemIdLMB == m_uniqueIdCounter && m_blockInputGlobal == false )
	{
		// click, then set this node to top
		auto oldZ = node->m_z_position;
		node->m_z_position = 0;
		m_currentNodeEditor->_updateZOrdering(node, oldZ);

		node->m_position.x += (float)m_mouseDelta.x / m_currentNodeEditor->m_zoomValue;
		node->m_position.y += (float)m_mouseDelta.y / m_currentNodeEditor->m_zoomValue;
	}

	Vec2f position = node->m_position;


	auto centerOffset = m_currentNodeEditor->m_center;
	position.x += centerOffset.x;
	position.y += centerOffset.y;

	m_currentClipRect.x = position.x - halfSize.x;
	m_currentClipRect.y = position.y - halfSize.y;
	m_currentClipRect.z = m_currentClipRect.x + size.x;
	m_currentClipRect.w = m_currentClipRect.y + size.y;


	m_currentClipRect.x += m_currentNodeEditor->m_eyePosition.x;
	m_currentClipRect.z += m_currentNodeEditor->m_eyePosition.x;
	m_currentClipRect.y += m_currentNodeEditor->m_eyePosition.y;
	m_currentClipRect.w += m_currentNodeEditor->m_eyePosition.y;

	m_currentClipRect.x -= centerOffset.x;
	m_currentClipRect.z -= centerOffset.x;
	m_currentClipRect.y -= centerOffset.y;
	m_currentClipRect.w -= centerOffset.y;

	m_currentClipRect *= m_guiZoom;

	m_currentClipRect.x += centerOffset.x;
	m_currentClipRect.z += centerOffset.x;
	m_currentClipRect.y += centerOffset.y;
	m_currentClipRect.w += centerOffset.y;

	auto buildRect = m_currentClipRect;
	/*if( m_useParentClipRect )
	{
		if( m_currentClipRect.x < m_parentClipRect.x ) m_currentClipRect.x = m_parentClipRect.x;
		if( m_currentClipRect.y < m_parentClipRect.y ) m_currentClipRect.y = m_parentClipRect.y;
		if( m_currentClipRect.z > m_parentClipRect.z ) m_currentClipRect.z = m_parentClipRect.z;
		if( m_currentClipRect.w > m_parentClipRect.w ) m_currentClipRect.w = m_parentClipRect.w;
	}*/
	_checkParentClipRect();

	node->m_inputRect = m_currentClipRect;
	node->m_inputRect.x -= node->m_inputRect_add.x;
	node->m_inputRect.y -= node->m_inputRect_add.y;
	node->m_inputRect.z += node->m_inputRect_add.z;
	node->m_inputRect.w += node->m_inputRect_add.w;

	m_currentNodeClipRect = m_currentClipRect;

	bool inRect = false;
	if( _internal::pointInRect( m_cursorCoords.x, m_cursorCoords.y, m_currentClipRect ) && !m_blockInputGlobal )
	{
		inRect = true;
		m_currentWindow->m_currentItemId_inRect = m_uniqueIdCounter;
		
		_updateMouseInputInRectMoveHover();

		if( m_currentWindow->m_lastItemId_inRect == m_uniqueIdCounter )
		{
			_updateMouseInputInRect(mouseButton::LMB);
		}
	}

	m_firstColor  = m_currentNodeEditor->m_style->nodeEditorNodeColor1;
	m_secondColor = m_currentNodeEditor->m_style->nodeEditorNodeColor2;
	m_firstColor.w = m_currentNodeEditor->m_style->nodeEditorNodeColorAlpha;
	m_secondColor.w = m_currentNodeEditor->m_style->nodeEditorNodeColorAlpha;

	_addRectangle(m_currentClipRect, buildRect, node->m_rounding);


	for( size_t i = 0, sz = node->m_sockets.size(); i < sz; ++i )
	{
		if( !m_currentNodeEditor->callback )
			break;

		auto * socket = &node->m_sockets[ i ];


		if( !socket->style ) socket->style = &m_defaultStyle;

		position.x = m_currentNodePosition->x + socket->position.x;
		position.y = m_currentNodePosition->y + socket->position.y;
		position.x += m_currentNodeEditor->m_eyePosition.x;
		position.y += m_currentNodeEditor->m_eyePosition.y;
		position.x *=  m_guiZoom;
		position.y *=  m_guiZoom;
		position.x += centerOffset.x;
		position.y += centerOffset.y;

		auto halfSize = socket->size * 0.5f;

		Vec4f socketClipRect;
		socketClipRect.x = position.x - halfSize.x;
		socketClipRect.y = position.y - halfSize.y;
		socketClipRect.z = position.x + socket->size.x - halfSize.x;
		socketClipRect.w = position.y + socket->size.y - halfSize.y;

		socket->_realPosition.x = socketClipRect.x + ((socketClipRect.z-socketClipRect.x)*0.5f);
		socket->_realPosition.y = socketClipRect.y + ((socketClipRect.w-socketClipRect.y)*0.5f);

		// сам zoom для socket
		socketClipRect.x -= socket->_realPosition.x;
		socketClipRect.z -= socket->_realPosition.x;
		socketClipRect.y -= socket->_realPosition.y;
		socketClipRect.w -= socket->_realPosition.y;
		socketClipRect.x *= m_guiZoom;
		socketClipRect.y *= m_guiZoom;
		socketClipRect.z *= m_guiZoom;
		socketClipRect.w *= m_guiZoom;
		socketClipRect.x += socket->_realPosition.x;
		socketClipRect.z += socket->_realPosition.x;
		socketClipRect.y += socket->_realPosition.y;
		socketClipRect.w += socket->_realPosition.y;
		
		buildRect = socketClipRect;

		if( socketClipRect.x < m_parentClipRects[m_parentClipRectCount].x ) socketClipRect.x = m_parentClipRects[m_parentClipRectCount].x;
		if( socketClipRect.y < m_parentClipRects[m_parentClipRectCount].y ) socketClipRect.y = m_parentClipRects[m_parentClipRectCount].y;
		if( socketClipRect.z > m_parentClipRects[m_parentClipRectCount].z ) socketClipRect.z = m_parentClipRects[m_parentClipRectCount].z;
		if( socketClipRect.w > m_parentClipRects[m_parentClipRectCount].w ) socketClipRect.w = m_parentClipRects[m_parentClipRectCount].w;

		//if( !m_blockInputGlobal)
		{
			++m_uniqueIdCounter;
			m_lastItemId = m_uniqueIdCounter;
		}

		if( _internal::pointInRect( m_cursorCoords.x, m_cursorCoords.y, socketClipRect )/* && !m_blockInputGlobal*/ )
		{
			
			//printf("%i\n", socket.id);
			m_currentWindow->m_currentItemId_inRect = m_uniqueIdCounter;
//			_updateMouseInputInRectMoveHover();
			if( m_mouseIsLMB_firstClick )
			{
				//printf("%i\n", socket.id);
				m_pressedItemIdLMB = m_uniqueIdCounter;

				m_nodeEditorPickedNode = node;
				m_nodeEditorPickedSocket = socket;
				if( !m_currentNodeEditor->callback(m_currentNodeEditor, Gui::NodeEditor::callbackReason::_pickSocket, m_nodeEditorPickedNode, m_nodeEditorPickedSocket, m_nodeEditorPickedNode, m_nodeEditorPickedSocket ) )
				{
					m_nodeEditorPickedNode = nullptr;
					m_nodeEditorPickedSocket = nullptr;
					m_pressedItemIdLMB = 0;
				}
			}

			if( m_mouseIsLMB_up && m_nodeEditorPickedSocket )
			{
				if( m_currentNodeEditor->callback(m_currentNodeEditor, Gui::NodeEditor::callbackReason::_connectSockets, m_nodeEditorPickedNode, m_nodeEditorPickedSocket, node, socket ) )
				{
					//m_nodeEditorPickedSocket->connectedWith = socket;
					//socket->connectedWith = m_nodeEditorPickedSocket;
					Gui::NodeEditorSocketConnection socketConnection;
					socketConnection.first  = m_nodeEditorPickedSocket;
					socketConnection.second = socket;
					socketConnection.node1  = socket->parentNode;
					socketConnection.node2  = m_nodeEditorPickedSocket->parentNode;
					m_currentNodeEditor->addSocketConnection(socketConnection);

					m_currentNodeEditor->callback(m_currentNodeEditor, Gui::NodeEditor::callbackReason::_socketsConnected, m_nodeEditorPickedNode, m_nodeEditorPickedSocket, node, socket );
				}
				m_nodeEditorPickedNode = nullptr;
				m_nodeEditorPickedSocket = nullptr;
			}
		}


		socketClipRect = m_parentClipRects[m_parentClipRectCount];

	//	printf("%i\n",m_uniqueIdCounter);
		if( m_pressedItemIdLMB == m_uniqueIdCounter /*&& !m_blockInputGlobal*/ )
		{
			m_currentClipRect = m_parentClipRects[m_parentClipRectCount];
			drawLine( socket->_realPosition, m_cursorCoords, 1.2f, Gui::ColorWhite, Gui::ColorDarkGray );
		}

		m_firstColor = socket->style->nodeEditorSocketIdleColor;
		m_secondColor = socket->style->nodeEditorSocketIdleColor;
		_addRectangle(socketClipRect, buildRect, socket->rounding );
	}

	return true;
}
void Gui::GuiSystem::endNode()
{
	if( m_disableBlockInputGlobal )
	{
		m_blockInputGlobal = m_blockInputGlobal_old;
	}

	m_disableBlockInputGlobal = false;
	m_currentNodePosition = nullptr;

	_setNewDrawGroup(true);
}