#ifndef _DEFAULT_CEF_RESOURCE_REQUEST_HANDLER_H_
#define _DEFAULT_CEF_RESOURCE_REQUEST_HANDLER_H_
#pragma once

#include <cef/base/cef_bind.h>
#include <cef/cef_resource_request_handler.h>
#include "default_cef_response_filter.h"

class DefaultResourceRequestHandler
	: public CefResourceRequestHandler
{
public:
	DefaultResourceRequestHandler();
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
	IMPLEMENT_REFCOUNTING(DefaultResourceRequestHandler);
	DISALLOW_COPY_AND_ASSIGN(DefaultResourceRequestHandler);
private:
	CefRefPtr<CefResponseFilter> _response_filter;
};

#endif // !_DEFAULT_CEF_RESOURCE_REQUEST_HANDLER_H_