#include "precomp.h"
#include "meituan/smart_hotel_robot_meituan.h"

SmartHotelRobotMeituan::SmartHotelRobotMeituan(SmartHotelRobotContext* context)
	: _context(context)
	, _url_login("https://passport.meituan.com/useraccount/ilogin")
	, _url_list("https://i.meituan.com/awp/h5/hotel/list/list.html")
	, _url_data("https://i.meituan.com/awp/h5/hotel/list/list.html")
	, _authorized(false)
	, _authorizing_start_event(nullptr)
	, _authorizing_complete_event(nullptr)
	, _authoriz_sms_start_event(nullptr)
	, _authoriz_sms_complete_event(nullptr)
{

}

SmartHotelRobotMeituan::~SmartHotelRobotMeituan()
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

int SmartHotelRobotMeituan::Init()
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

void SmartHotelRobotMeituan::Dispose()
{
	delete this;
}

SmartHotelRobotType SmartHotelRobotMeituan::GetRobotType()
{
	return TYPE_ROBOT_MEITUAN;
}

const char* SmartHotelRobotMeituan::GetLoginUrl()
{
	return _url_login.c_str();
}

const char* SmartHotelRobotMeituan::GetListUrl()
{
	return _url_list.c_str();
}

const char* SmartHotelRobotMeituan::GetDataUrl()
{
	return _url_data.c_str();
}

bool SmartHotelRobotMeituan::IsLoginUrl(const char* url)
{
	return boost::istarts_with(url, _url_login);
}

bool SmartHotelRobotMeituan::IsListUrl(const char* url)
{
	return boost::istarts_with(url, _url_list);
}

bool SmartHotelRobotMeituan::IsDataUrl(const char* url)
{
	return boost::istarts_with(url, _url_data);
}

void SmartHotelRobotMeituan::SetAuthorized()
{
	_authorized = true;
}

void SmartHotelRobotMeituan::SetUnauthorized()
{
	_authorized = false;
}

bool SmartHotelRobotMeituan::IsAuthorized()
{
	return _authorized;
}

void SmartHotelRobotMeituan::WaitAuthorizingStart()
{
	WaitForSingleObject(_authorizing_start_event, INFINITE);
}

void SmartHotelRobotMeituan::WaitAuthorizingComplete()
{
	WaitForSingleObject(_authorizing_complete_event, INFINITE);
}

void SmartHotelRobotMeituan::WaitAuthorizSmsStart()
{
	WaitForSingleObject(_authoriz_sms_start_event, INFINITE);
}

void SmartHotelRobotMeituan::WaitAuthorizSmsComplete()
{
	WaitForSingleObject(_authoriz_sms_complete_event, INFINITE);
}

void SmartHotelRobotMeituan::StartAuthorizing(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{
	SetEvent(_authorizing_start_event);
}

void SmartHotelRobotMeituan::StartAuthorizeSms(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{
	SetEvent(_authoriz_sms_start_event);
}

void SmartHotelRobotMeituan::DoAuthorizing(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
	std::thread([](CefRefPtr<CefFrame> frame, const CefString& url, HANDLE authorize_complete_event, SmartHotelRobotContext* context) {
		std::string id = context->GetCacheEnviromentId();
		std::string code_authorizing = R"(
				var iloginUserConfirm = document.getElementById("iloginUserConfirm");
				if(iloginUserConfirm) {
					iloginUserConfirm.click();
				}

				var phoneNumInput = document.getElementById("phoneNumInput");
				if(phoneNumInput) {
					phoneNumInput.value = "XXXXXXXXXXX";
					phoneNumInput.focus();
				}

				var codeInput = document.getElementById("codeInput");
				if(codeInput) {
					codeInput.focus();
				}

				var sendCodeBtn = document.getElementById("sendCodeBtn");
				if(sendCodeBtn) {
					sendCodeBtn.click();
					codeInput.focus();
				}
		)";

		boost::replace_all(code_authorizing, "XXXXXXXXXXX", id);

		Sleep(2000);

		frame->ExecuteJavaScript(code_authorizing, url, 0);

		Sleep(1000);

		SetEvent(authorize_complete_event);

		}, frame, url, _authorizing_complete_event, _context).detach();
}

void SmartHotelRobotMeituan::DoAuthorizSms(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
	std::thread([](CefRefPtr<CefFrame> frame, const CefString& url, HANDLE authorize_complete_event, SmartHotelRobotContext* context) {
		std::string id = context->GetCacheEnviromentId();
		std::string code_authorizing = R"(
				var iloginUserConfirm = document.getElementById("iloginUserConfirm");
				if(iloginUserConfirm) {
					iloginUserConfirm.click();
				}

				var phoneNumInput = document.getElementById("phoneNumInput");
				if(phoneNumInput) {
					phoneNumInput.value = "XXXXXXXXXXX";
					phoneNumInput.focus();
				}

				var codeInput = document.getElementById("codeInput");
				if(codeInput) {
					codeInput.focus();
				}

				var sendCodeBtn = document.getElementById("sendCodeBtn");
				if(sendCodeBtn) {
					sendCodeBtn.click();
					codeInput.focus();
				}
		)";

		boost::replace_all(code_authorizing, "XXXXXXXXXXX", id);

		Sleep(2000);

		frame->ExecuteJavaScript(code_authorizing, url, 0);

		Sleep(1000);

		SetEvent(authorize_complete_event);

		}, frame, url, _authorizing_complete_event, _context).detach();
}

void SmartHotelRobotMeituan::QueryStatus(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{
	MessageRobotHeader* message_in = (MessageRobotHeader*)message_buffer;
	MessageRobotStatus* message_out = (MessageRobotStatus*)answer_buffer;
	RtlCopyMemory(message_out, message_in, message_length);
	message_out->authorized = _authorized;
}

void SmartHotelRobotMeituan::QueryHotels(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{

}

void SmartHotelRobotMeituan::StartScrollList(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
	cef_mouse_event_t event;
	event.modifiers = EVENTFLAG_MIDDLE_MOUSE_BUTTON;
	event.x = 100;
	event.y = 100;
	browser->GetHost()->SendMouseWheelEvent(event, 200, 200);

	//_context->SendServerMessage(MESSAGE_ROBOT_HOTEL_LIST_READY, nullptr, 0, nullptr, nullptr);

	/*
	HWND hwnd = FindWindowA("SmartCefWindow", "SmartCefApp");
	if (hwnd)
	{
		std::thread([](HWND hwnd) {
			Sleep(3000);
			PostMessage(hwnd, WM_SIMULATE_START, 0, 0);
		}, hwnd).detach();
	}
	*/
}