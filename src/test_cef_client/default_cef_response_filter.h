#ifndef _DEFAULT_CEF_RESPONSE_FILTER_H_
#define _DEFAULT_CEF_RESPONSE_FILTER_H_
#pragma once

#include <cef/base/cef_bind.h>
#include <cef/cef_response_filter.h>

class DefaultCefResponseFilter 
	: public CefResponseFilter
{
public:
	DefaultCefResponseFilter();
public:
	virtual bool InitFilter() override;
	virtual cef_response_filter_status_t Filter(void* data_in, size_t data_in_size, size_t& data_in_read,
		void* data_out, size_t data_out_size, size_t& data_out_written) override;
private:
	IMPLEMENT_REFCOUNTING(DefaultCefResponseFilter);
	DISALLOW_COPY_AND_ASSIGN(DefaultCefResponseFilter);
};

#endif // !_DEFAULT_CEF_RESPONSE_FILTER_H_
