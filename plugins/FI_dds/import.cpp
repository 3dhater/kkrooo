// SPDX-License-Identifier: GPL-3.0-only
#include "PluginSDK.h"

#ifdef KK_PLATFORM_WINDOWS
#pragma comment(lib, "..\\..\\3rd\\FreeImage\\lib\\x64\\FreeImage.lib")
#endif

#include "FreeImage.h"

void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message)
{
	printf("\n*** "); 
	if(fif != FIF_UNKNOWN) {
		printf("%s Format\n", FreeImage_GetFormatFromFIF(fif));
	}
	printf(message);
	printf(" ***\n");
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
						fprintf(stderr,"DDS: unsupported format :(\n"); // пока так
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
					printf("Failed to load dds... :(\n");
				}
			}
        }

		return image;
	}
	
}