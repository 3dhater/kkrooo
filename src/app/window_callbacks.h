#ifndef __KK_WINDOW_CALLBACKS_H__
#define __KK_WINDOW_CALLBACKS_H__

void window_onMove(kkWindow* window);
void window_onSize(kkWindow* window);
void window_onPaint(kkWindow* window);
void window_onClose(kkWindow* window);

enum E_WINDOW_ID
{
	EWID_MAIN_WINDOW,
	EWID_MATERIALEDITOR_WINDOW,
	EWID_RENDER_WINDOW,
	EWID_IMPORTEXPORT_WINDOW,
};

#endif