#ifndef _SMART_HOTEL_ROBOT_H_
#define _SMART_HOTEL_ROBOT_H_
#pragma once

#include <cef/internal/cef_ptr.h>
#include <cef/cef_browser.h>
#include <cef/cef_frame.h>
#include "smart_hotel_message.h"

class SmartHotelRobot
{
public:
	virtual int Init() = 0;
	virtual void Dispose() = 0;
public:
	virtual SmartHotelRobotType GetRobotType() = 0;
	virtual const char* GetLoginUrl() = 0;
	virtual const char* GetAccountUrl() = 0;
	virtual const char* GetListUrl() = 0;
	virtual const char* GetDataUrl() = 0;
	virtual bool IsLoginUrl(const char* url) = 0;
	virtual bool IsListUrl(const char* url) = 0;
	virtual bool IsDataUrl(const char* url) = 0;
	virtual const char* GetData() = 0;
public:
	virtual void SetAuthorized() = 0;
	virtual void SetUnauthorized() = 0;
	virtual bool IsAuthorized() = 0;
	virtual void AuthorizeAccount(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) = 0;
	virtual void AuthorizeCode(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) = 0;
	virtual void QueryHotels(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) = 0;
	virtual void AddHotels(void* data, size_t data_size) = 0;
public:
	virtual void HandleAuthorizeAccountStart(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length) = 0;
	virtual void HandleAuthorizeAccount(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length) = 0;
	virtual void HandleAuthorizeCodeStart(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length) = 0;
	virtual void HandleAuthorizeCode(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length) = 0;
	virtual void HandleQueryAccount(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length) = 0;
	virtual void HandleQueryStatus(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length) = 0;
	virtual void HandleQueryHotels(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length) = 0;
};

#endif // !_SMART_HOTEL_ROBOT_H_