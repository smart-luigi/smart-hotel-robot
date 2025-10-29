#include "precomp.h"
#include "meituan/smart_hotel_robot_meituan.h"

SmartHotelRobotMeituan::SmartHotelRobotMeituan(SmartHotelRobotContext* context)
	: _context(context)
	, _url_login("https://passport.meituan.com/useraccount/ilogin")
	, _url_list("https://i.meituan.com/awp/h5/hotel/list/list.html")
	, _url_data("https://i.meituan.com/awp/h5/hotel/list/list.html")
	, _authorized(false)
{

}

SmartHotelRobotMeituan::~SmartHotelRobotMeituan()
{

}

int SmartHotelRobotMeituan::Init()
{
	int result = ERROR_SUCCESS;

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

void SmartHotelRobotMeituan::SetAuthorized()
{
	_authorized = true;
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

bool SmartHotelRobotMeituan::IsAuthorized()
{
	return _authorized;
}

void SmartHotelRobotMeituan::StartAuthorizing(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{

}

void SmartHotelRobotMeituan::StartAuthorizeSms(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{

}

void SmartHotelRobotMeituan::QueryStatus(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{

}

void SmartHotelRobotMeituan::QueryHotels(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{

}

void SmartHotelRobotMeituan::StartAutoLogin(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
	std::thread([](CefRefPtr<CefFrame> frame, const CefString& url, SmartHotelRobotContext* context) {
		std::string id = context->GetCacheEnviromentId();
		std::string code = R"(
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

		boost::replace_all(code, "XXXXXXXXXXX", id);

		char sms[6] = { 0 };
		context->SendServerMessageRobotAuthorizeCompleted(sms, sizeof(sms));

		//Sleep(2000);

		//frame->ExecuteJavaScript(code, url, 0);

	}, frame, url, _context).detach();
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