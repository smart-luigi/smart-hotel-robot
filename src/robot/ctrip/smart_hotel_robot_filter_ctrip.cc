#include <smart_base.h>
#include "ctrip/smart_hotel_robot_filter_ctrip.h"

SmartHotelRobotResponseFilterCtrip::SmartHotelRobotResponseFilterCtrip(SmartHotelRobotContext* context)
    : _context(context)
{

}

bool SmartHotelRobotResponseFilterCtrip::InitFilter()
{
	return true;
}

cef_response_filter_status_t SmartHotelRobotResponseFilterCtrip::Filter(void* data_in, size_t data_in_size, size_t& data_in_read,
	void* data_out, size_t data_out_size, size_t& data_out_written)
{
    data_out_written = std::min(data_in_size, data_out_size);

    data_in_read = data_out_written;

    if (data_out_written > 0) 
    {
        memcpy(data_out, data_in, data_out_written);
    }

    return RESPONSE_FILTER_DONE;
}