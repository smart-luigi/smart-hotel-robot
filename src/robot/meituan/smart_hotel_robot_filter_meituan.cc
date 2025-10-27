#include "precomp.h"
#include "meituan/smart_hotel_robot_filter_meituan.h"

SmartHotelRobotResponseFilterMeituan::SmartHotelRobotResponseFilterMeituan(SmartHotelRobotContext* context)
    : _context(context)
{

}

bool SmartHotelRobotResponseFilterMeituan::InitFilter()
{
	return true;
}

cef_response_filter_status_t SmartHotelRobotResponseFilterMeituan::Filter(void* data_in, size_t data_in_size, size_t& data_in_read,
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