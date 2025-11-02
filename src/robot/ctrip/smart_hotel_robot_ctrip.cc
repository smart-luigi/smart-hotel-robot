#include "precomp.h"
#include "ctrip/smart_hotel_robot_ctrip.h"

SmartHotelRobotCtrip::SmartHotelRobotCtrip(SmartHotelRobotContext* context)
	: _context(context)
	, _url_login("https://passport.meituan.com/useraccount/ilogin")
	, _url_account("https://passport.meituan.com/useraccount/ilogin")
	, _url_list("https://i.meituan.com/awp/h5/hotel/list/list.html")
	, _url_data("https://i.meituan.com/awp/h5/hotel/list/list.html")
	, _authorized(false)
	, _authorizing_start_event(nullptr)
	, _authorizing_complete_event(nullptr)
	, _authorize_sms_start_event(nullptr)
	, _authorize_sms_complete_event(nullptr)
{

}

SmartHotelRobotCtrip::~SmartHotelRobotCtrip()
{
	if (_authorize_sms_complete_event)
	{
		CloseHandle(_authorize_sms_complete_event);
		_authorize_sms_complete_event = nullptr;
	}

	if (_authorize_sms_start_event)
	{
		CloseHandle(_authorize_sms_start_event);
		_authorize_sms_start_event = nullptr;
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

		_authorize_sms_start_event = CreateEvent(nullptr, true, false, nullptr);
		if (_authorize_sms_start_event == nullptr)
		{
			result = GetLastError();
			break;
		}

		_authorize_sms_complete_event = CreateEvent(nullptr, true, false, nullptr);
		if (_authorize_sms_complete_event == nullptr)
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
	return TYPE_ROBOT_MEITUAN;
}

const char* SmartHotelRobotCtrip::GetLoginUrl()
{
	return _url_login.c_str();
}

const char* SmartHotelRobotCtrip::GetAccountUrl()
{
	return _url_account.c_str();
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

const char* SmartHotelRobotCtrip::GetData()
{
	return _hotels_data.c_str();
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

void SmartHotelRobotCtrip::AuthorizeAccount(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{

}

void SmartHotelRobotCtrip::AuthorizeCode(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
	
}

void SmartHotelRobotCtrip::QueryHotels(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{

}

void SmartHotelRobotCtrip::AddHotels(void* data, size_t data_size)
{

}

void SmartHotelRobotCtrip::HandleAuthorizeAccount(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{

}

void SmartHotelRobotCtrip::HandleAuthorizeCodeStart(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{

}

void SmartHotelRobotCtrip::HandleAuthorizeCode(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{
	
}

void SmartHotelRobotCtrip::HandleQueryAccount(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{

}

void SmartHotelRobotCtrip::HandleQueryStatus(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{
	MessageRobotHeader* message_in = (MessageRobotHeader*)message_buffer;
	MessageRobotStatus* message_out = (MessageRobotStatus*)answer_buffer;
	RtlCopyMemory(message_out, message_in, message_length);
	message_out->authorized = _authorized;
}

void SmartHotelRobotCtrip::HandleQueryHotels(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{

}


void SmartHotelRobotCtrip::WaitAuthorizingStart()
{
	WaitForSingleObject(_authorizing_start_event, INFINITE);
	ResetEvent(_authorizing_start_event);
}

void SmartHotelRobotCtrip::WaitAuthorizingComplete()
{
	WaitForSingleObject(_authorizing_complete_event, INFINITE);
	ResetEvent(_authorizing_complete_event);
}

void SmartHotelRobotCtrip::WaitAuthorizSmsStart()
{
	WaitForSingleObject(_authorize_sms_start_event, INFINITE);
	ResetEvent(_authorize_sms_start_event);
}

void SmartHotelRobotCtrip::WaitAuthorizSmsComplete()
{
	WaitForSingleObject(_authorize_sms_complete_event, INFINITE);
	ResetEvent(_authorize_sms_complete_event);
}

void SmartHotelRobotCtrip::HandleAuthorizeAccountStart(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{
	SetEvent(_authorizing_start_event);
}

void SmartHotelRobotCtrip::DoAuthorizing(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
	
}

void SmartHotelRobotCtrip::DoAuthorizSms(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
	
}