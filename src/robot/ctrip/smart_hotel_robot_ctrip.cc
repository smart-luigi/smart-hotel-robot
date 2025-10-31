#include "precomp.h"
#include "ctrip/smart_hotel_robot_ctrip.h"

SmartHotelRobotCtrip::SmartHotelRobotCtrip(SmartHotelRobotContext* context)
	: _context(context)
	, _url_login("https://passport.meituan.com")
	, _url_list("https://i.meituan.com/awp/h5/hotel/list/list.html")
	, _url_data("https://i.meituan.com/awp/h5/hotel/list/list.html")
	, _authorized(false)
	, _authorizing_start_event(nullptr)
	, _authorizing_complete_event(nullptr)
	, _authoriz_sms_start_event(nullptr)
	, _authoriz_sms_complete_event(nullptr)
{

}

SmartHotelRobotCtrip::~SmartHotelRobotCtrip()
{
	if (_authoriz_sms_complete_event)
	{
		CloseHandle(_authoriz_sms_complete_event);
		_authoriz_sms_complete_event = nullptr;
	}

	if (_authoriz_sms_start_event)
	{
		CloseHandle(_authoriz_sms_start_event);
		_authoriz_sms_start_event = nullptr;
	}

	if (_authorizing_complete_event)
	{
		CloseHandle(_authorizing_complete_event);
		_authorizing_complete_event = nullptr;
	}

	if (_authorizing_start_event)
	{
		CloseHandle(_authorizing_start_event);
		_authorizing_start_event = nullptr;
	}
}

int SmartHotelRobotCtrip::Init()
{
	int result = ERROR_SUCCESS;

	do
	{
		_authorizing_start_event = CreateEvent(nullptr, true, false, nullptr);
		if (_authorizing_start_event == nullptr)
		{
			result = GetLastError();
			break;
		}

		_authorizing_complete_event = CreateEvent(nullptr, true, false, nullptr);
		if (_authorizing_complete_event == nullptr)
		{
			result = GetLastError();
			break;
		}

		_authoriz_sms_start_event = CreateEvent(nullptr, true, false, nullptr);
		if (_authoriz_sms_start_event == nullptr)
		{
			result = GetLastError();
			break;
		}

		_authoriz_sms_complete_event = CreateEvent(nullptr, true, false, nullptr);
		if (_authoriz_sms_complete_event == nullptr)
		{
			result = GetLastError();
			break;
		}
	} while (false);

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

void SmartHotelRobotCtrip::SetAuthorized()
{
	_authorized = true;
}

void SmartHotelRobotCtrip::SetUnauthorized()
{
	_authorized = false;
}

bool SmartHotelRobotCtrip::IsAuthorized()
{
	return _authorized;
}

void SmartHotelRobotCtrip::WaitAuthorizingStart()
{
	WaitForSingleObject(_authorizing_start_event, INFINITE);
}

void SmartHotelRobotCtrip::WaitAuthorizingComplete()
{
	WaitForSingleObject(_authorizing_complete_event, INFINITE);
}

void SmartHotelRobotCtrip::WaitAuthorizSmsStart()
{
	WaitForSingleObject(_authoriz_sms_start_event, INFINITE);
}

void SmartHotelRobotCtrip::WaitAuthorizSmsComplete()
{
	WaitForSingleObject(_authoriz_sms_complete_event, INFINITE);
}


void SmartHotelRobotCtrip::StartAuthorizing(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{
	SetEvent(_authorizing_start_event);
}

void SmartHotelRobotCtrip::StartAuthorizeSms(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{

}

void SmartHotelRobotCtrip::DoAuthorizing(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{

}

void SmartHotelRobotCtrip::DoAuthorizSms(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{

}

void SmartHotelRobotCtrip::QueryStatus(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{

}

void SmartHotelRobotCtrip::QueryHotels(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{

}

void SmartHotelRobotCtrip::StartScrollList(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{

}