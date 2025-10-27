#ifndef _SMART_HOTEL_ROBOT_CTRIP_H_
#define _SMART_HOTEL_ROBOT_CTRIP_H_
#pragma once

#include <string>
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
	virtual const char* GetListUrl() override;
	virtual const char* GetDataUrl() override;
	virtual bool IsLoginUrl(const char* url) override;
	virtual bool IsListUrl(const char* url) override;
	virtual bool IsDataUrl(const char* url) override;
protected:
	virtual void StartAutoLogin(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) override;
	virtual void StartScrollList(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) override;
protected:
	SmartHotelRobotContext* _context;
	std::string				_url_login;
	std::string				_url_list;
	std::string				_url_data;
};

#endif // !_SMART_HOTEL_ROBOT_CTRIP_H_