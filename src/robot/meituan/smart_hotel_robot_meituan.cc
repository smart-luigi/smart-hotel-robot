#include "precomp.h"
#include "meituan/smart_hotel_robot_meituan.h"

SmartHotelRobotMeituan::SmartHotelRobotMeituan(SmartHotelRobotContext* context)
	: _context(context)
	, _url_login("https://passport.meituan.com/useraccount/ilogin")
	, _url_account("https://i.meituan.com/mttouch/page/account")
	, _url_list("https://i.meituan.com/awp/h5/hotel/list/list.html")
	, _url_data("https://ihotel.meituan.com/hbsearch/HotelSearch")
	, _authorized(false)
	, _authorizing_code_start_event(nullptr)
	, _authorizing_code_complete_event(nullptr)
	, _authorize_code_start_event(nullptr)
	, _authorize_code_complete_event(nullptr)
{

}

SmartHotelRobotMeituan::~SmartHotelRobotMeituan()
{
	if (_authorize_code_complete_event)
	{
		CloseHandle(_authorize_code_complete_event);
		_authorize_code_complete_event = nullptr;
	}

	if (_authorize_code_start_event)
	{
		CloseHandle(_authorize_code_start_event);
		_authorize_code_start_event = nullptr;
	}

	if (_authorizing_code_complete_event)
	{
		CloseHandle(_authorizing_code_complete_event);
		_authorizing_code_complete_event = nullptr;
	}

	if (_authorizing_code_start_event)
	{
		CloseHandle(_authorizing_code_start_event);
		_authorizing_code_start_event = nullptr;
	}
}

int SmartHotelRobotMeituan::Init()
{
	int result = ERROR_SUCCESS;

	do
	{
		_authorizing_code_start_event = CreateEvent(nullptr, true, false, nullptr);
		if (_authorizing_code_start_event == nullptr)
		{
			result = GetLastError();
			break;
		}

		_authorizing_code_complete_event = CreateEvent(nullptr, true, false, nullptr);
		if (_authorizing_code_complete_event == nullptr)
		{
			result = GetLastError();
			break;
		}

		_authorize_code_start_event = CreateEvent(nullptr, true, false, nullptr);
		if (_authorize_code_start_event == nullptr)
		{
			result = GetLastError();
			break;
		}

		_authorize_code_complete_event = CreateEvent(nullptr, true, false, nullptr);
		if (_authorize_code_complete_event == nullptr)
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

const char* SmartHotelRobotMeituan::GetAccountUrl()
{
	return _url_account.c_str();
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

const char* SmartHotelRobotMeituan::GetData()
{
	return _hotels_data.c_str();
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

void SmartHotelRobotMeituan::AuthorizeAccount(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
	std::thread([](CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url, SmartHotelRobotMeituan* robot) {


	}, browser, frame, url, this).detach();
}

void SmartHotelRobotMeituan::AuthorizeCode(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
	std::thread([](CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url, SmartHotelRobotMeituan* robot) {

		robot->WaitAuthorizingCodeStart();

		robot->DoAuthorizingCode(browser, frame, url);

		robot->WaitAuthorizingCodeComplete();

		robot->WaitAuthorizCodeStart();

		robot->DoAuthorizCode(browser, frame, url);

		robot->WaitAuthorizCodeComplete();

	}, browser, frame, url, this).detach();
}

void SmartHotelRobotMeituan::QueryHotels(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
	std::thread([](CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) {
		Sleep(3000);
		std::string code_scroll = R"(		
			var text = document.body.innerHTML;
			var searchText = "没有更多了";
			var timeId = setInterval(function(){
				debugger;
				if(text.indexOf(searchText) === -1) {
					window.scrollBy(0, document.body.scrollHeight);
				} else {
					clearInterval(timeId);
				}
			}, 2000);
		)";

		char* utf8_code_scroll = (char*)SmartMemAlloc(code_scroll.length() * 4);
		if (utf8_code_scroll == nullptr)
			return;

		if(!SmartStrA2U(utf8_code_scroll, code_scroll.c_str()))
		{
			SmartMemFree(utf8_code_scroll);
			utf8_code_scroll = nullptr;
			return;
		}

		frame->ExecuteJavaScript(utf8_code_scroll, url, 0);

		if(utf8_code_scroll)
		{
			SmartMemFree(utf8_code_scroll);
			utf8_code_scroll = nullptr;
		}

	}, browser, frame, url).detach();
}

void SmartHotelRobotMeituan::AddHotels(void* data, size_t data_size)
{
	_hotels_data.append((char*)data);
}

void SmartHotelRobotMeituan::HandleAuthorizeAccount(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{

}

void SmartHotelRobotMeituan::HandleAuthorizeCodeStart(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{
	SetEvent(_authorizing_code_start_event);
}

void SmartHotelRobotMeituan::HandleAuthorizeCode(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{
	MessageRobotAuthorizeCodePtr message = (MessageRobotAuthorizeCodePtr)message_buffer;
	if (message == nullptr)
		return;

	_authorize_code.clear();
	_authorize_code.append(message->code);

	SetEvent(_authorize_code_start_event);
}

void SmartHotelRobotMeituan::HandleQueryAccount(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{
	_context->PostWindowMessage(_context->GetApplication()->GetRootWindowHandle(), WM_LOAD_URL, (WPARAM)GetAccountUrl(), 0);
}

void SmartHotelRobotMeituan::HandleQueryStatus(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{
	MessageRobotHeader* message_in = (MessageRobotHeader*)message_buffer;
	MessageRobotStatus* message_out = (MessageRobotStatus*)answer_buffer;
	RtlCopyMemory(message_out, message_in, message_length);
	message_out->authorized = _authorized;
}

void SmartHotelRobotMeituan::HandleQueryHotels(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{
	std::memset(answer_buffer, 0, answer_length);

	size_t data_length = _hotels_data.length();
	if (data_length <= answer_length)
	{
		std::memcpy(answer_buffer, _hotels_data.c_str(), _hotels_data.size());
	}
	else
	{
		std::memcpy(answer_buffer, _hotels_data.c_str(), answer_length);
	}
}

void SmartHotelRobotMeituan::WaitAuthorizingCodeStart()
{
	WaitForSingleObject(_authorizing_code_start_event, INFINITE);
	ResetEvent(_authorizing_code_start_event);
}

void SmartHotelRobotMeituan::WaitAuthorizingCodeComplete()
{
	WaitForSingleObject(_authorizing_code_complete_event, INFINITE);
	ResetEvent(_authorizing_code_complete_event);
}

void SmartHotelRobotMeituan::WaitAuthorizCodeStart()
{
	WaitForSingleObject(_authorize_code_start_event, INFINITE);
	ResetEvent(_authorize_code_start_event);
}

void SmartHotelRobotMeituan::WaitAuthorizCodeComplete()
{
	WaitForSingleObject(_authorize_code_complete_event, INFINITE);
	ResetEvent(_authorize_code_complete_event);
}

void SmartHotelRobotMeituan::HandleAuthorizeAccountStart(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{
	SetEvent(_authorizing_code_start_event);
}

void SmartHotelRobotMeituan::DoAuthorizingCode(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
	std::string id = _context->GetCacheEnviromentId();
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
		
		setTimeout(function(){
			var sendCodeBtn = document.getElementById("sendCodeBtn");
			if(sendCodeBtn) {
				sendCodeBtn.click();
				phoneNumInput.focus();
			}
		}, 1000);
	)";

	boost::replace_all(code_authorizing, "XXXXXXXXXXX", id);

	Sleep(1000);

	frame->ExecuteJavaScript(code_authorizing, url, 0);

	SetEvent(_authorizing_code_complete_event);
}

void SmartHotelRobotMeituan::DoAuthorizCode(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
	std::string code_authoriz_code = R"(
		var codeInput = document.getElementById("codeInput");
		if(codeInput) {
			codeInput.value = "000000";
			codeInput.focus();
		}

		var phoneNumInput = document.getElementById("phoneNumInput");
		if(phoneNumInput) {
			phoneNumInput.focus();
		}

		if(codeInput) {
			codeInput.focus();
		}

		var iloginBtn = document.getElementById("iloginBtn");
		if(iloginBtn) {
			iloginBtn.click();
		}
	)";

	boost::replace_all(code_authoriz_code, "000000", _authorize_code);

	frame->ExecuteJavaScript(code_authoriz_code, url, 0);

	SetEvent(_authorize_code_complete_event);
}