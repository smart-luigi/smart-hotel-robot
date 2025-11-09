#include "precomp.h"
#include <boost/property_tree/json_parser.hpp>
#include "meituan/smart_hotel_robot_meituan.h"

SmartHotelRobotMeituan::SmartHotelRobotMeituan(SmartHotelRobotContext* context)
	: _context(context)
	, _url_login("https://passport.meituan.com/useraccount/ilogin")
	, _url_account("https://i.meituan.com/mttouch/page/account")
	, _url_list("https://i.meituan.com/awp/h5/hotel/list/list.html")
	, _url_data("https://ihotel.meituan.com/hbsearch/HotelSearch")
	, _authorized(false)
	, _authorize_account_start_event(nullptr)
	, _authorize_account_complete_event(nullptr)
	, _authorize_code_start_event(nullptr)
	, _authorize_code_complete_event(nullptr)
	, _authorize_password_start_event(nullptr)
{

}

SmartHotelRobotMeituan::~SmartHotelRobotMeituan()
{
	if (_authorize_password_complete_event)
	{
		CloseHandle(_authorize_password_complete_event);
		_authorize_password_complete_event = nullptr;
	}

	if (_authorize_password_start_event)
	{
		CloseHandle(_authorize_password_start_event);
		_authorize_password_start_event = nullptr;
	}

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

	if (_authorize_account_complete_event)
	{
		CloseHandle(_authorize_account_complete_event);
		_authorize_account_complete_event = nullptr;
	}

	if (_authorize_account_start_event)
	{
		CloseHandle(_authorize_account_start_event);
		_authorize_account_start_event = nullptr;
	}

	for (auto it = _hotels_set.begin(); it != _hotels_set.end(); it++)
	{
		MessageRobotHotel* hotel = *it;
		if (hotel)
		{
			SmartMemFree(hotel);
			hotel = nullptr;
		}
	}

	_hotels_set.clear();
}

int SmartHotelRobotMeituan::Init()
{
	int result = ERROR_SUCCESS;

	do
	{
		_authorize_account_start_event = CreateEvent(nullptr, true, false, nullptr);
		if (_authorize_account_start_event == nullptr)
		{
			result = GetLastError();
			break;
		}

		_authorize_account_complete_event = CreateEvent(nullptr, true, false, nullptr);
		if (_authorize_account_complete_event == nullptr)
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

		_authorize_password_start_event = CreateEvent(nullptr, true, false, nullptr);
		if (_authorize_password_start_event == nullptr)
		{
			result = GetLastError();
			break;
		}

		_authorize_password_complete_event = CreateEvent(nullptr, true, false, nullptr);
		if (_authorize_password_complete_event == nullptr)
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
	boost::property_tree::ptree items;
	for (auto it = _hotels_set.begin(); it != _hotels_set.end(); it++)
	{
		MessageRobotHotel* hotel = *it;
		if (hotel)
		{
			boost::property_tree::ptree item;
			item.put<int>("index", hotel->index);
			item.put<std::string>("image", hotel->image);
			item.put<std::string>("title", hotel->title);
			item.put<std::string>("score", hotel->score);
			item.put<std::string>("feedback", hotel->feedback);
			item.put<std::string>("star", hotel->star);
			item.put<std::string>("address", hotel->address);
			item.put<std::string>("price", hotel->price);
			item.put<std::string>("orig_price", hotel->orig_price);
			item.put<std::string>("bought", hotel->bought);
			items.push_back(std::make_pair("", item));
		}
	}

	boost::property_tree::ptree doc;
	doc.add_child("hotels", items);

	std::ostringstream oss;
	boost::property_tree::write_json(oss, doc);

	_hotels_data.clear();
	_hotels_data.assign(oss.str().c_str());
	
	return _hotels_data.c_str();
}

void SmartHotelRobotMeituan::SetData(const char* data)
{
	_hotels_data.clear();
	_hotels_data.append(data);
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

void SmartHotelRobotMeituan::AuthorizeAccountPassword(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
	std::thread([](CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url, SmartHotelRobotMeituan* robot) {

		robot->RedirectAccountLogin(browser, frame, url);

		robot->WaitAuthorizeAccountPasswordStart();

		robot->DoAuthorizAccountPassword(browser, frame, url);

		robot->WaitAuthorizeAccountPasswordComplete();

	}, browser, frame, url, this).detach();
}

void SmartHotelRobotMeituan::AuthorizeAccountCode(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
	std::thread([](CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url, SmartHotelRobotMeituan* robot) {

		robot->WaitAuthorizeAccountStart();

		robot->DoAuthorizeAccount(browser, frame, url);

		robot->WaitAuthorizeAccountComplete();

		robot->WaitAuthorizeCodeStart();

		robot->DoAuthorizeCode(browser, frame, url);

		robot->WaitAuthorizeCodeComplete();

	}, browser, frame, url, this).detach();
}

void SmartHotelRobotMeituan::QueryHotels(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
	std::thread([](CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) {
		Sleep(3000);
		std::string code_scroll = R"(
			var scrollHeight = 0;
			function ScrollPage() {
				if(scrollHeight >= document.body.scrollHeight) {
					window.cefQuery({
						request: document.body.innerHTML,
						onSuccess: function(response) {},
						onFailure: function(error_code, error_message) {}
					});
				} else {
					window.scrollBy(0, document.body.scrollHeight);
					scrollHeight = document.body.scrollHeight;
					setTimeout(function() {
						ScrollPage();
					}, 2000);
				}
			}
			ScrollPage();
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

void SmartHotelRobotMeituan::AddHotel(MessageRobotHotel* hotel)
{
	MessageRobotHotel* item = (MessageRobotHotel*)SmartMemAlloc(sizeof(MessageRobotHotel));
	if (item)
	{
		std::memcpy(item, hotel, sizeof(MessageRobotHotel));
		_hotels_set.insert(item);
	}
}

void SmartHotelRobotMeituan::HandleAuthorizeAccountPassword(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{
	MessageRobotAuthorizePasswordPtr message = (MessageRobotAuthorizePasswordPtr)message_buffer;
	if (message == nullptr)
		return;

	_authorize_password.clear();
	_authorize_password.append(message->password);

	SetEvent(_authorize_password_start_event);
}

void SmartHotelRobotMeituan::HandleAuthorizeAccount(const void* message_buffer, unsigned int message_length, void* answer_buffer, unsigned int answer_length)
{
	SetEvent(_authorize_account_start_event);
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

	const char* data = GetData();
	size_t data_length = lstrlenA(data);
	if (data_length <= answer_length)
	{
		std::memcpy(answer_buffer, data, data_length);
	}
	else
	{
		std::memcpy(answer_buffer, data, answer_length);
	}
}

void SmartHotelRobotMeituan::RedirectAccountLogin(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
	std::string id = _context->GetCacheEnviromentId();
	std::string code_redirect_login = R"(
		var toAccountLogin = document.getElementById("toAccountLogin");
		if(toAccountLogin) {
			toAccountLogin.click();
		}
	)";

	Sleep(1000);

	frame->ExecuteJavaScript(code_redirect_login, url, 0);
}

void SmartHotelRobotMeituan::DoAuthorizAccountPassword(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
	std::string id = _context->GetCacheEnviromentId();
	std::string code_authorize_account_password = R"(
		var pwInputChecked = document.getElementById("pwInputChecked");
		if(pwInputChecked) {
			pwInputChecked.click();
		}

		var accountInput = document.getElementById("accountInput");
		if(accountInput) {
			accountInput.focus();
			accountInput.value = "XXXXXXXXXXX";
		}

		var accountPwInput = document.getElementById("accountPwInput");
		if(accountPwInput) {
			accountPwInput.focus();
			accountPwInput.value = "YYYYYYYYYYY";
		}
		
		setTimeout(function(){
			var accountLogin = document.getElementById("accountLogin");
			if(accountLogin) {
				accountInput.focus();
				accountLogin.click();
			}
		}, 1000);
	)";

	boost::replace_all(code_authorize_account_password, "XXXXXXXXXXX", id);
	boost::replace_all(code_authorize_account_password, "YYYYYYYYYYY", _authorize_password);

	frame->ExecuteJavaScript(code_authorize_account_password, url, 0);

	SetEvent(_authorize_password_complete_event);
}

void SmartHotelRobotMeituan::WaitAuthorizeAccountPasswordStart()
{
	WaitForSingleObject(_authorize_password_start_event, INFINITE);
	ResetEvent(_authorize_password_start_event);
}

void SmartHotelRobotMeituan::WaitAuthorizeAccountPasswordComplete()
{
	WaitForSingleObject(_authorize_password_complete_event, INFINITE);
	ResetEvent(_authorize_password_complete_event);
}

void SmartHotelRobotMeituan::WaitAuthorizeAccountStart()
{
	WaitForSingleObject(_authorize_account_start_event, INFINITE);
	ResetEvent(_authorize_account_start_event);
}

void SmartHotelRobotMeituan::WaitAuthorizeAccountComplete()
{
	WaitForSingleObject(_authorize_account_complete_event, INFINITE);
	ResetEvent(_authorize_account_complete_event);
}

void SmartHotelRobotMeituan::WaitAuthorizeCodeStart()
{
	WaitForSingleObject(_authorize_code_start_event, INFINITE);
	ResetEvent(_authorize_code_start_event);
}

void SmartHotelRobotMeituan::WaitAuthorizeCodeComplete()
{
	WaitForSingleObject(_authorize_code_complete_event, INFINITE);
	ResetEvent(_authorize_code_complete_event);
}

void SmartHotelRobotMeituan::DoAuthorizeAccount(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
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

	SetEvent(_authorize_account_complete_event);
}

void SmartHotelRobotMeituan::DoAuthorizeCode(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
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