#ifndef __EVENT_CONSUMER_H__
#define __EVENT_CONSUMER_H__

#include "Classes/Math/kkVector4.h"

class Application;

class EventConsumer : public kkEventConsumer
{
	v2f m_cursor_coords_old;



	bool m_isLastKeyDownOnce = false;
	kkKey m_lastKeyDownOnce;

public:
	EventConsumer();
	~EventConsumer();

	void _reset();
	void processEvent( const kkEvent& ev );
	
	bool isKeyDown( kkKey k );
	bool isKeyDownOnce( kkKey k );
	bool isKeyUp( kkKey k, bool reset );

	// если предыдущий isKeyDownOnce( kkKey k ) правда, но его не нужно было использовать (он нужен в другом месте), то
	// этот метод должен вернуть состояние.
	//     условие не выполнено, но isKeyDownOnce сбросило значение...это плохо
	// if( условие и isKeyDownOnce( kkKey ) ){  }
	void restoreLastKeyDownOnce();
	
	bool isLmbDown();
	bool isLmbDownOnce();
	bool isLmbUp();

	bool isMmbDown();
	bool isMmbDownOnce();
	bool isMmbUp();

	bool isRmbDown();
	bool isRmbDownOnce();
	bool isRmbUp();

	Application * m_app = nullptr;
	bool m_input_update = false;

	/// Нажали - символ печатается непрерывно
	s8 m_keys_down[ 256 ];
	bool m_lmb_down = false;
	bool m_lmb_once = false;
	bool m_lmb_once_state = false;
	bool m_lmb_up = false;

	bool m_rmb_down = false;
	bool m_rmb_once = false;
	bool m_rmb_once_state = false;
	bool m_rmb_up = false;

	bool m_mmb_down = false;
	bool m_mmb_once = false;
	bool m_mmb_once_state = false;
	bool m_mmb_up = false;

	/// Нажали - символ печатается 1 раз
	s8 m_keys_down_once[ 256 ];
	s8 m_keys_down_once_state[ 256 ]; /// это как бы блокиратор

	/// печатается 1 символ на отжатии
	s8 m_keys_up[ 256 ];
};

#endif