// SPDX-License-Identifier: GPL-3.0-only
#include "PluginSDK.h"

#ifdef KK_PLATFORM_WINDOWS
#pragma comment(lib, "..\\..\\3rd\\FreeImage\\lib\\x64\\FreeImage.lib")
#endif

#include "FreeImage.h"
//#include "Utilities.h"

void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message)
{
	printf("\n*** "); 
	if(fif != FIF_UNKNOWN) {
		printf("%s Format\n", FreeImage_GetFormatFromFIF(fif));
	}
	printf(message);
	printf(" ***\n");
}

template <class T> void INPLACESWAP(T& a, T& b) {
	a ^= b; b ^= a; a ^= b;
}
BOOL SwapRedBlue32(FIBITMAP* dib) {
	if(FreeImage_GetImageType(dib) != FIT_BITMAP) {
		return FALSE;
	}
		
	const unsigned bytesperpixel = FreeImage_GetBPP(dib) / 8;
	if(bytesperpixel > 4 || bytesperpixel < 3) {
		return FALSE;
	}
		
	const unsigned height = FreeImage_GetHeight(dib);
	const unsigned pitch = FreeImage_GetPitch(dib);
	const unsigned lineSize = FreeImage_GetLine(dib);
	
	BYTE* line = FreeImage_GetBits(dib);
	for(unsigned y = 0; y < height; ++y, line += pitch) {
		for(BYTE* pixel = line; pixel < line + lineSize ; pixel += bytesperpixel) {
			INPLACESWAP(pixel[0], pixel[2]);
		}
	}
	
	return TRUE;
}

extern "C" 
{

	KK_API kkImage* KK_C_DECL kkPlugin_onImportImage(kkPluginCommonInterface * CI, const char16_t* file_path )
	{
		kkImage* image = nullptr;
		
		if( CI->FSFileExist( file_path ) )
        {
            FreeImage_SetOutputMessage(FreeImageErrorHandler);

		    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

			fif = FreeImage_GetFileTypeU( (wchar_t*)file_path, 0);
			if(fif == FIF_UNKNOWN)
			{
				fif = FreeImage_GetFIFFromFilenameU((wchar_t*)file_path);
			}
			if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) 
			{
				FIBITMAP *dib = FreeImage_LoadU(fif, (wchar_t*)file_path, 0);

				if( dib )
				{
					image = CI->CreateImage();

					FIBITMAP * dib32 = FreeImage_ConvertTo32Bits(dib);
					FreeImage_Unload(dib);
					dib = dib32;

					image->m_bits    = FreeImage_GetBPP(dib);
					image->m_height  = FreeImage_GetHeight(dib);
					image->m_width   = FreeImage_GetWidth(dib);
					image->m_pitch   = FreeImage_GetPitch(dib);

					FREE_IMAGE_TYPE fit = FreeImage_GetImageType(dib);

					switch(fit)
					{
					default:
					case FIT_UNKNOWN:
					case FIT_UINT16:
					case FIT_INT16:
					case FIT_UINT32:
					case FIT_INT32:
					case FIT_FLOAT:
					case FIT_DOUBLE:
					case FIT_COMPLEX:
					case FIT_RGB16:
					case FIT_RGBA16:
					case FIT_RGBF:
					case FIT_RGBAF:
						fprintf(stderr,"PNG: unsupported format :(\n"); // пока так
						break;
					case FIT_BITMAP:
					{
						FreeImage_FlipVertical(dib); // почему-то png файлы грузит перевёрнутыми
						image->m_dataSize = image->m_pitch * image->m_height;
						image->m_data8    = CI->MemAlloc(image->m_dataSize);
						memcpy(&image->m_data8[0], FreeImage_GetBits(dib), image->m_dataSize ); 
						auto data = image->m_data8;
						for( int h = 0; h < image->m_height; ++h )
						{
							for( int w = 0; w < image->m_width; ++w )
							{
								auto pixel  = data;
								auto blue   = *pixel; ++pixel;
								auto green  = *pixel; ++pixel;
								auto red    = *pixel; ++pixel;
								auto alpha  = *pixel; ++pixel;

								*data = red;   ++data;
								*data = green; ++data;
								*data = blue;  ++data;
								*data = alpha; ++data;
							}
						}
					}
						break;
					}

					FreeImage_Unload(dib);
				}
				else
				{
					printf("Failed to load png... :(\n");
				}
			}
        }

		return image;
	}
	
	KK_API void KK_C_DECL kkPlugin_onExportImage(kkPluginCommonInterface * CI, const char16_t* file_path, kkImage * image )
	{
		if( image )
		{
			//FIBITMAP * dib = FreeImage_Allocate(image->m_width, image->m_height, 32);
			FIBITMAP * dib = FreeImage_ConvertFromRawBits(image->m_data8, image->m_width, image->m_height,
				image->m_pitch, image->m_bits, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE );
			
			if( dib )
			{
				/*for( u32 i = 0; i < image->m_width; ++i)
				{
					for( u32 j = 0; j < image->m_height; ++j)
					{
						kkColor imagePixelColor = image->getPixelColor(i,j);
						RGBQUAD color;
						color.rgbRed = imagePixelColor.getAsByteRed();
						color.rgbGreen = imagePixelColor.getAsByteGreen();
						color.rgbBlue = imagePixelColor.getAsByteBlue();
						color.rgbReserved = imagePixelColor.getAsByteAlpha();
						FreeImage_SetPixelColor( dib, i, j, &color );
					}
				}*/
				SwapRedBlue32(dib);
				FreeImage_SaveU(FREE_IMAGE_FORMAT::FIF_PNG, dib, (wchar_t*)file_path, 0);
				FreeImage_Unload(dib);
			}
		}
	}
}