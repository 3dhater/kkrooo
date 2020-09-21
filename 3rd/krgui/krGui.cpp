#include "KrGui.h"

#ifdef KRGUI_PLATFORM_WINDOWS
#include <Windows.h>
#include <Windowsx.h>
#endif

using namespace Kr;//Gui;


int Gui::GuiSystem::m_wheel_delta = 0;
bool Gui::GuiSystem::m_IsLeft = false;
bool Gui::GuiSystem::m_IsRight = false;
bool Gui::GuiSystem::m_IsDelete = false;
bool Gui::GuiSystem::m_IsBackspace = false;
bool Gui::GuiSystem::m_IsHome = false;
bool Gui::GuiSystem::m_IsEnd = false;
bool Gui::GuiSystem::m_IsA = false;
bool Gui::GuiSystem::m_IsX = false;
bool Gui::GuiSystem::m_IsC = false;
bool Gui::GuiSystem::m_IsV = false;
bool Gui::GuiSystem::m_IsLMBDouble = false;
char16_t Gui::GuiSystem::m_character = 0;
Gui::Vec2f Gui::GuiSystem::m_mouseDelta = Gui::Vec2f();
Gui::Vec2f Gui::GuiSystem::m_cursorCoords = Gui::Vec2f();

std::u16string Gui::GetTextFromClipboard()
{
	std::u16string result;
#ifdef KRGUI_PLATFORM_WINDOWS
	if(!OpenClipboard(0))
		return result;

	HANDLE hData = GetClipboardData( CF_UNICODETEXT );
	char16_t* buffer = (char16_t*)GlobalLock( hData );
	GlobalUnlock( hData );
	CloseClipboard();
	result = buffer;
#else
#error Implement....
#endif
	return result;
}
void Gui::CopyToClipboard( const char16_t* str)
{
	assert(str);

	auto len = Gui::_internal::stl_len(str);
	if(!len)
		return;

#ifdef KRGUI_PLATFORM_WINDOWS
	if(!OpenClipboard(0))
		return;
	EmptyClipboard();
	HGLOBAL clipbuffer;
	clipbuffer = GlobalAlloc(GMEM_DDESHARE, (len+1) * sizeof(WCHAR));

	wchar_t* buffer;
	buffer = (wchar_t*)GlobalLock(clipbuffer);

	memcpy(buffer, str, len * sizeof(char16_t));
	buffer[len] = 0;

	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_UNICODETEXT, clipbuffer);
	CloseClipboard();
#else
#error Implement....
#endif
}

Gui::GuiSystem::GuiSystem()
{
	for(int i = 0; i < KRGUI_POPUP_DEPTH; ++i)
	{
		m_popupMenuActive[i] = nullptr;
	}
}

Gui::GuiSystem::~GuiSystem()
{
	for( size_t i = 0, sz = m_fontCache.size(); i < sz; ++i )
	{
		delete m_fontCache.data()[ i ];
	}


	switch (m_gsType)
	{
	case Gui::GraphicsSystemType::OpenGL3:
		if( m_whiteTexture )
		{
			Gui::GraphicsSystem_OpenGL3DestroyTexture(m_whiteTexture);
		}
		Gui::GraphicsSystem_OpenGL3Release();
		break;
	default:
		printf("IMPLEMENT!!! %i\n", KRGUI_LINE );
		break;
	}

	//if( m_drawGroupMain )delete m_drawGroupMain;
	for( size_t i = 0, sz = m_drawCommandGroups.size(); i < sz; ++i )
	{
		delete m_drawCommandGroups.data()[ i ];
	}
}

bool Gui::GuiSystem::_init(Gui::GraphicsSystemType type, const char* defaultFontDir, const char* defaultFontFilename)
{
	m_gsType = type;

	m_drawGroupMain = new Gui::DrawCommandsGroup;
	m_currentDrawGroup = m_drawGroupMain;
	m_drawCommandGroups.push_back(m_drawGroupMain);

	bool result = false;

	switch (m_gsType)
	{
	case Gui::GraphicsSystemType::OpenGL3:
		result = Gui::GraphicsSystem_OpenGL3Init();
		if( result )
		{
			Image whiteImage;
			whiteImage.m_width  = 2;
			whiteImage.m_height = 2;
			whiteImage.m_pitch  = whiteImage.m_width * 4; // RGBA - 4 bytes
			whiteImage.m_dataSize = whiteImage.m_pitch * whiteImage.m_height;
			whiteImage.m_data   = new Image::_byte[whiteImage.m_dataSize];
			whiteImage.fill(ColorWhite);
			m_whiteTexture = Gui::GraphicsSystem_OpenGL3CreateTexture(&whiteImage);
			if(!m_whiteTexture)
			{
				printf("Can't create white texture...line %i\n", KRGUI_LINE );
				result = false;
			}
		}
		break;
	default:
		result = false;
		printf("IMPLEMENT!!! %i\n", KRGUI_LINE );
		break;
	}

	if( result )
	{
		//std::string str = defaultFontDir;
		//str += defaultFontFilename;
		m_defaultFont = createFontFromFile(defaultFontDir, defaultFontFilename);
		
		//if(!m_defaultFont) return false;
		if(!m_defaultFont)
		{
			m_defaultFont = createFontFromSystem(u"Noto Sans", 8, false, false, true, true, true, defaultFontDir, defaultFontFilename );
			
			if(!m_defaultFont)
			{
				m_defaultFont = createFontFromSystem(u"Courier", 8, false, false, true, true, false );
				if(!m_defaultFont)
				{
					fprintf(stderr, "%s\n", "Can't create default font\n");
					return false;
				}
			}
		}

		m_fontCache.push_back(m_defaultFont);
		m_currentFont = m_defaultFont;
	}

	return result;
}



void Gui::GuiSystem::setCurrentFont(Gui::Font* font)
{
	m_currentFont = font ?  font : m_defaultFont;
}

void Gui::GuiSystem::newFrame(Gui::Window * guiWindow, float deltaTime)
{
	m_deltaTime = deltaTime;
	m_currentWindow = guiWindow;

	if( !m_mouseIsLMB ) m_pressedItemIdLMB = 0;
	if( !m_mouseIsMMB ) m_pressedItemIdMMB = 0;
	if( !m_mouseIsRMB ) m_pressedItemIdRMB = 0;
	m_lastCursorHoverItemId = 0;
	m_lastCursorMoveItemId  = 0;
	m_lastDisabledItemId    = 0;
	m_lastItemId            = 0;
	m_lastKeyboardInputItemId            = 0;
	m_lastKeyboardInputItemIdExit            = 0;

	m_groupInfoCount = 0;

	m_IsEsc = false;
	m_IsShift = false;
	m_IsAlt   = false;
	m_IsCtrl  = false;
	m_IsEnter = false;
	

	m_mouseIsLMB_old = m_mouseIsLMB;
	m_mouseIsLMB = false;
	m_mouseIsLMB_up = false;
	m_mouseIsLMB_firstClick = false;

	m_mouseIsMMB_old = m_mouseIsMMB;
	m_mouseIsMMB = false;
	m_mouseIsMMB_up = false;
	m_mouseIsMMB_firstClick = false;

	m_mouseIsRMB_old = m_mouseIsRMB;
	m_mouseIsRMB = false;
	m_mouseIsRMB_up = false;
	m_mouseIsRMB_firstClick = false;

	m_uniqueIdCounter = 0;

	m_lastGroupInRectId = m_currentGroupInRectId;
	m_currentGroupInRectId = 0;

	switchWindow(guiWindow);

	m_mouseDelta.x = m_cursorCoords.x - m_cursorCoordsOld.x;
	m_mouseDelta.y = m_cursorCoords.y - m_cursorCoordsOld.y;

	m_cursorCoordsOld = m_cursorCoords;
}

void Gui::GuiSystem::switchWindow( Gui::Window * guiWindow )
{
	m_currentWindow->m_currentItemId_inRect = 0;
	m_currentWindow = guiWindow;
	m_maxItemHeightOnCurrentRow[m_maxItemHeightOnCurrentRowCount] = 0;
	m_drawPointer.x = 0;
	m_drawPointer.y = 0;
	m_drawGroupPriorityCounter = 0;
	//m_addMenuItemCounter = 0;

	//m_OSWindow = OSWindow;
	m_doWindowInput = false;

#ifdef KRGUI_PLATFORM_WINDOWS
	{
		HWND hwnd = reinterpret_cast<HWND>(guiWindow->OSWindow);
		if( GetActiveWindow() == hwnd )
		{
			m_doWindowInput = true;

			POINT point;
			GetCursorPos(&point);
			ScreenToClient(hwnd,&point);

			m_cursorCoords.x = (float)point.x;
			m_cursorCoords.y = (float)point.y;
		}
		
		RECT rc;
		GetClientRect(hwnd,&rc);
		m_OSWindowClientRect.x = (float)rc.left;
		m_OSWindowClientRect.y = (float)rc.top;
		m_OSWindowClientRect.z = (float)rc.right;
		m_OSWindowClientRect.w = (float)rc.bottom;

		
		
		if( GetAsyncKeyState(VK_LBUTTON) ) m_mouseIsLMB = true;
		if( GetAsyncKeyState(VK_RBUTTON) ) m_mouseIsRMB = true;
		if( GetAsyncKeyState(VK_MBUTTON) ) m_mouseIsMMB = true;

		if( GetAsyncKeyState(VK_SHIFT) ) m_IsShift = true;
		if( GetAsyncKeyState(VK_MENU) )  m_IsAlt   = true;

		if( GetAsyncKeyState(VK_RETURN) )  m_IsEnter   = true;
		if( GetAsyncKeyState(VK_CONTROL) )  m_IsCtrl   = true;
		if( GetAsyncKeyState(VK_ESCAPE) )  m_IsEsc   = true;
	}
#else
#error Please implement me
#endif

	static bool m_lmb_firstClick = false;
	static bool m_mmb_firstClick = false;
	static bool m_rmb_firstClick = false;

	if( m_mouseIsLMB_old && !m_mouseIsLMB )
	{
		m_mouseIsLMB_up = true; // отжали LMB
		m_lmb_firstClick = false;
	}

	if( m_mouseIsMMB_old && !m_mouseIsMMB )
	{
		m_mouseIsMMB_up = true; // отжали MMB
		m_mmb_firstClick = false;
	}

	if( m_mouseIsRMB_old && !m_mouseIsRMB )
	{
		m_mouseIsRMB_up = true; // отжали RMB
		m_rmb_firstClick = false;
	}

	if( m_mouseIsLMB && !m_lmb_firstClick && m_doWindowInput)
	{
		m_mouseIsLMB_firstClick = true;
		m_lmb_firstClick = true; // закрыли проход. открыть - при отжатии
	}

	if( m_mouseIsMMB && !m_mmb_firstClick && m_doWindowInput)
	{
		m_mouseIsMMB_firstClick = true;
		m_mmb_firstClick = true; // закрыли проход. открыть - при отжатии
	}

	if( m_mouseIsRMB && !m_rmb_firstClick && m_doWindowInput)
	{
		m_mouseIsRMB_firstClick = true;
		m_rmb_firstClick = true; // закрыли проход. открыть - при отжатии
	}

	m_OSWindowClientSize.x = m_OSWindowClientRect.z - m_OSWindowClientRect.x;
	m_OSWindowClientSize.y = m_OSWindowClientRect.w - m_OSWindowClientRect.y;
}

void Gui::GuiSystem::endFrame()
{
	m_popupMenuInfoCount = 0;
	m_wheel_delta = 0;
	m_IsLeft = false;
	m_IsRight = false;
	m_IsDelete = false;
	m_IsBackspace = false;
	m_IsHome = false;
	m_IsEnd = false;
	m_IsA = false;
	m_IsC = false;
	m_IsX = false;
	m_IsV = false;
	m_IsLMBDouble = false;
	m_character = 0;
}

void Gui::GuiSystem::render()
{
	m_currentWindow->m_lastItemId_inRect = m_currentWindow->m_currentItemId_inRect;

	// 0 1 2 3 4
	std::sort(m_drawCommandGroups.begin()+1, m_drawCommandGroups.end(), 
		[](Gui::DrawCommandsGroup* first, Gui::DrawCommandsGroup* second)
		{
			return first->m_priority < second->m_priority;
		}
	);

	switch (m_gsType)
	{
	case Gui::GraphicsSystemType::OpenGL3:
		Gui::GraphicsSystem_OpenGL3Draw(this);
		break;
	default:
		printf("IMPLEMENT!!! %i\n", KRGUI_LINE );
		break;
	}
	_clearCommands();
}

void Gui::GuiSystem::drawLine( const Vec2f& begin, const Vec2f& end, float size, const Vec4f& color1, const Vec4f& color2 )
{
	auto dir = end - begin;
	auto angle = std::atan2(dir.y, dir.x)+Gui::PI;
	
	auto cs = std::cos(-angle);
	auto sn = std::sin(-angle);

	Vec2f p1, p2, p3, p4;
	p1.x = (-size * sn) + begin.x;
	p1.y = (-size * cs) + begin.y;

	p2.x = (size * sn) + begin.x;
	p2.y = (size * cs) + begin.y;

	p3.x = (size * sn) + end.x;
	p3.y = (size * cs) + end.y;

	p4.x = (-size * sn) + end.x;
	p4.y = (-size * cs) + end.y;


	Gui::DrawCommands * command = _getDrawCommand();
	command->clipRect = m_currentClipRect;
	command->texture.texture_address = 0;
	Gui::Vertex vertex1;
	Gui::Vertex vertex2;
	Gui::Vertex vertex3;
	Gui::Vertex vertex4;
	
	vertex1.position = p1;
	vertex2.position = p2;
	vertex3.position = p3;
	vertex4.position = p4;
	vertex1.textCoords.set( 0, 0 );
	vertex2.textCoords.set( 0, 0 );
	vertex3.textCoords.set( 0, 0 );
	vertex4.textCoords.set( 0, 0 );
	vertex1.color = color1;
	vertex2.color = color1;
	vertex3.color = color2;
	vertex4.color = color2;
	command->inds.clear();
	command->inds.push_back(0);
	command->inds.push_back(1);
	command->inds.push_back(2);
	command->inds.push_back(0);
	command->inds.push_back(2);
	command->inds.push_back(3);
	command->verts.clear();
	command->verts.push_back(vertex1);
	command->verts.push_back(vertex2);
	command->verts.push_back(vertex3);
	command->verts.push_back(vertex4);
}

bool Gui::GuiSystem::isLastItemCursorHover(){ if(m_disableInput)return false; return m_lastCursorHoverItemId == m_lastItemId;}
bool Gui::GuiSystem::isLastItemCursorMove() { if(m_disableInput)return false; return m_lastCursorMoveItemId == m_lastItemId;}
bool Gui::GuiSystem::isLastItemPressed()    { if(m_disableInput)return false; return m_pressedItemIdLMB == m_lastItemId;  }
bool Gui::GuiSystem::isLastItemPressedOnce(){ if(m_disableInput)return false; bool result = m_pressedItemIdLMB == m_lastItemId; if(result)m_pressedItemIdLMB = -1; return result;  }
bool Gui::GuiSystem::isLastItemDisabled()   { if(m_disableInput)return false; return m_lastDisabledItemId == m_lastItemId;}
bool Gui::GuiSystem::isLastItemKeyboardInput(){ if(m_disableInput)return false; return m_lastKeyboardInputItemId == m_lastItemId;}
bool Gui::GuiSystem::isLastItemKeyboardInputExit(){ if(m_disableInput)return false; return m_lastKeyboardInputItemIdExit == m_lastItemId;}


void Gui::GuiSystem::newLine(float offset)
{
	if( m_parentClipRectCount ) // если рисуется внутри чего-то
	{
		m_drawPointer.x = m_parentClipRects[m_parentClipRectCount].x; // то устанавливается относительно этого чего-то
	}
	else
	{
		m_drawPointer.x = 0; // иначе просто 0
	}
	m_drawPointer.y += (m_maxItemHeightOnCurrentRow[m_maxItemHeightOnCurrentRowCount] + offset); // и переход на новую строку
	
	if( m_groupInfoCount )
	{
		auto * currentGroup = &m_groupInfo[m_groupInfoCount];
		currentGroup->m_contentHeight += m_maxItemHeightOnCurrentRow[m_maxItemHeightOnCurrentRowCount] * m_guiZoom;
		currentGroup->m_contentHeight += offset;
	}

	m_maxItemHeightOnCurrentRow[m_maxItemHeightOnCurrentRowCount] = 0;
}


void Gui::GuiSystem::movePause(bool v){m_pauseMove = v;}
void Gui::GuiSystem::moveLeftRight(float offset){m_drawPointer.x += offset;}

int Gui::GuiSystem::getTextLen( const char16_t * text, Vec2f * out_size, float spacing, float spaceAdd )
{
	int len = _internal::strLen(text);
	float max_h  = 0;
	for( int i = 0; i < len; ++i )
	{
		auto * g = &m_currentFont->m_glyphs[ text[i] ];
		out_size->x += ((float)g->width + spacing) * m_guiZoom;
		if(g->symbol == u' ')
			out_size->x += spaceAdd * m_guiZoom;

		if( g->height > max_h ) 
			max_h = g->height;
	}
	out_size->y = max_h * m_guiZoom;
	return len;
}

float Gui::GuiSystem::getTextMaxCharHeight( const char16_t * text )
{
	float result = 0;
	auto len = _internal::strLen(text);
	for( int i = 0; i < len; ++i )
	{
		auto * g = &m_currentFont->m_glyphs[ text[i] ];
		if( g->height > result ) result = g->height;
	}
	return result;
}

void Gui::GuiSystem::setDrawPosition( float x, float y, bool zeroHeight )
{
	m_drawPointer.x = x;
	m_drawPointer.y = y;
	if(zeroHeight) m_maxItemHeightOnCurrentRow[m_maxItemHeightOnCurrentRowCount] = 0.f;
}

const Gui::Vec2f& Gui::GuiSystem::getDrawPosition()
{
	return m_drawPointer;
}

void Gui::GuiSystem::saveImageToFile(Gui::Image* img, const char* fileName, const char* extesionName )
{
	assert(img);
	assert(fileName);
	assert(extesionName);
	char filePath[256];
	sprintf(filePath, "%s%s", fileName, extesionName);
	
	//FILE * f = fopen(filePath, "w+");
	_internal::file_io f;
	if(!f.open(filePath, "wb+")) return;

	_internal::dds_header hdr;
	hdr.height = img->m_height;
	hdr.width = img->m_width;
	hdr.pitchOrLinearSize = img->m_dataSize;
	
	const unsigned int dds_magic = 0x20534444;
	f.writeUnsignedInt(dds_magic);
	f.writeBytes(&hdr, hdr.size);
	f.writeBytes(img->m_data, img->m_dataSize);
}

bool Gui::GuiSystem::loadImageFromFile(Image* img, const char* fileName )
{
	assert(img);
	assert(fileName);
	if( !std::filesystem::exists(fileName) )        return false;
	if( std::filesystem::file_size(fileName) < 4 )  return false;

	_internal::file_io f;
	if(!f.open(fileName, "rb+")) return false;
	unsigned int dds_magic = 0;// = 0x20534444;
	auto read = f.readUnsignedInt(dds_magic);
	
	if(read!=4) return false;
	if(dds_magic != 0x20534444) return false;

	_internal::dds_header hdr;
	read = f.readBytes(&hdr, sizeof(_internal::dds_header));
	if(read!=sizeof(_internal::dds_header)) return false;
	
	if( !(hdr.ddspf.flags & 0x00000041) ) return false;
	if( hdr.ddspf.RGBBitCount != 32 ) return false;

	img->m_width = hdr.width;
	img->m_height = hdr.height;
	img->m_bits = 32;
	img->m_pitch = hdr.width * 4;
	img->m_dataSize = img->m_height * img->m_pitch;
	img->m_data = new Image::_byte[img->m_dataSize];

	f.readBytes(img->m_data, img->m_dataSize);

	// BGR to RGB
	auto data = img->m_data;
	for( unsigned int h = 0; h < hdr.height; ++h )
	{
		for( unsigned int w = 0; w < hdr.width; ++w )
		{
			auto red = data[0];
			auto blue = data[2];
			data[0] = blue;
			data[2] = red;

			data += 4;
		}
	}

	return true;
}

bool Gui::GuiSystem::addSensorArea( const Vec2f& _size )
{
	_newId();
	Vec2f size = _size;
	_checkSize(&size);
	
	auto position = m_drawPointer;
	if( m_currentNodePosition )
	{
		position.x = m_currentNodePosition->x + m_currentNodeContentOffset.x;
		position.y = m_currentNodePosition->y + m_currentNodeContentOffset.y;
		position.x += m_currentNodeEditor->m_eyePosition.x;
		position.y += m_currentNodeEditor->m_eyePosition.y;
		position.x *=  m_guiZoom;
		position.y *=  m_guiZoom;
		auto centerOffset = m_currentNodeEditor->m_center;
		position.x += centerOffset.x;
		position.y += centerOffset.y;
	}
	Vec4f sensorRect;
	sensorRect.x = position.x;
	sensorRect.y = position.y;
	sensorRect.z = position.x + size.x;
	sensorRect.w = position.y + size.y;

	return _internal::pointInRect( m_cursorCoords.x, m_cursorCoords.y, sensorRect );
}


void Gui::GuiSystem::_checkStyle(Gui::Style** style)
{
	if(!*style) *style = &m_defaultStyle;
	if(!(*style)->iconFont) (*style)->iconFont = m_currentFont;
}

void Gui::GuiSystem::_newId()
{
	++m_uniqueIdCounter;
	m_lastItemId = m_uniqueIdCounter;
}

void Gui::GuiSystem::_checkSize(Vec2f* size)
{
	if( size->x == 0 ) size->x = 20.f;
	if( size->y == 0 ) size->y = 20.f;
	*size *= m_guiZoom;
}

void Gui::GuiSystem::_checkNewLine(float end_x_position)
{
	if( m_parentClipRectCount )
	{
		if( end_x_position > m_parentClipRects[m_parentClipRectCount].z ) 
			newLine();
	}
}

void Gui::GuiSystem::_setCurrentClipRect(const Vec2f& position, const Vec2f& size)
{
	m_currentClipRect.x = position.x;
	m_currentClipRect.y = position.y;
	m_currentClipRect.z = position.x + size.x;
	m_currentClipRect.w = position.y + size.y;
}

void Gui::GuiSystem::_checkParentClipRect(Vec4f& rect)
{
	if( m_parentClipRectCount )
	{
		if( rect.x < m_parentClipRects[m_parentClipRectCount].x ) 
			rect.x = m_parentClipRects[m_parentClipRectCount].x;

		if( rect.y < m_parentClipRects[m_parentClipRectCount].y ) 
			rect.y = m_parentClipRects[m_parentClipRectCount].y;

		if( rect.z > m_parentClipRects[m_parentClipRectCount].z ) 
			rect.z = m_parentClipRects[m_parentClipRectCount].z;

		if( rect.w > m_parentClipRects[m_parentClipRectCount].w ) 
			rect.w = m_parentClipRects[m_parentClipRectCount].w;
	}
}

void Gui::GuiSystem::_checkParentClipRect()
{
	if( m_parentClipRectCount )
	{
		if( m_currentClipRect.x < m_parentClipRects[m_parentClipRectCount].x ) 
			m_currentClipRect.x = m_parentClipRects[m_parentClipRectCount].x;

		if( m_currentClipRect.y < m_parentClipRects[m_parentClipRectCount].y ) 
			m_currentClipRect.y = m_parentClipRects[m_parentClipRectCount].y;

		if( m_currentClipRect.z > m_parentClipRects[m_parentClipRectCount].z ) 
			m_currentClipRect.z = m_parentClipRects[m_parentClipRectCount].z;

		if( m_currentClipRect.w > m_parentClipRects[m_parentClipRectCount].w ) 
			m_currentClipRect.w = m_parentClipRects[m_parentClipRectCount].w;
	}
}

void Gui::GuiSystem::_checkItemHeight(float size_y)
{
	if(size_y > m_maxItemHeightOnCurrentRow[m_maxItemHeightOnCurrentRowCount])
		m_maxItemHeightOnCurrentRow[m_maxItemHeightOnCurrentRowCount] = size_y;
}

void Gui::GuiSystem::_checkZoomPosition(Vec2f* point)
{
	if( m_currentNodePosition )
	{
		point->x = m_currentNodePosition->x + m_currentNodeContentOffset.x;
		point->y = m_currentNodePosition->y + m_currentNodeContentOffset.y;

		point->x += m_currentNodeEditor->m_eyePosition.x;
		point->y += m_currentNodeEditor->m_eyePosition.y;

		point->x *=  m_guiZoom;
		point->y *=  m_guiZoom;

		auto centerOffset = m_currentNodeEditor->m_center;

		point->x += centerOffset.x;
		point->y += centerOffset.y;
	}
}

const Gui::Vec4f& Gui::GuiSystem::getLastClipRect()
{
	return m_currentClipRect;
}


