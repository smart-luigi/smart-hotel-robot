#ifndef _SMART_HOTEL_ROBOT_WINDOW_HANDLER_H_
#define _SMART_HOTEL_ROBOT_WINDOW_HANDLER_H_
#pragma once

#include <smart_cef_client.h>
#include "smart_hotel_robot_context.h"

class SmartHotelRobotWindowHandler
	: public CefWindowMessageHandler
{
public:
	SmartHotelRobotWindowHandler(SmartHotelRobotContext* context);
	virtual ~SmartHotelRobotWindowHandler();
protected:
	virtual LRESULT HandleWindowMessage(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam) override;
protected:
	int HandleMessageSimulateStart(HWND hwnd);
protected:
	SmartHotelRobotContext* _context;
};

#endif // !_SMART_HOTEL_ROBOT_WINDOW_HANDLER_H_