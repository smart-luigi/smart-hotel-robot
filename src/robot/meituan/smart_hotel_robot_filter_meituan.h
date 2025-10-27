#ifndef _DEFAULT_CEF_RESPONSE_FILTER_H_
#define _DEFAULT_CEF_RESPONSE_FILTER_H_
#pragma once

#include <cef/base/cef_bind.h>
#include <cef/cef_response_filter.h>
#include "smart_hotel_robot_context.h"

class SmartHotelRobotResponseFilterMeituan
	: public CefResponseFilter
{
public:
	SmartHotelRobotResponseFilterMeituan(SmartHotelRobotContext* context);
public:
	virtual bool InitFilter() override;
	virtual cef_response_filter_status_t Filter(void* data_in, size_t data_in_size, size_t& data_in_read,
		void* data_out, size_t data_out_size, size_t& data_out_written) override;
private:
	IMPLEMENT_REFCOUNTING(SmartHotelRobotResponseFilterMeituan);
	DISALLOW_COPY_AND_ASSIGN(SmartHotelRobotResponseFilterMeituan);
private:
	SmartHotelRobotContext*	_context;
};

#endif // !_DEFAULT_CEF_RESPONSE_FILTER_H_
