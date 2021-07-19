#include "include.h"
#include "macros.h"

#ifndef MAIN_CONFIG
	#define MAIN_CONFIG
		enum EVENT_CODE
		{
			NULL_EVENT_CODE,
			CLOSE_EVENT_CODE,
			RESTART_EVENT_CODE,
			SHOW_MOUSE_EVENT_CODE
		};
#endif