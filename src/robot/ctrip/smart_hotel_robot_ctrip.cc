#include "precomp.h"
#include "ctrip/smart_hotel_robot_ctrip.h"

SmartHotelRobotCtrip::SmartHotelRobotCtrip(SmartHotelRobotContext* context)
	: _context(context)
	, _url_login("https://passport.meituan.com")
	, _url_list("https://i.meituan.com/awp/h5/hotel/list/list.html")
	, _url_data("https://i.meituan.com/awp/h5/hotel/list/list.html")
{

}

SmartHotelRobotCtrip::~SmartHotelRobotCtrip()
{

}

int SmartHotelRobotCtrip::Init()
{
	int result = ERROR_SUCCESS;

	return result;
}

void SmartHotelRobotCtrip::Dispose()
{
	delete this;
}

SmartHotelRobotType SmartHotelRobotCtrip::GetRobotType()
{
	return TYPE_ROBOT_CTRIP;
}

const char* SmartHotelRobotCtrip::GetLoginUrl()
{
	return _url_login.c_str();
}

const char* SmartHotelRobotCtrip::GetListUrl()
{
	return _url_list.c_str();
}

const char* SmartHotelRobotCtrip::GetDataUrl()
{
	return _url_data.c_str();
}

bool SmartHotelRobotCtrip::IsLoginUrl(const char* url)
{
	return boost::istarts_with(url, _url_login);
}

bool SmartHotelRobotCtrip::IsListUrl(const char* url)
{
	return boost::istarts_with(url, _url_list);
}

bool SmartHotelRobotCtrip::IsDataUrl(const char* url)
{
	return boost::istarts_with(url, _url_data);
}

void SmartHotelRobotCtrip::StartAutoLogin(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{

}

void SmartHotelRobotCtrip::StartScrollList(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{

}