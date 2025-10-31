#ifndef _SMART_HOTEL_MESSAGE_H_
#define _SMART_HOTEL_MESSAGE_H_
#pragma once

#define	MESSAGE_TOPIC_SERVER				"smart-hotel-server"
#define	MESSAGE_TOPIC_ROBOT					"smart-hotel-robot"
#define	MESSAGE_SIZE						1024 * 1024

#define MESSAGE_ROBOT_START					0x101
#define MESSAGE_ROBOT_STARTED				0x102
#define MESSAGE_ROBOT_STOP					0x201
#define MESSAGE_ROBOT_STOPPED				0x202
#define MESSAGE_ROBOT_AUTHORIZING			0x301
#define MESSAGE_ROBOT_AUTHORIZE_SMS			0x302
#define MESSAGE_ROBOT_AUTHORIZED			0x303
#define MESSAGE_ROBOT_QUERY_STATUS			0x401
#define MESSAGE_ROBOT_QUERY_HOTELS			0x402

#define WM_EXIT								WM_USER + 100
#define WM_SIMULATE_START					WM_USER + 200

enum SmartHotelRobotType
{
	TYPE_ROBOT_UNKNOWN	= 0,
	TYPE_ROBOT_MEITUAN	= 1,
	TYPE_ROBOT_CTRIP	= 2,
};

enum SmartHotelRobotStatus
{
	STATUS_ROBOT_UNAUTHORIZE	= 0,
	STATUS_ROBOT_AUTHORIZE		= 1,
};

typedef struct tagMessageRobotHeader
{
	unsigned int	message;
	char			id[12];
	unsigned int	type;
} MessageRobotHeader;

#define CreateMessageRobotHeader(header, _message, _id, _type)	\
{																\
	((MessageRobotHeader*)header)->message = _message;			\
	lstrcpyA(((MessageRobotHeader*)header)->id, _id);			\
	((MessageRobotHeader*)header)->type = _type;				\
}

typedef struct tagMessageRobotStatus
{
	MessageRobotHeader	header;
	bool				authorized;
} MessageRobotStatus, *MessageRobotStatusPtr;

typedef struct tagMessageRobotAuthorizeSms
{
	MessageRobotHeader	header;
	char				sms[16];
} MessageRobotAuthorizeSms, *MessageRobotAuthorizeSmsPtr;

typedef struct tagMessageRobotHotels
{
	MessageRobotHeader	header;
	unsigned int		response_length;
	char*				response_content;
} MessageRobotHotels, *MessageRobotHotelsPtr;

#endif // !_SMART_HOTEL_MESSAGE_H_