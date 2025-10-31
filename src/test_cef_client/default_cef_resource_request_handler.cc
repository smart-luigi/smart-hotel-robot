#include <smart_base.h>
#include <cef/wrapper/cef_helpers.h>
#include "default_cef_resource_request_handler.h"

DefaultResourceRequestHandler::DefaultResourceRequestHandler()
	: _response_filter(new DefaultCefResponseFilter)
{

}

cef_return_value_t DefaultResourceRequestHandler::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefRequest> request,
	CefRefPtr<CefCallback> callback) {
	CefString* url = new CefString(request->GetURL());
	SmartLogInfo("OnBeforeResourceLoad url = %s", url->ToString().c_str());

	return RV_CONTINUE_ASYNC;
}

CefRefPtr<CefResponseFilter> DefaultResourceRequestHandler::GetResourceResponseFilter(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefRequest> request,
	CefRefPtr<CefResponse> response) {
	return _response_filter;
}

void DefaultResourceRequestHandler::OnResourceLoadComplete(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefRequest> request,
	CefRefPtr<CefResponse> response,
	URLRequestStatus status,
	int64_t received_content_length) {
	if (!CefCurrentlyOn(TID_IO))
		return;

	CefString* url = new CefString(request->GetURL());
	SmartLogInfo("OnResourceLoadComplete url = %s", url->ToString().c_str());
}