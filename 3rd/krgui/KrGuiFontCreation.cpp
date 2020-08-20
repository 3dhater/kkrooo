#include "KrGui.h"

#ifdef KRGUI_PLATFORM_WINDOWS
#include <Windows.h>
#include <Windowsx.h>
#endif

using namespace Kr;//Gui;


Gui::Font* Gui::GuiSystem::createFontFromTexture(unsigned long long * texture, int textureCount, int textureWidth, int textureHeight)
{
	auto oldFont = m_currentFont;
	m_currentFont = new Gui::Font(this);

	if( !m_currentFont )
	{
		m_currentFont = oldFont;
		return nullptr;
	}

	for( int i = 0; i < textureCount; ++i )
	{
		Gui::Texture * t = new Gui::Texture;
		t->height = textureHeight;
		t->width = textureWidth;
		t->texture_address = texture[i];
		t->can_destroy = false;
		m_currentFont->addTexture(t);
	}

	m_fontCache.push_back(m_currentFont);
	return m_currentFont;
}

Gui::Font* Gui::GuiSystem::createFontFromDDS_RGBA8( const char16_t* fileName )
{
	assert(fileName);

	auto oldFont = m_currentFont;
	m_currentFont = new Gui::Font(this);
	if( !m_currentFont )
	{
		m_currentFont = oldFont;
		return m_currentFont;
	}

	if(!std::filesystem::exists(fileName))
		return m_currentFont;
	
	Image img;
	std::string s;
	std::u16string tn(fileName);
	_internal::UTF16_to_UTF8(tn, s);
	if( this->loadImageFromFile(&img, s.data()) ){
		m_currentFont->addTexture(Gui::GraphicsSystem_OpenGL3CreateTexture(&img));
	}
	return m_currentFont;
}

Gui::Font* Gui::GuiSystem::createFontFromSystem
(
	const char16_t* fontName,
	unsigned int fontSize,
	bool bold,
	bool italic,
	bool aa,
	bool alpha,
	bool saveToFile,
	const char * fileDir,
	const char * fileName
)
{
	auto oldFont = m_currentFont;
	m_currentFont = new Gui::Font(this);
	if( !m_currentFont )
	{
		m_currentFont = oldFont;
		return m_currentFont;
	}

	// fixed size
	// just create a lot of small textures...
	const int textureWidth  = 64;
	const int textureHeight = 64;
	Image image;
	image.m_width  = textureWidth;
	image.m_height = textureHeight;
	image.m_pitch  = image.m_width * 4;
	image.m_dataSize = image.m_pitch * image.m_height;
	image.m_data = new Image::_byte[image.m_dataSize];

#ifdef KRGUI_PLATFORM_WINDOWS
	HDC dc = CreateDC(L"DISPLAY", L"DISPLAY", 0 ,0 );
	HFONT font = CreateFontW(
		-MulDiv(fontSize, GetDeviceCaps(dc, LOGPIXELSY), 72), 0,
		0,0,
		bold ? FW_SEMIBOLD : 0,
		italic, 0,0, ANSI_CHARSET, 0,0,
		aa ? ANTIALIASED_QUALITY : 0,
		0, (const wchar_t*)fontName );
	if(!font)
	{
		delete m_currentFont;
		m_currentFont = oldFont;
		return m_currentFont;
	}
	auto err = GetLastError();
	SelectObject( dc, font );
	SetTextAlign( dc, TA_LEFT | TA_TOP | TA_NOUPDATECP );

	char* buf = new char[GetFontUnicodeRanges( dc, 0 )];
	LPGLYPHSET glyphs = (LPGLYPHSET)buf;
	GetFontUnicodeRanges( dc, glyphs );

	struct FontImage
	{
		std::vector<CharInfo> chars;
	};
	std::vector<FontImage*> imgs;
	//chars.reserve(glyphs->cGlyphsSupported);
	Vec2f leftTop(0.f,0.f);
	int textureSlot = 0;
	float maxy = 0.f;
	float oldmaxy = 0.f;
	FontImage* fntImg = nullptr;
	for( DWORD range = 0; range < glyphs->cRanges; ++range )
	{
		WCRANGE* currentRange = &glyphs->ranges[range];
		for( WCHAR ch = currentRange->wcLow; ch < currentRange->wcLow + currentRange->cGlyphs; ++ch )
		{
			if(!fntImg)
				fntImg = new FontImage;
			wchar_t currentChar = ch;
			SIZE size;
			ABC abc;
			
			CharInfo charInfo;
			charInfo.symbol = ch;
			charInfo.underhang = 0.f;
			charInfo.overhang  = 0.f;

			if( IsDBCSLeadByte((BYTE) ch) ) continue;
			GetTextExtentPoint32W( dc, &currentChar, 1, &size );
			if( GetCharABCWidthsW( dc, currentChar, currentChar, &abc ) )
			{
				size.cx = abc.abcB;
				charInfo.underhang = (float)abc.abcA;
				charInfo.overhang  = (float)abc.abcC;
				if( abc.abcB - abc.abcA + abc.abcC < 1 ) continue; 
			}
			if( size.cy < 1 ) continue;

			//size.cx = abc.abcB;

			charInfo.rect = Vec4f(
				leftTop.x, 
				leftTop.y, 
				leftTop.x + (float)size.cx, 
				leftTop.y + (float)size.cy );
			charInfo.textureSlot = textureSlot;

			if( leftTop.x + size.cx > (float)textureWidth )
			{
				leftTop.x = 0.f;
				leftTop.y += maxy;
				charInfo.rect = Vec4f(
					leftTop.x, 
					leftTop.y, 
					leftTop.x + (float)size.cx , 
					leftTop.y + (float)size.cy );
				
				maxy = 0.f;
			}
	
			if( leftTop.y + oldmaxy > textureHeight )
			{
				imgs.push_back(fntImg);
				fntImg = nullptr;
				++textureSlot;
				leftTop.y = 0.f;
				--ch;
			}

			if( fntImg )
				fntImg->chars.emplace_back(charInfo);

			leftTop.x += (float)size.cx + 1.f;

			if( (float)size.cy + 1.f > maxy )
				maxy = (float)size.cy + 1.f;

			oldmaxy = maxy;
		}
	}

	if( fntImg )
	{
		imgs.push_back(fntImg);
	}

	//std::basic_ofstream<char16_t, std::char_traits<char16_t>> out;//(L"test.txt");
	_internal::file_io file;
	if( saveToFile && fileName )
	{
		std::string fileNameString = fileDir;

		if(!std::filesystem::exists(fileDir))
			std::filesystem::create_directories(fileDir);

		fileNameString += fileName;
		file.open(fileNameString.c_str(), "wb");
		// Little Edndian UTF-16 BOM
		file.writeByte(0xFF);
		file.writeByte(0xFE);

		// num of textures
		file.writeUnsignedIntAsUTF16LE((unsigned int)imgs.size());

		// '\n'
		file.writeByte(0x0A);
		file.writeByte(0x00);

		for( size_t i = 0, sz = imgs.size(); i < sz; ++i )
		{
			// file name
			char filenameBuf[256];
			sprintf( filenameBuf, "%s%i.dds", fileName, (int)i );

			file.writeANSIStringAsUTF16LE(filenameBuf);

			// '\n'
			file.writeByte(0x0A);
			file.writeByte(0x00);
		}
		// info
		file.writeANSIStringAsUTF16LE("# symbol leftTopX leftTopY rightBottomX rightBottomY underhang overhang textureId");
		// '\n'
		file.writeByte(0x0A);
		file.writeByte(0x00);
	}
	// -----------------------------
	for( size_t i = 0, sz = imgs.size(); i < sz; ++i )
	{
		auto fntImg = imgs[i];
	
		HBITMAP bmp = CreateCompatibleBitmap( dc, textureWidth, textureHeight );
		HDC bmpdc = CreateCompatibleDC( dc );
		LOGBRUSH lbrush;
		lbrush.lbHatch = 0;
		lbrush.lbStyle = BS_SOLID;
		lbrush.lbColor = RGB(0,0,0);
		HBRUSH brush = CreateBrushIndirect(&lbrush);
		HPEN pen = CreatePen(PS_NULL, 0, 0);

		SelectObject(bmpdc, bmp);
		SelectObject(bmpdc, pen);
		SelectObject(bmpdc, brush);
		SelectObject(bmpdc, font);
		
		SetTextColor( bmpdc, RGB(255,255,255) );
		Rectangle( bmpdc, 0,0,textureWidth,textureHeight );
		SetBkMode( bmpdc, TRANSPARENT );

		for( size_t j = 0, jsz = fntImg->chars.size(); j < jsz; ++j )
		{
			auto & char_info = fntImg->chars[j];

			m_currentFont->addCharacter(
				char_info.symbol, 
				char_info.rect, 
				char_info.textureSlot, 
				char_info.underhang, 
				char_info.overhang, 
				textureWidth, 
				textureHeight );
			
			if( saveToFile && fileName )
			{
				// symbol
				file.writeChar16((char16_t)char_info.symbol);

				// ' '
				file.writeByte(0x20);
				file.writeByte(0x00);

				// leftTopX
				file.writeUnsignedIntAsUTF16LE((unsigned int)char_info.rect.x);
				file.writeByte(0x20);
				file.writeByte(0x00);

				// leftTopY
				file.writeUnsignedIntAsUTF16LE((unsigned int)char_info.rect.y);
				file.writeByte(0x20);
				file.writeByte(0x00);

				// rightBottomX
				file.writeUnsignedIntAsUTF16LE((unsigned int)char_info.rect.z);
				file.writeByte(0x20);
				file.writeByte(0x00);

				// rightBottomY
				file.writeUnsignedIntAsUTF16LE((unsigned int)char_info.rect.w);
				file.writeByte(0x20);
				file.writeByte(0x00);

				// underhang
				file.writeIntAsUTF16LE((unsigned int)char_info.underhang);
				file.writeByte(0x20);
				file.writeByte(0x00);

				// overhang
				file.writeIntAsUTF16LE((unsigned int)char_info.overhang);
				file.writeByte(0x20);
				file.writeByte(0x00);

				// textureId
				file.writeUnsignedIntAsUTF16LE((unsigned int)char_info.textureSlot);
				file.writeByte(0x20);
				file.writeByte(0x00);

				// '\n'
				file.writeByte(0x0A);
				file.writeByte(0x00);
			}

			TextOutW(bmpdc, (int)(char_info.rect.x - char_info.underhang),
				(int)char_info.rect.y, &char_info.symbol, 1);
		}

		BITMAP b;
		GetObject(bmp, sizeof(BITMAP), (LPSTR)&b);
		WORD cClrBits = (WORD)(b.bmPlanes * b.bmBitsPixel);
		PBITMAPINFO pbmi = (PBITMAPINFO) LocalAlloc(LPTR,
			sizeof(BITMAPINFOHEADER));
		pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pbmi->bmiHeader.biWidth = b.bmWidth;
		pbmi->bmiHeader.biHeight = b.bmHeight;
		pbmi->bmiHeader.biPlanes = b.bmPlanes;
		pbmi->bmiHeader.biBitCount = b.bmBitsPixel;
		pbmi->bmiHeader.biCompression = BI_RGB;
		pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
				* pbmi->bmiHeader.biHeight;
		pbmi->bmiHeader.biClrImportant = 0;

		LPBYTE lpBits;
		PBITMAPINFOHEADER pbih = (PBITMAPINFOHEADER)pbmi;
		lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);
		auto r = GetDIBits(dc, bmp, 0, (WORD) pbih->biHeight, lpBits, pbmi, DIB_RGB_COLORS);
		int rowsize = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8;
		char *row = new char[rowsize];
		for( int i = 0; i < (pbih->biHeight/2); ++i )
		{
			memcpy(row, lpBits + (rowsize * i), rowsize);
			memcpy(lpBits + (rowsize * i), lpBits + ((pbih->biHeight-1 -i) * rowsize ) , rowsize);
			memcpy(lpBits + ((pbih->biHeight-1 -i) * rowsize ), row , rowsize);
		}
		auto imageData = image.m_data;
		if( cClrBits > 24 )
		{
			for( LPBYTE m = lpBits; m < lpBits + pbih->biSizeImage; m+=4 )
			{
				*imageData = m[0]; ++imageData;  // R
				*imageData = m[1]; ++imageData;
				*imageData = m[2]; ++imageData;

				unsigned char A = 0;

				if( m[0] > 0 )
				{
					A = 255 - (255 - m[0]);
				}

				*imageData = A; ++imageData; // A
			}
		}
		if( saveToFile && fileName )
		{
			char filenameBuf[256];
			sprintf( filenameBuf, "%s%s%i", fileDir, fileName, (int)i );
			saveImageToFile(&image, filenameBuf, ".dds");
		}
		//memcpy( image.m_data, lpBits, image.m_dataSize );
		delete [] row;
		LocalFree(pbmi);
		GlobalFree(lpBits);
		
		// new texture
		switch (m_gsType)
		{
		case Gui::GraphicsSystemType::OpenGL3:
		{
			m_currentFont->addTexture(Gui::GraphicsSystem_OpenGL3CreateTexture(&image));
		}break;
		default:
			printf("IMPLEMENT!!! %i\n", KRGUI_LINE );
			break;
		}
		DeleteDC(bmpdc);
		DeleteObject(brush);
		DeleteObject(pen);
		DeleteObject(bmp);
	}


	DeleteObject(font);
	DeleteDC(dc);
	delete [] buf;
#else
#error Please implement me
#endif

	m_fontCache.push_back(m_currentFont);
	return m_currentFont;
}

Gui::Font* Gui::GuiSystem::createFontFromFile(const char * dir, const char * fileName)
{
	assert(fileName);
	assert(dir);
	if(!std::filesystem::exists(dir))
		return m_currentFont;

	std::string filePath = dir;
	filePath += fileName;

	// read .txt file with font information
	_internal::file_io file;
	file.open(filePath.c_str(), "rb");
	if( !file.isOpen() )
		return m_currentFont;

	if( !file.isUTF16LE() )
		return m_currentFont;

	std::u16string str;

	// read num of textures
	file.readWordFromUTF16LE(str);
	int num_of_textures = _internal::to_int(str.data(), (int)str.size());
	if(num_of_textures<1)
		return m_currentFont;

	std::vector<std::u16string> texture_name_array;
	texture_name_array.reserve(num_of_textures);
	for( int i = 0; i < num_of_textures; ++i ){
		file.get_lineUTF16LE(str);
		texture_name_array.emplace_back(str);
	}

	if(!texture_name_array.size())
		return m_currentFont;

	// skip comment
	file.get_lineUTF16LE(str);

	// get char info
	std::vector<CharInfo> char_info_array;
	while(!file.isEOF())
	{
		CharInfo ci;

		bool is_new_line = false;
		int integer = 0;

		// symbol
		file.readWordFromUTF16LE(str, false, false, false, false, false, false, &is_new_line);
		if(str.size()) ci.symbol = str[0];
		if( is_new_line ) continue;
		
		// leftTopX
		file.readWordFromUTF16LE(str, false, false, false, false, false, false, &is_new_line);
		if(str.size()) ci.rect.x = (float)_internal::to_int(str.data(),(int)str.size());
		if( is_new_line ) continue;
	
		// leftTopY
		file.readWordFromUTF16LE(str, false, false, false, false, false, false, &is_new_line);
		if(str.size()) ci.rect.y = (float)_internal::to_int(str.data(),(int)str.size());
		if( is_new_line ) continue;

		// rightBottomX
		file.readWordFromUTF16LE(str, false, false, false, false, false, false, &is_new_line);
		if(str.size()) ci.rect.z = (float)_internal::to_int(str.data(),(int)str.size());
		if( is_new_line ) continue;

		// rightBottomY
		file.readWordFromUTF16LE(str, false, false, false, false, false, false, &is_new_line);
		if(str.size()) ci.rect.w = (float)_internal::to_int(str.data(),(int)str.size());
		if( is_new_line ) continue;

		// underhang
		file.readWordFromUTF16LE(str, false, false, false, false, false, false, &is_new_line);
		if(str.size()) ci.underhang = (float)_internal::to_int(str.data(),(int)str.size());
		if( is_new_line ) continue;

		// overhang
		file.readWordFromUTF16LE(str, false, false, false, false, false, false, &is_new_line);
		if(str.size()) ci.overhang = (float)_internal::to_int(str.data(),(int)str.size());
		if( is_new_line ) continue;

		// textureId
		file.readWordFromUTF16LE(str, false, false, false, false, false, false, &is_new_line);
		if(str.size()) ci.textureSlot = _internal::to_int(str.data(),(int)str.size());

		char_info_array.emplace_back(ci);
	}

	if( char_info_array.size() )
	{
		auto oldFont = m_currentFont;
		m_currentFont = new Gui::Font(this);
		if( !m_currentFont )
		{
			m_currentFont = oldFont;
			return m_currentFont;
		}

		for( auto & ci : char_info_array )
		{
			m_currentFont->addCharacter(ci.symbol, 
				ci.rect, ci.textureSlot, ci.underhang, ci.overhang, 64, 64
			);
		}

		for( auto & tn : texture_name_array )
		{
			Image img;
			std::string s;
			_internal::UTF16_to_UTF8(tn, s);

			filePath = dir;
			filePath += s;
			if( this->loadImageFromFile(&img, filePath.data()) ){
				m_currentFont->addTexture(Gui::GraphicsSystem_OpenGL3CreateTexture(&img));
			}
		}
	}

	return m_currentFont;
}