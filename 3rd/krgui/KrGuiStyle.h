#ifndef __KK_KRGUI_STYLE_H__
#define __KK_KRGUI_STYLE_H__

namespace Kr
{
	namespace Gui
	{
		struct Style
		{
			float commonTextSpacing = 2; // for just text
			float commonTextSpaceAddSize = 2; 
			Vec4f commonTextColor = ColorWhite; 
			float commonTextAlpha = 1.f; 
			float commonTextYOffset = 0.f; // move text up\down

			// for buttons. gradient - up/down (only for rectangular buttons)
			Vec4f buttonIdleColor1 = ColorLightGray; // not active button
			Vec4f buttonIdleColor2 = ColorGray;
			Vec4f buttonHoverColor1 = ColorDarkGray; // when mouse cursor over button
			Vec4f buttonHoverColor2 = ColorDarkGray;
			Vec4f buttonPushColor1 = ColorGray;      // when button pressed
			Vec4f buttonPushColor2 = ColorLightGray;
			Vec4f buttonDisabledColor1 = ColorLightGray;
			Vec4f buttonDisabledColor2 = ColorLightGray;
			float buttonTextSpacing = 2; // for text on buttons
			float buttonTextSpaceAddSize = 2; // for text on buttons
			Vec4f buttonTextIdleColor     = ColorBlack; // for text on buttons
			Vec4f buttonTextHoverColor    = 0xFFB75403; //   when mouse cursor over button
			Vec4f buttonTextPushColor     = 0xFF7A3602; //   when button pressed
			Vec4f buttonTextDisabledColor = ColorDarkGray; //   when button disabled
			Vec2f buttonTextPositionAdd = Vec2f(0,0);
			float buttonBackgroundAlpha = 1.f; // for button BG
			float buttonTextAlpha = 1.f; // for button text

			// BG color for addRectangle
			Vec4f rectangleIdleColor1 = ColorGray;
			Vec4f rectangleIdleColor2 = ColorGray;
			Vec4f rectangleHoverColor1 = ColorLightGray;
			Vec4f rectangleHoverColor2 = ColorLightGray;

			Vec4f nodeEditorColor1 = ColorGray;
			Vec4f nodeEditorColor2 = ColorGray;
			Vec4f nodeEditorNodeColor1 = ColorDarkGray;
			Vec4f nodeEditorNodeColor2 = ColorDarkGray;
			float nodeEditorNodeColorAlpha = 0.8f;

			Vec4f nodeEditorSocketIdleColor = ColorLightCyan;

			// BG color for beginGroup
			Vec4f groupColor1   = 0xFFA0A0A0;
			Vec4f groupColor2   = 0xFFA0A0A0;
			Vec4f groupHoverColor1 = ColorDarkGray;
			Vec4f groupHoverColor2 = ColorDarkGray;
			float groupBackgroundAlpha = 0.8f;

			Vec4f popupColor1   = 0xFF515151;
			Vec4f popupColor2   = 0xFF515151;
			float popupAlpha    = 0.82f;
			Vec4f popupBorderIndent = Vec4f(2.f,2.f,2.f,2.f);
			//float popupItemIndent = 3.f;


			float menuBarHeight = 18.f;
			Vec4f menuBarColor1 = 0xff555555;
			Vec4f menuBarColor2 = 0xff666666;
			float menuBarBGAlpha  = 0.9f;
			float menuBarTextSpacing = 2;
			float menuBarTextSpaceAddSize = 2;
			float menuBarTextBorderOffset = 12.f;
			float menuBarTextYOffset = 2.f;
			Vec4f menuBarHoverBGColor1 = 0xff007ACC;
			Vec4f menuBarHoverBGColor2 = 0xff0070C0;
			float menuBarHoverBGAlpha  = 0.8f;
			Vec4f menuBarTextHoverColor = 0xFFDDDDDD;
			Vec4f menuBarTextColor = ColorWhite;
			Vec4f menuBarDisabledTextColor = ColorGrey;

			float menuItemHeight = 18.f;
			Vec4f menuItemBGColor1 = 0xff555555;
			Vec4f menuItemBGColor2 = 0xff555555;
			Vec4f menuItemHoverBGColor1 = 0xff007ACC;
			Vec4f menuItemHoverBGColor2 = 0xff0070C0;
			float menuItemHoverBGAlpha  = 0.8f;
			float menuItemTextSpacing = 2;
			float menuItemTextSpaceAddSize = 2;
			float menuItemTextYOffset = 2.f;
			Vec4f menuItemTextHoverColor = 0xFF111111;
			Vec4f menuItemTextColor = ColorWhite;
			Vec2f menuItemIconSize = Vec2f(menuItemHeight, menuItemHeight);
			Vec2f menuItemIconSubmenuSize = Vec2f(menuItemHeight, menuItemHeight);
			float menuItemIconTextOffset = 2.f;
			float menuItemItemTextShortcutOffset = 40.f;

			float separatorHeight = 5.f;
			float separatorLineWidth = 1.f;
			Vec4f separatorLineColor = 0xFF777777;
			float separatorLineBorderOffset = 3.f;

			Vec4f rangeSliderBgColor = 0xff797979;
			Vec4f rangeSliderFgColor = ColorLightGray;

			Vec4f textInputBgColor = 0xff000000;
			Vec4f textInputTextColor = 0xffFFFFFF;
			Vec4f textInputSelectedTextColor = 0xff000000;
			Vec4f textInputSelectedBgColor = 0xff0000AA;

			float    checkboxBoxTextOffset = 5.f;
			char16_t checkboxCheckSymbol   = u'#';
			char16_t checkboxUncheckSymbol = u'_';

			// for using this, you need to create special font, where 'symbol' must have own icon
			// see example in main.cpp
			/*char16_t menuItemIcon = 0;
			char16_t menuItemSubmenuIcon = 0;*/
			Font * iconFont = nullptr;
		};
	}
}

#endif