// SPDX-License-Identifier: GPL-3.0-only
#ifndef __KKROO_COLOR_H__
#define __KKROO_COLOR_H__

#include <cmath>

namespace math
{
		//	char to float
		//	255 / 255 = 1.F // ok
		//	0 / 255 - bad
		//	0 * some = 0
		//	255	* 0.00392156862745 = 0.99999999999975 // good
	constexpr f32 kkColorDivider = static_cast<f32>(0.00392156862745);
}

class kkColor
{
public:
	
	f32	m_data[ 4u ];

	kkColor()
	{
		m_data[ 0u ] = m_data[ 1u ] = m_data[ 2u ] = 0.f;
		m_data[ 3u ] = 1.f;
	}

	kkColor( f32 v )
	{
		m_data[ 0u ] = m_data[ 1u ] = m_data[ 2u ] = v;
		m_data[ 3u ] = 1.f;
	}

	kkColor( f32 r, f32 g, f32 b, f32 a = 1.f )
	{
		m_data[ 0u ] = r;
		m_data[ 1u ] = g;
		m_data[ 2u ] = b;
		m_data[ 3u ] = a;
	}

	kkColor( s32 r, s32 g, s32 b, s32 a = 255 )
	{
		this->setAsByteAlpha( a );
		this->setAsByteRed( r );
		this->setAsByteGreen( g );
		this->setAsByteBlue( b );
	}

	kkColor( u32 uint_data )
	{
		setAsInteger( uint_data );
	}

	const f32 * data() const { return &m_data[ 0u ]; }

	const f32 getRed() const   { return m_data[ 0u ]; }
	const f32 getGreen() const { return m_data[ 1u ]; }
	const f32 getBlue() const  { return m_data[ 2u ]; }
	const f32 getAlpha() const { return m_data[ 3u ]; }

	const u8 getAsByteRed() const   { return static_cast<u8>( m_data[ 0u ] * 255. ); }
	const u8 getAsByteGreen() const { return static_cast<u8>( m_data[ 1u ] * 255. ); }
	const u8 getAsByteBlue() const  { return static_cast<u8>( m_data[ 2u ] * 255. ); }
	const u8 getAsByteAlpha() const { return static_cast<u8>( m_data[ 3u ] * 255. ); }

	void normalize()
	{
		f32 len = std::sqrt(( m_data[ 0 ] * m_data[ 0 ] ) + (m_data[ 1 ] * m_data[ 1 ] ) + ( m_data[ 2 ] * m_data[ 2 ] ) );
		if(len > 0)
		{
			len = 1.0f/len;
		}
		m_data[0] *= len;
		m_data[1] *= len;
		m_data[2] *= len;
	}

	u32 getAsInteger()
	{
		return KK_MAKEFOURCC(
			this->getAsByteBlue(),
			this->getAsByteGreen(),
			this->getAsByteRed(),
			this->getAsByteAlpha()
		);
	}

	void setAlpha( f32 v ){ m_data[ 3u ] = v;	}
	void setRed( f32 v )  { m_data[ 0u ] = v;	}
	void setGreen( f32 v ){ m_data[ 1u ] = v;	}
	void setBlue( f32 v ) { m_data[ 2u ] = v;	}

	void setAsByteAlpha( s32 v ){ m_data[ 3u ] = static_cast<f32>(v) * math::kkColorDivider; }
	void setAsByteRed( s32 v )  { m_data[ 0u ] = static_cast<f32>(v) * math::kkColorDivider; }
	void setAsByteGreen( s32 v ){ m_data[ 1u ] = static_cast<f32>(v) * math::kkColorDivider; }
	void setAsByteBlue( s32 v ) { m_data[ 2u ] = static_cast<f32>(v) * math::kkColorDivider; }

		//	argb
		//	\param v: kk32u color
	void setAsInteger( u32 v )
	{
		this->setAsByteRed( static_cast<u8>( v >> 16u ) );
		this->setAsByteGreen( static_cast<u8>( v >> 8u ) );
		this->setAsByteBlue( static_cast<u8>( v ) );
		this->setAsByteAlpha( static_cast<u8>( v >> 24u ) );
	}

		// \param r: red component
		// \param g: green component
		// \param b: blue component
		// \param z: alpha component
	void set( f32 r, f32 g, f32 b, f32 a = 1. )
	{
		setAlpha( a );
		setRed( r );
		setGreen( g );
		setBlue( b );
	}

		// \param v: rgb components
		// \param a: alpha component
	void set( f32 v, f32 a = 1. )
	{
		setAlpha( a );
		setRed( v );
		setGreen( v );
		setBlue( v );
	}

	void operator+=( const kkColor& c )
	{
		m_data[0] += c.m_data[0];
		m_data[1] += c.m_data[1];
		m_data[2] += c.m_data[2];
		m_data[3] += c.m_data[3];
	}

	void operator*=( f32 v )
	{
		m_data[0] *= v;
		m_data[1] *= v;
		m_data[2] *= v;
		m_data[3] *= v;
	}
};

	//	HTML colors
const kkColor kkColorAliceBlue            = 0xffF0F8FF;
const kkColor kkColorAntiqueWhite         = 0xffFAEBD7;
const kkColor kkColorAqua                 = 0xff00FFFF;
const kkColor kkColorAquamarine           = 0xff7FFFD4;
const kkColor kkColorAzure                = 0xffF0FFFF;
const kkColor kkColorBeige                = 0xffF5F5DC;
const kkColor kkColorBisque               = 0xffFFE4C4;
const kkColor kkColorBlack                = 0xff000000;
const kkColor kkColorBlanchedAlmond       = 0xffFFEBCD;
const kkColor kkColorBlue                 = 0xff0000FF;
const kkColor kkColorBlueViolet           = 0xff8A2BE2;
const kkColor kkColorBrown                = 0xffA52A2A;
const kkColor kkColorBurlyWood            = 0xffDEB887;
const kkColor kkColorCadetBlue            = 0xff5F9EA0;
const kkColor kkColorChartreuse           = 0xff7FFF00;
const kkColor kkColorChocolate            = 0xffD2691E;
const kkColor kkColorCoral                = 0xffFF7F50;
const kkColor kkColorCornflowerBlue       = 0xff6495ED;
const kkColor kkColorCornsilk             = 0xffFFF8DC;
const kkColor kkColorCrimson              = 0xffDC143C;
const kkColor kkColorCyan                 = 0xff00FFFF;
const kkColor kkColorDarkBlue             = 0xff00008B;
const kkColor kkColorDarkCyan             = 0xff008B8B;
const kkColor kkColorDarkGoldenRod        = 0xffB8860B;
const kkColor kkColorDarkGray             = 0xffA9A9A9;
const kkColor kkColorDarkGrey             = 0xffA9A9A9;
const kkColor kkColorDarkGreen            = 0xff006400;
const kkColor kkColorDarkKhaki            = 0xffBDB76B;
const kkColor kkColorDarkMagenta          = 0xff8B008B;
const kkColor kkColorDarkOliveGreen       = 0xff556B2F;
const kkColor kkColorDarkOrange           = 0xffFF8C00;
const kkColor kkColorDarkOrchid           = 0xff9932CC;
const kkColor kkColorDarkRed              = 0xff8B0000;
const kkColor kkColorDarkSalmon           = 0xffE9967A;
const kkColor kkColorDarkSeaGreen         = 0xff8FBC8F;
const kkColor kkColorDarkSlateBlue        = 0xff483D8B;
const kkColor kkColorDarkSlateGray        = 0xff2F4F4F;
const kkColor kkColorDarkSlateGrey        = 0xff2F4F4F;
const kkColor kkColorDarkTurquoise        = 0xff00CED1;
const kkColor kkColorDarkViolet           = 0xff9400D3;
const kkColor kkColorDeepPink             = 0xffFF1493;
const kkColor kkColorDeepSkyBlue          = 0xff00BFFF;
const kkColor kkColorDimGray              = 0xff696969;
const kkColor kkColorDimGrey              = 0xff696969;
const kkColor kkColorDodgerBlue           = 0xff1E90FF;
const kkColor kkColorFireBrick            = 0xffB22222;
const kkColor kkColorFloralWhite          = 0xffFFFAF0;
const kkColor kkColorForestGreen          = 0xff228B22;
const kkColor kkColorFuchsia              = 0xffFF00FF;
const kkColor kkColorGainsboro            = 0xffDCDCDC;
const kkColor kkColorGhostWhite           = 0xffF8F8FF;
const kkColor kkColorGold                 = 0xffFFD700;
const kkColor kkColorGoldenRod            = 0xffDAA520;
const kkColor kkColorGray                 = 0xff808080;
const kkColor kkColorGrey                 = 0xff808080;
const kkColor kkColorGreen                = 0xff008000;
const kkColor kkColorGreenYellow          = 0xffADFF2F;
const kkColor kkColorHoneyDew             = 0xffF0FFF0;
const kkColor kkColorHotPink              = 0xffFF69B4;
const kkColor kkColorIndianRed            = 0xffCD5C5C;
const kkColor kkColorIndigo               = 0xff4B0082;
const kkColor kkColorIvory                = 0xffFFFFF0;
const kkColor kkColorKhaki                = 0xffF0E68C;
const kkColor kkColorLavender             = 0xffE6E6FA;
const kkColor kkColorLavenderBlush        = 0xffFFF0F5;
const kkColor kkColorLawnGreen            = 0xff7CFC00;
const kkColor kkColorLemonChiffon         = 0xffFFFACD;
const kkColor kkColorLightBlue            = 0xffADD8E6;
const kkColor kkColorLightCoral           = 0xffF08080;
const kkColor kkColorLightCyan            = 0xffE0FFFF;
const kkColor kkColorLightGoldenRodYellow = 0xffFAFAD2;
const kkColor kkColorLightGray            = 0xffD3D3D3;
const kkColor kkColorLightGrey            = 0xffD3D3D3;
const kkColor kkColorLightGreen           = 0xff90EE90;
const kkColor kkColorLightPink            = 0xffFFB6C1;
const kkColor kkColorLightSalmon          = 0xffFFA07A;
const kkColor kkColorLightSeaGreen        = 0xff20B2AA;
const kkColor kkColorLightSkyBlue         = 0xff87CEFA;
const kkColor kkColorLightSlateGray       = 0xff778899;
const kkColor kkColorLightSlateGrey       = 0xff778899;
const kkColor kkColorLightSteelBlue       = 0xffB0C4DE;
const kkColor kkColorLightYellow          = 0xffFFFFE0;
const kkColor kkColorLime                 = 0xff00FF00;
const kkColor kkColorLimeGreen            = 0xff32CD32;
const kkColor kkColorLinen                = 0xffFAF0E6;
const kkColor kkColorMagenta              = 0xffFF00FF;
const kkColor kkColorMaroon               = 0xff800000;
const kkColor kkColorMediumAquaMarine     = 0xff66CDAA;
const kkColor kkColorMediumBlue           = 0xff0000CD;
const kkColor kkColorMediumOrchid         = 0xffBA55D3;
const kkColor kkColorMediumPurple         = 0xff9370DB;
const kkColor kkColorMediumSeaGreen       = 0xff3CB371;
const kkColor kkColorMediumSlateBlue      = 0xff7B68EE;
const kkColor kkColorMediumSpringGreen    = 0xff00FA9A;
const kkColor kkColorMediumTurquoise      = 0xff48D1CC;
const kkColor kkColorMediumVioletRed      = 0xffC71585;
const kkColor kkColorMidnightBlue         = 0xff191970;
const kkColor kkColorMintCream            = 0xffF5FFFA;
const kkColor kkColorMistyRose            = 0xffFFE4E1;
const kkColor kkColorMoccasin             = 0xffFFE4B5;
const kkColor kkColorNavajoWhite          = 0xffFFDEAD;
const kkColor kkColorNavy                 = 0xff000080;
const kkColor kkColorOldLace              = 0xffFDF5E6;
const kkColor kkColorOlive                = 0xff808000;
const kkColor kkColorOliveDrab            = 0xff6B8E23;
const kkColor kkColorOrange               = 0xffFFA500;
const kkColor kkColorOrangeRed            = 0xffFF4500;
const kkColor kkColorOrchid               = 0xffDA70D6;
const kkColor kkColorPaleGoldenRod        = 0xffEEE8AA;
const kkColor kkColorPaleGreen            = 0xff98FB98;
const kkColor kkColorPaleTurquoise        = 0xffAFEEEE;
const kkColor kkColorPaleVioletRed        = 0xffDB7093;
const kkColor kkColorPapayaWhip           = 0xffFFEFD5;
const kkColor kkColorPeachPuff            = 0xffFFDAB9;
const kkColor kkColorPeru                 = 0xffCD853F;
const kkColor kkColorPink                 = 0xffFFC0CB;
const kkColor kkColorPlum                 = 0xffDDA0DD;
const kkColor kkColorPowderBlue           = 0xffB0E0E6;
const kkColor kkColorPurple               = 0xff800080;
const kkColor kkColorRebeccaPurple        = 0xff663399;
const kkColor kkColorRed                  = 0xffFF0000;
const kkColor kkColorRosyBrown            = 0xffBC8F8F;
const kkColor kkColorRoyalBlue            = 0xff4169E1;
const kkColor kkColorSaddleBrown          = 0xff8B4513;
const kkColor kkColorSalmon               = 0xffFA8072;
const kkColor kkColorSandyBrown           = 0xffF4A460;
const kkColor kkColorSeaGreen             = 0xff2E8B57;
const kkColor kkColorSeaShell             = 0xffFFF5EE;
const kkColor kkColorSienna               = 0xffA0522D;
const kkColor kkColorSilver               = 0xffC0C0C0;
const kkColor kkColorSkyBlue              = 0xff87CEEB;
const kkColor kkColorSlateBlue            = 0xff6A5ACD;
const kkColor kkColorSlateGray            = 0xff708090;
const kkColor kkColorSlateGrey            = 0xff708090;
const kkColor kkColorSnow                 = 0xffFFFAFA;
const kkColor kkColorSpringGreen          = 0xff00FF7F;
const kkColor kkColorSteelBlue            = 0xff4682B4;
const kkColor kkColorTan                  = 0xffD2B48C;
const kkColor kkColorTeal                 = 0xff008080;
const kkColor kkColorThistle              = 0xffD8BFD8;
const kkColor kkColorTomato               = 0xffFF6347;
const kkColor kkColorTurquoise            = 0xff40E0D0;
const kkColor kkColorViolet               = 0xffEE82EE;
const kkColor kkColorWheat                = 0xffF5DEB3;
const kkColor kkColorWhite                = 0xffffffff;
const kkColor kkColorWhiteSmoke           = 0xffF5F5F5;
const kkColor kkColorYellow               = 0xffFFFF00;
const kkColor kkColorYellowGreen          = 0xff9ACD32;

#endif