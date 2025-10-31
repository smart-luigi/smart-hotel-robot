#ifndef _SMART_HOTEL_ROBOT_MEITUAN_H_
#define _SMART_HOTEL_ROBOT_MEITUAN_H_
#pragma once

#include <string>
#include <atomic>
#include "smart_hotel_robot.h"
#include "smart_hotel_robot_context.h"

class SmartHotelRobotMeituan
	: public SmartHotelRobot
{
public:
	SmartHotelRobotMeituan(SmartHotelRobotContext* context);
protected:
	virtual ~SmartHotelRobotMeituan();
protected:
	virtual int Init() override;
	virtual void Dispose() override;
protected:
	virtual SmartHotelRobotType GetRobotType() override;
	virtual const char* GetLoginUrl() override;
	virtual const char* GetListUrl() override;
	virtual const char* GetDataUrl() override;
	virtual bool IsLoginUrl(const char* url) override;
	virtual bool IsListUrl(const char* url) override;
	virtual bool IsDataUrl(const char* url) override;
protected:
	virtual void SetAuthorized() override;
	virtual void SetUnauthorized() override;
	virtual bool IsAuthorized() override;
	virtual void WaitAuthorizingStart() override;
	virtual void WaitAuthorizingComplete() override;
	virtual void WaitAuthorizSmsStart() override;
	virtual void WaitAuthorizSmsComplete() override;
	virtual void StartAuthorizing(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length) override;
	virtual void StartAuthorizeSms(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length) override;
	virtual void DoAuthorizing(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) override;
	virtual void DoAuthorizSms(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) override;
protected:
	virtual void QueryStatus(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length) override;
	virtual void QueryHotels(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length) override;
	virtual void StartScrollList(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) override;
protected:
	SmartHotelRobotContext* _context;
	std::string				_url_login;
	std::string				_url_list;
	std::string				_url_data;
	std::atomic<bool>		_authorized;
	HANDLE					_authorizing_start_event;
	HANDLE					_authorizing_complete_event;
	HANDLE					_authoriz_sms_start_event;
	HANDLE					_authoriz_sms_complete_event;
};

#endif // !_SMART_HOTEL_ROBOT_MEITUAN_H_