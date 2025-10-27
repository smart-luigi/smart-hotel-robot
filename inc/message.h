#ifndef _SMART_HOTEL_MESSAGE_H_
#define _SMART_HOTEL_MESSAGE_H_
#pragma once

#define	MESSAGE_TOPIC_SERVER			"smart-hotel-server"
#define	MESSAGE_TOPIC_ROBOT				"smart-hotel-robot"
#define	MESSAGE_SIZE					1024 * 1024

#define MESSAGE_RANGE_BEGIN				1

#define MESSAGE_ROBOT_START				2
#define MESSAGE_ROBOT_STARTED			3
#define MESSAGE_ROBOT_STOP				4
#define MESSAGE_ROBOT_STOPPED			5
#define MESSAGE_ROBOT_QUERY				6

#define MESSAGE_ROBOT_NEED_SMS_AUTH		6
#define MESSAGE_ROBOT_HOTEL_LIST_READY	7

#define MESSAGE_RANGE_END				100

#define WM_EXIT							WM_USER + 100

#define WM_SIMULATE_START				WM_USER + 200

enum SmartHotelRobotType
{
	TYPE_ROBOT_UNKNOWN = 0,
	TYPE_ROBOT_MEITUAN = 1,
	TYPE_ROBOT_CTRIP = 2,
};

typedef struct tagRobotMessageHeader
{
	unsigned int	message;
	char			id[12];
	unsigned int	type;
} RobotMessageHeader;

#define CreateRobotMessageHeader(header, _message, _id, _type)	\
{																\
	((RobotMessageHeader*)header)->message = _message;			\
	lstrcpyA(((RobotMessageHeader*)header)->id, _id);			\
	((RobotMessageHeader*)header)->type = _type;				\
}

typedef struct tagMessageRobotStatus
{
	RobotMessageHeader	header;
	bool				authorized;
} MessageRobotStatus, *MessageRobotStatusPtr;

typedef struct tagMessageHotelList
{
	RobotMessageHeader	header;
	unsigned int		response_length;
	char* response_content;
} MessageHotelList, * MessageHotelListPtr;

#endif // !_SMART_HOTEL_MESSAGE_H_