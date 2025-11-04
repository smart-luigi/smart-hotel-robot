#ifndef _SMART_HOTEL_ROBOT_CTRIP_H_
#define _SMART_HOTEL_ROBOT_CTRIP_H_
#pragma once

#include <string>
#include <atomic>
#include "smart_hotel_robot.h"
#include "smart_hotel_robot_context.h"

class SmartHotelRobotCtrip
	: public SmartHotelRobot
{
public:
	SmartHotelRobotCtrip(SmartHotelRobotContext* context);
protected:
	virtual ~SmartHotelRobotCtrip();
protected:
	virtual int Init() override;
	virtual void Dispose() override;
protected:
	virtual SmartHotelRobotType GetRobotType() override;
	virtual const char* GetLoginUrl() override;
	virtual const char* GetAccountUrl() override;
	virtual const char* GetListUrl() override;
	virtual const char* GetDataUrl() override;
	virtual bool IsLoginUrl(const char* url) override;
	virtual bool IsListUrl(const char* url) override;
	virtual bool IsDataUrl(const char* url) override;
	virtual const char* GetData() override;
	virtual void SetData(const char* data) override;
protected:
	virtual void SetAuthorized() override;
	virtual void SetUnauthorized() override;
	virtual bool IsAuthorized() override;
	virtual void AuthorizeAccount(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) override;
	virtual void AuthorizeCode(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) override;
	virtual void QueryHotels(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) override;
	virtual void AddHotel(MessageRobotHotel* hotel) override;
protected:
	virtual void HandleAuthorizeAccountStart(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length) override;
	virtual void HandleAuthorizeAccount(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length) override;
	virtual void HandleAuthorizeCodeStart(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length) override;
	virtual void HandleAuthorizeCode(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length) override;
	virtual void HandleQueryAccount(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length) override;
	virtual void HandleQueryStatus(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length) override;
	virtual void HandleQueryHotels(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length) override;
protected:
	virtual void WaitAuthorizingStart();
	virtual void WaitAuthorizingComplete();
	virtual void WaitAuthorizSmsStart();
	virtual void WaitAuthorizSmsComplete();
	virtual void DoAuthorizing(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url);
	virtual void DoAuthorizSms(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url);
protected:
	SmartHotelRobotContext* _context;
	std::string				_url_login;
	std::string				_url_account;
	std::string				_url_list;
	std::string				_url_data;
	std::atomic<bool>		_authorized;
	HANDLE					_authorizing_start_event;
	HANDLE					_authorizing_complete_event;
	HANDLE					_authorize_sms_start_event;
	HANDLE					_authorize_sms_complete_event;
	std::string				_authorize_sms;
	std::string				_hotels_data;
};

#endif // !_SMART_HOTEL_ROBOT_CTRIP_H_