#ifndef __KK_KRGUI_FILE_IO_H__
#define __KK_KRGUI_FILE_IO_H__

namespace Kr
{
	namespace Gui
	{
		namespace _internal
		{
			class file_io
			{
				FILE * m_file = nullptr;
				bool m_isOpen = false;
			public:
				file_io(){}
				~file_io(){if(m_isOpen)close();}
				bool isEOF(){if(m_isOpen){return feof(m_file) != 0;}return true;}
				void close(){if(m_isOpen){fclose(m_file);m_file=nullptr;m_isOpen=false;}}
				bool isOpen(){return m_isOpen;}
				bool open(const char* fileName, const char* charMode){
					assert(fileName);
					assert(charMode);
					close();
					m_file = fopen(fileName,charMode);
					if( m_file ) m_isOpen = true;
					return m_isOpen;
				}
				size_t writeByte(char b){
					if( m_isOpen ) return fwrite(&b,1,1,m_file);
					return 0;
				}
				size_t writeByte(int i){
					if( m_isOpen ) return fwrite(&i,1,1,m_file);
					return 0;
				}
				size_t writeUnsignedInt(unsigned int i){
					if( m_isOpen ) return fwrite(&i,1,4,m_file);
					return 0;
				}
				size_t writeIntAsUTF16LE(int i){
					size_t r=0;
					if( m_isOpen )
					{
						char buf[32];
						sprintf(buf, "%i", i);
						r = writeANSIStringAsUTF16LE(buf);
					}
					return r;
				}
				size_t writeUnsignedIntAsUTF16LE(unsigned int i){
					size_t r=0;
					if( m_isOpen )
					{
						char buf[32];
						sprintf(buf, "%u", i);
						r = writeANSIStringAsUTF16LE(buf);
					}
					return r;
				}
				size_t writeANSIStringAsUTF16LE(const char* str){
					size_t r=0;
					if( m_isOpen )
					{
						auto len = strlen(str);
						for(size_t i = 0; i < len; ++i)
						{
							char c = str[i];
							r += writeByte(c);
							r += writeByte(0x00);
						}
					}
					return r;
				}
				size_t writeChar16(char16_t c){
					if( m_isOpen ) return fwrite(&c,1,2,m_file);
					return 0;
				}
				size_t writeBytes(void* bytes, size_t numOfBytes){
					if( m_isOpen ) return fwrite(bytes,1,numOfBytes,m_file);
					return 0;
				}
				size_t skipLine()
				{
					size_t readNum=0;
					if( m_isOpen )
					{
						unsigned char buf[2];
						while(!feof(m_file))
						{
							auto rn = fread(buf, 1, 2, m_file);
							readNum += rn;
							if( rn < 2 )
								return readNum;

							char16_t currentChar = buf[1];
							currentChar <<= 8;
							currentChar |= buf[0];
							if(currentChar == u'\n')
								break;
						}
					}
					return readNum;
				}
				size_t readWordFromUTF16LE(
					std::u16string& outString, 
					bool withoutAlphas = false, 
					bool withoutDigits = false, 
					bool withoutDots = false, 
					bool withoutCommas = false, 
					bool withoutPlusAndMinus = false,
					bool withoutAnyOtheSymbols = false,
					bool * isNewLine = nullptr)
				{
					size_t readNum=0;
					if( m_isOpen )
					{
						outString.clear();
						bool contain = false;

						// read 2 bytes
						unsigned char buf[2];
						while(!feof(m_file))
						{
							auto rn = fread(buf, 1, 2, m_file);
							readNum += rn;
							if( rn < 2 )
								return readNum;

							char16_t currentChar = buf[1];
							currentChar <<= 8;
							currentChar |= buf[0];

							if(isNewLine){
								*isNewLine = false;
								//if( currentChar == u'\n' )
									//*isNewLine = true;
							}

							if( currentChar == u'\n' )
							{
								if(isNewLine)  *isNewLine = true;
								return readNum;
							}

							if( contain && is_space(currentChar) )
								return readNum;

						
							if( is_digit(currentChar) && !withoutDigits )
								outString.push_back(currentChar);
							else if( is_alpha(currentChar) && !withoutAlphas )
								outString.push_back(currentChar);
							else if( currentChar == u'-' && !withoutPlusAndMinus )
								outString.push_back(currentChar);
							else if( currentChar == u'+' && !withoutPlusAndMinus )
								outString.push_back(currentChar);
							else if( currentChar == u'.' && !withoutPlusAndMinus )
								outString.push_back(currentChar);
							else if( currentChar == u',' && !withoutPlusAndMinus )
								outString.push_back(currentChar);
							else if( !withoutAnyOtheSymbols )
								outString.push_back(currentChar);

							if( outString.size() )
								contain = true;
						}
					}
					return readNum;
				}
				bool isUTF16LE(){
					if( m_isOpen )
					{
						fseek(m_file, 0, SEEK_SET);
						unsigned char buf[2];
						if( fread(buf,1,2,m_file) == 2 ){
							if( buf[0] == 0xff && buf[1] == 0xfe )
								return true;
							else
								fseek(m_file, 0, SEEK_SET);
							return false;
						}
					}
					return false;
				}
				size_t get_lineUTF16LE(
					std::u16string& outString){
					size_t readNum=0;
					if( m_isOpen )
					{
						outString.clear();
						bool contain = false;

						// read 2 bytes
						unsigned char buf[2];
						while(!feof(m_file))
						{
							auto rn = fread(buf, 1, 2, m_file);
							readNum += rn;
							if( rn < 2 )
								return readNum;

							char16_t currentChar = buf[1];
							currentChar <<= 8;
							currentChar |= buf[0];

							if( currentChar == u'\n' )
								break;
							else
								outString.push_back(currentChar);
						}
					}
					return readNum;
				}
				size_t readUnsignedInt(unsigned int & dds_magic){
					size_t r=0;
					if( m_isOpen )
					{
						r = fread(&dds_magic, 1, 4, m_file);
					}
					return r;
				}
				size_t readBytes(void* bytes, size_t numOfBytes){
					size_t r=0;
					if( m_isOpen )
					{
						r = fread(bytes, 1, numOfBytes, m_file);
					}
					return r;
				}
			};
		}
	}
}

#endif