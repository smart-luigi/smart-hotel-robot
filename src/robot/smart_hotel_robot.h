#ifndef _SMART_HOTEL_ROBOT_H_
#define _SMART_HOTEL_ROBOT_H_
#pragma once

#include <cef/internal/cef_ptr.h>
#include <cef/cef_browser.h>
#include <cef/cef_frame.h>
#include "message.h"

class SmartHotelRobot
{
public:
	virtual int Init() = 0;
	virtual void Dispose() = 0;
public:
	virtual SmartHotelRobotType GetRobotType() = 0;
	virtual const char* GetLoginUrl() = 0;
	virtual const char* GetListUrl() = 0;
	virtual const char* GetDataUrl() = 0;
	virtual bool IsLoginUrl(const char* url) = 0;
	virtual bool IsListUrl(const char* url) = 0;
	virtual bool IsDataUrl(const char* url) = 0;
public:
	virtual void StartAutoLogin(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) = 0;
	virtual void StartScrollList(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) = 0;
};

#endif // !_SMART_HOTEL_ROBOT_H_