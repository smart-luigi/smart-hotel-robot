#include "precomp.h"
#include <cef/wrapper/cef_helpers.h>
#include "meituan/smart_hotel_robot_filter_meituan.h"
#include "ctrip/smart_hotel_robot_filter_ctrip.h"
#include "smart_hotel_robot_resource_handler.h"

SmartHotelRobotResourceHandler::SmartHotelRobotResourceHandler(SmartHotelRobotContext* context)
	: _context(context)
	, _robot(context->GetHotelRobot())
	, _response_filter(nullptr)
{
	SmartHotelRobotType robot_type = _robot->GetRobotType();
	switch (robot_type)
	{
	case TYPE_ROBOT_MEITUAN:
		_response_filter = new SmartHotelRobotResponseFilterMeituan(context);
		break;
	case TYPE_ROBOT_CTRIP:
		_response_filter = new SmartHotelRobotResponseFilterCtrip(context);
		break;
	}
}

SmartHotelRobotResourceHandler::~SmartHotelRobotResourceHandler()
{
	
}

cef_return_value_t SmartHotelRobotResourceHandler::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefRequest> request,
	CefRefPtr<CefCallback> callback) 
{
	CefString* url = new CefString(request->GetURL());
	SmartLogInfo("url = %s", url->ToString().c_str());

	return RV_CONTINUE_ASYNC;
}

CefRefPtr<CefResponseFilter> SmartHotelRobotResourceHandler::GetResourceResponseFilter(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefRequest> request,
	CefRefPtr<CefResponse> response) 
{
	CefString* url = new CefString(request->GetURL());
	if (_robot->IsDataUrl(url->ToString().c_str()))
	{
		return _response_filter;
	}

	return nullptr;
}

void SmartHotelRobotResourceHandler::OnResourceLoadComplete(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefRequest> request,
	CefRefPtr<CefResponse> response,
	URLRequestStatus status,
	int64_t received_content_length) 
{
	if (!CefCurrentlyOn(TID_IO))
		return;

	do
	{
		CefString* url = new CefString(request->GetURL());
		if (_robot->IsLoginUrl(url->ToString().c_str()))
		{
			_robot->SetUnauthorized();
			_robot->AuthorizeAccountPassword(browser, frame, *url);
			break;
		}

		if (_robot->IsListUrl(url->ToString().c_str()))
		{
			_robot->SetAuthorized();
			_robot->QueryHotels(browser, frame, *url);
			break;
		}
	} while (false);
}