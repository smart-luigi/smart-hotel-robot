#ifndef _SMART_HOTEL_ROBOT_RESOURCE_HANDLER_H_
#define _SMART_HOTEL_ROBOT_RESOURCE_HANDLER_H_
#pragma once

#include <cef/base/cef_bind.h>
#include <cef/cef_resource_request_handler.h>
#include "smart_hotel_robot.h"
#include "smart_hotel_robot_context.h"

class SmartHotelRobotResourceHandler
	: public CefResourceRequestHandler
{
public:
	SmartHotelRobotResourceHandler(SmartHotelRobotContext* context);
protected:
	virtual ~SmartHotelRobotResourceHandler();
public:
	virtual cef_return_value_t OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		CefRefPtr<CefCallback> callback) override;

	virtual CefRefPtr<CefResponseFilter> GetResourceResponseFilter(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		CefRefPtr<CefResponse> response) override;

	virtual void OnResourceLoadComplete(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		CefRefPtr<CefResponse> response,
		URLRequestStatus status,
		int64_t received_content_length) override;
private:
	IMPLEMENT_REFCOUNTING(SmartHotelRobotResourceHandler);
	DISALLOW_COPY_AND_ASSIGN(SmartHotelRobotResourceHandler);
private:
	SmartHotelRobotContext*			_context;
	SmartHotelRobot*				_robot;
	CefRefPtr<CefResponseFilter>	_response_filter;
};

#endif // !_SMART_HOTEL_ROBOT_RESOURCE_HANDLER_H_