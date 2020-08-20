// SPDX-License-Identifier: GPL-3.0-only
#define KRGUI_DEFAULT_FONT_FILE_PATH "../res/gui/defaultFont.data"
#include "kkrooo.engine.h"

#include "Application.h"
#include "Functions.h"
#include "Materials/MaterialEditor.h"
#include "GUI/GUIResources.h"

using namespace Kr;

bool Application::_initFonts()
{
	unsigned long long smallFontTextures[1];
    smallFontTextures[ 0 ] = (unsigned long long)m_guiResources->m_smallFontTexture->getHandle();
    m_smallFont = m_KrGuiSystem->createFontFromTexture(smallFontTextures,1,481,14);
    if(!m_smallFont)
    {
        KK_PRINT_FAILED;
		return false;
    }
    unsigned long long microFontTextures[1];
    microFontTextures[ 0 ] = (unsigned long long)m_guiResources->m_microFontTexture->getHandle();
    m_microFont = m_KrGuiSystem->createFontFromTexture(microFontTextures,1,436,9);
    if(!m_microFont)
    {
        KK_PRINT_FAILED;
		return false;
    }
	unsigned long long iconsFontTextures[1];
    iconsFontTextures[ 0 ] = (unsigned long long)m_guiResources->m_blenderIcons->getHandle();
    m_iconsFont = m_KrGuiSystem->createFontFromTexture(iconsFontTextures,1,602,640);
    if(!m_iconsFont)
    {
        KK_PRINT_FAILED;
		return false;
    }

    m_KrGuiSystem->setCurrentFont(nullptr);

    auto font = m_smallFont;
	font->addCharacter(u'A', Gui::Vec4f(1,0,8,13), 0, 0, 0, 481, 14 );   font->addCharacter(u'B', Gui::Vec4f(8,0,8,14), 0, 0, 0, 481, 14 );
    font->addCharacter(u'C', Gui::Vec4f(14,0,20,13), 0, 0, 0, 481, 14 ); font->addCharacter(u'D', Gui::Vec4f(20,0,26,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'E', Gui::Vec4f(26,0,31,13), 0, 0, 0, 481, 14 ); font->addCharacter(u'F', Gui::Vec4f(31,0,36,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'G', Gui::Vec4f(36,0,42,13), 0, 0, 0, 481, 14 ); font->addCharacter(u'H', Gui::Vec4f(42,0,48,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'I', Gui::Vec4f(48,0,52,13), 0, 0, 0, 481, 14 ); font->addCharacter(u'J', Gui::Vec4f(52,0,56,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'K', Gui::Vec4f(56,0,62,13), 0, 0, 0, 481, 14 ); font->addCharacter(u'L', Gui::Vec4f(62,0,67,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'M', Gui::Vec4f(67,0,75,13), 0, 0, 0, 481, 14 ); font->addCharacter(u'N', Gui::Vec4f(75,0,81,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'O', Gui::Vec4f(81,0,88,13), 0, 0, 0, 481, 14 ); font->addCharacter(u'P', Gui::Vec4f(88,0,93,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'Q', Gui::Vec4f(93,0,100,13), 0, 0, 0, 481, 14 ); font->addCharacter(u'R', Gui::Vec4f(100,0,106,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'S', Gui::Vec4f(106,0,111,13), 0, 0, 0, 481, 14 ); font->addCharacter(u'T', Gui::Vec4f(111,0,117,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'U', Gui::Vec4f(117,0,123,13), 0, 0, 0, 481, 14 ); font->addCharacter(u'V', Gui::Vec4f(123,0,130,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'W', Gui::Vec4f(130,0,140,13), 0, 0, 0, 481, 14 ); font->addCharacter(u'X', Gui::Vec4f(140,0,146,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'Y', Gui::Vec4f(146,0,152,13), 0, 0, 0, 481, 14 ); font->addCharacter(u'Z', Gui::Vec4f(152,0,158,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'a', Gui::Vec4f(158,0,164,13), 0, 1, 0, 481, 14 );    font->addCharacter(u'b', Gui::Vec4f(164,0,169,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'c', Gui::Vec4f(169,0,173,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'd', Gui::Vec4f(173,0,178,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'e', Gui::Vec4f(178,0,183,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'f', Gui::Vec4f(183,0,188,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'g', Gui::Vec4f(188,0,193,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'h', Gui::Vec4f(193,0,198,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'i', Gui::Vec4f(198,0,200,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'j', Gui::Vec4f(200,0,204,13), 0, 2, 0, 481, 14 );
    font->addCharacter(u'k', Gui::Vec4f(204,0,209,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'l', Gui::Vec4f(209,0,211,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'm', Gui::Vec4f(211,0,219,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'n', Gui::Vec4f(219,0,224,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'o', Gui::Vec4f(224,0,229,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'p', Gui::Vec4f(229,0,234,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'q', Gui::Vec4f(234,0,239,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'r', Gui::Vec4f(239,0,243,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u's', Gui::Vec4f(243,0,248,13), 0, 0, 0, 481, 14 );    font->addCharacter(u't', Gui::Vec4f(248,0,252,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'u', Gui::Vec4f(252,0,257,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'v', Gui::Vec4f(257,0,263,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'w', Gui::Vec4f(263,0,271,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'x', Gui::Vec4f(271,0,277,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'y', Gui::Vec4f(277,0,283,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'z', Gui::Vec4f(283,0,288,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'0', Gui::Vec4f(288,0,293,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'1', Gui::Vec4f(293,0,296,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'2', Gui::Vec4f(296,0,301,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'3', Gui::Vec4f(301,0,307,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'4', Gui::Vec4f(307,0,314,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'5', Gui::Vec4f(314,0,319,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'6', Gui::Vec4f(319,0,324,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'7', Gui::Vec4f(324,0,329,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'8', Gui::Vec4f(329,0,334,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'9', Gui::Vec4f(334,0,339,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'_', Gui::Vec4f(339,0,344,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'-', Gui::Vec4f(344,0,348,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'+', Gui::Vec4f(348,0,353,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'=', Gui::Vec4f(353,0,358,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'~', Gui::Vec4f(358,0,363,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'`', Gui::Vec4f(363,0,366,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'!', Gui::Vec4f(366,0,368,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'?', Gui::Vec4f(368,0,373,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'#', Gui::Vec4f(373,0,380,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'№', Gui::Vec4f(380,0,390,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'$', Gui::Vec4f(390,0,395,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'%', Gui::Vec4f(395,0,403,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'^', Gui::Vec4f(403,0,409,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'&', Gui::Vec4f(409,0,416,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'*', Gui::Vec4f(416,0,421,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'(', Gui::Vec4f(421,0,424,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u')', Gui::Vec4f(424,0,427,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'[', Gui::Vec4f(428,0,431,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'{', Gui::Vec4f(434,0,438,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'}', Gui::Vec4f(438,0,442,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'<', Gui::Vec4f(442,0,447,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'>', Gui::Vec4f(447,0,452,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u';', Gui::Vec4f(452,0,455,13), 0, 0, 0, 481, 14 );    font->addCharacter(u':', Gui::Vec4f(455,0,457,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'\'', Gui::Vec4f(457,0,459,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'\"', Gui::Vec4f(459,0,462,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u',', Gui::Vec4f(462,0,465,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'.', Gui::Vec4f(465,0,467,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'\\', Gui::Vec4f(467,0,471,13), 0, 0, 0, 481, 14 );    font->addCharacter(u'|', Gui::Vec4f(471,0,473,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'/', Gui::Vec4f(473,0,477,13), 0, 0, 0, 481, 14 );    font->addCharacter(u' ', Gui::Vec4f(353,0,358,13), 0, 0, 0, 481, 14 );
    font->addCharacter(u'\t', Gui::Vec4f(348,0,366,13), 0, 0, 0, 481, 14 );

    font = m_microFont;
	font->addCharacter(u'A', Gui::Vec4f(0,0,5,8), 0, 0, 0, 436,9 ); 
	font->addCharacter(u'B', Gui::Vec4f(5,0,10,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'C', Gui::Vec4f(10,0,15,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'D', Gui::Vec4f(15,0,20,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'E', Gui::Vec4f(20,0,25,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'F', Gui::Vec4f(25,0,30,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'G', Gui::Vec4f(30,0,35,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'H', Gui::Vec4f(35,0,40,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'I', Gui::Vec4f(40,0,42,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'J', Gui::Vec4f(42,0,46,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'K', Gui::Vec4f(46,0,51,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'L', Gui::Vec4f(51,0,56,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'M', Gui::Vec4f(56,0,61,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'N', Gui::Vec4f(61,0,66,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'O', Gui::Vec4f(66,0,71,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'P', Gui::Vec4f(71,0,76,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'Q', Gui::Vec4f(76,0,81,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'R', Gui::Vec4f(81,0,86,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'S', Gui::Vec4f(86,0,91,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'T', Gui::Vec4f(91,0,96,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'U', Gui::Vec4f(96,0,101,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'V', Gui::Vec4f(101,0,106,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'W', Gui::Vec4f(106,0,111,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'X', Gui::Vec4f(111,0,116,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'Y', Gui::Vec4f(116,0,121,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'Z', Gui::Vec4f(121,0,126,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'a', Gui::Vec4f(126,0,131,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'b', Gui::Vec4f(131,0,136,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'c', Gui::Vec4f(136,0,141,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'd', Gui::Vec4f(141,0,146,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'e', Gui::Vec4f(146,0,151,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'f', Gui::Vec4f(151,0,156,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'g', Gui::Vec4f(156,0,161,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'h', Gui::Vec4f(161,0,166,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'i', Gui::Vec4f(166,0,168,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'j', Gui::Vec4f(168,0,171,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'k', Gui::Vec4f(171,0,176,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'l', Gui::Vec4f(176,0,178,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'm', Gui::Vec4f(178,0,183,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'n', Gui::Vec4f(183,0,188,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'o', Gui::Vec4f(188,0,193,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'p', Gui::Vec4f(193,0,198,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'q', Gui::Vec4f(198,0,203,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'r', Gui::Vec4f(203,0,208,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u's', Gui::Vec4f(208,0,213,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u't', Gui::Vec4f(213,0,218,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'u', Gui::Vec4f(218,0,223,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'v', Gui::Vec4f(223,0,228,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'w', Gui::Vec4f(228,0,233,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'x', Gui::Vec4f(233,0,238,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'y', Gui::Vec4f(238,0,243,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'z', Gui::Vec4f(243,0,248,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'0', Gui::Vec4f(248,0,253,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'1', Gui::Vec4f(253,0,257,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'2', Gui::Vec4f(258,0,263,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'3', Gui::Vec4f(263,0,268,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'4', Gui::Vec4f(268,0,273,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'5', Gui::Vec4f(273,0,278,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'6', Gui::Vec4f(278,0,283,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'7', Gui::Vec4f(283,0,288,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'8', Gui::Vec4f(288,0,293,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'9', Gui::Vec4f(293,0,298,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'-', Gui::Vec4f(298,0,303,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'_', Gui::Vec4f(303,0,308,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'=', Gui::Vec4f(308,0,313,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'+', Gui::Vec4f(313,0,318,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'`', Gui::Vec4f(318,0,322,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'~', Gui::Vec4f(322,0,327,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'!', Gui::Vec4f(327,0,329,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'?', Gui::Vec4f(329,0,334,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'@', Gui::Vec4f(334,0,339,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'#', Gui::Vec4f(339,0,344,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'№', Gui::Vec4f(344,0,352,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'$', Gui::Vec4f(352,0,357,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'%', Gui::Vec4f(357,0,362,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'^', Gui::Vec4f(362,0,367,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'&', Gui::Vec4f(367,0,372,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'*', Gui::Vec4f(372,0,377,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'(', Gui::Vec4f(377,0,380,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u')', Gui::Vec4f(380,0,383,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'{', Gui::Vec4f(383,0,387,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'}', Gui::Vec4f(387,0,391,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'[', Gui::Vec4f(391,0,394,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u']', Gui::Vec4f(394,0,397,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'<', Gui::Vec4f(397,0,402,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'>', Gui::Vec4f(402,0,407,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'\\', Gui::Vec4f(407,0,412,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'|', Gui::Vec4f(412,0,414,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'/', Gui::Vec4f(414,0,419,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'\'', Gui::Vec4f(419,0,421,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'\"', Gui::Vec4f(421,0,425,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u',', Gui::Vec4f(425,0,427,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u'.', Gui::Vec4f(427,0,429,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u';', Gui::Vec4f(429,0,432,8), 0, 0, 0, 436,9 );  
	font->addCharacter(u':', Gui::Vec4f(432,0,435,8), 0, 0, 0, 436,9 );  
	
	font = m_iconsFont;
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::NextMenu), Gui::Vec4f(3,8,22,27), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::NewObject), Gui::Vec4f(3,29,22,48), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::EditObject), Gui::Vec4f(24,29,43,48), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::EditUV), Gui::Vec4f(45,29,64,48), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::CollapseCategory), Gui::Vec4f(30,9,38,13), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::ExpandCategory), Gui::Vec4f(30,15,38,23), 0, 0, 0, 602,640 ); 
	
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::Cross1), Gui::Vec4f(155,601,164,610), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::Add1), Gui::Vec4f(238,621,248,631), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::Sub1), Gui::Vec4f(217,621,227,631), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::Eq1), Gui::Vec4f(301,559,311,569), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::ArrowB1), Gui::Vec4f(112,623,124,631), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::ArrowL1), Gui::Vec4f(134,621,142,633), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::ArrowT1), Gui::Vec4f(154,622,166,630), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::ArrowR1), Gui::Vec4f(93,621,101,633), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::PushCircle), Gui::Vec4f(321,556,335,570), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::AssignMaterial), Gui::Vec4f(381,577,400,593), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::Save), Gui::Vec4f(192,71,211,90), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::ClearOutputImage), Gui::Vec4f(45,470,64,489), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::Undo), Gui::Vec4f(276,50,295,69), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::Redo), Gui::Vec4f(297,50,316,69), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::SelectByName), Gui::Vec4f(297,29,316,48), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::SelectModeSelect), Gui::Vec4f(444,428,463,447), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::ActiveButtonLTB), Gui::Vec4f(444,449,463,468), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::TransformMove), Gui::Vec4f(3,302,22,321), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::TransformScale), Gui::Vec4f(24,302,43,321), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::TransformRotate), Gui::Vec4f(45,302,64,321), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::EditVertex), Gui::Vec4f(171,155,190,174), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::EditEdge), Gui::Vec4f(192,155,211,174), 0, 0, 0, 602,640 ); 
	font->addCharacter(kkrooo::getIconFontChar(IconFontSymbol::EditFace), Gui::Vec4f(213,155,232,174), 0, 0, 0, 602,640 ); 

	return true;
}