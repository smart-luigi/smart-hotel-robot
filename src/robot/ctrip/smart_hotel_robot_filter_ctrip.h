#ifndef _SMART_HOTEL_RESPONSE_FILTER_CTRIP_H_
#define _SMART_HOTEL_RESPONSE_FILTER_CTRIP_H_
#pragma once

#include <cef/base/cef_bind.h>
#include <cef/cef_response_filter.h>
#include "smart_hotel_robot_context.h"

class SmartHotelRobotResponseFilterCtrip
	: public CefResponseFilter
{
public:
	SmartHotelRobotResponseFilterCtrip(SmartHotelRobotContext* context);
public:
	virtual bool InitFilter() override;
	virtual cef_response_filter_status_t Filter(void* data_in, size_t data_in_size, size_t& data_in_read,
		void* data_out, size_t data_out_size, size_t& data_out_written) override;
private:
	IMPLEMENT_REFCOUNTING(SmartHotelRobotResponseFilterCtrip);
	DISALLOW_COPY_AND_ASSIGN(SmartHotelRobotResponseFilterCtrip);
private:
	SmartHotelRobotContext*	_context;
};

#endif // !_SMART_HOTEL_RESPONSE_FILTER_CTRIP_H_
