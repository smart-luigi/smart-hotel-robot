#ifndef _SMART_HOTEL_MESSAGE_H_
#define _SMART_HOTEL_MESSAGE_H_
#pragma once
#include <windows.h>

#define	MESSAGE_TOPIC_SERVER					"smart-hotel-server"
#define	MESSAGE_TOPIC_ROBOT						"smart-hotel-robot"
#define	MESSAGE_SIZE							1024 * 1024

#define MESSAGE_ROBOT_START						0x101
#define MESSAGE_ROBOT_STARTED					0x102
#define MESSAGE_ROBOT_STOP						0x201
#define MESSAGE_ROBOT_STOPPED					0x202
#define MESSAGE_ROBOT_AUTHORIZE_ACCOUNT_START	0x301
#define MESSAGE_ROBOT_AUTHORIZE_ACCOUNT			0x302
#define MESSAGE_ROBOT_AUTHORIZE_CODE_START		0x303
#define MESSAGE_ROBOT_AUTHORIZE_CODE			0x304
#define MESSAGE_ROBOT_AUTHORIZED				0x305
#define MESSAGE_ROBOT_QUERY_ACCOUNT				0x401
#define MESSAGE_ROBOT_QUERY_STATUS				0x402
#define MESSAGE_ROBOT_QUERY_HOTELS				0x403

#define WM_EXIT									WM_USER + 100
#define WM_LOAD_URL								WM_USER + 200
#define WM_SIMULATE_START						WM_USER + 300

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

typedef struct tagMessageRobotAuthorizeAccount
{
	MessageRobotHeader	header;
	char				password[32];
} MessageRobotAuthorizeAccount, *MessageRobotAuthorizeAccountPtr;

typedef struct tagMessageRobotAuthorizeCode
{
	MessageRobotHeader	header;
	char				code[16];
} MessageRobotAuthorizeCode, *MessageRobotAuthorizeCodePtr;

typedef struct tagMessageRobotStatus
{
	MessageRobotHeader	header;
	bool				authorized;
} MessageRobotStatus, * MessageRobotStatusPtr;

typedef struct tagMessageRobotHotel
{
	unsigned int	index;
	char			image[MAX_PATH];
	char			title[MAX_PATH];
	char			score[32];
	char			feedback[128];
	char			star[32];
	char			address[MAX_PATH];
	char			price[8];
	char			orig_price[8];
	char			bought[32];
} MessageRobotHotel, *MessageRobotHotelPtr;

typedef struct tagMessageRobotHotels
{
	MessageRobotHeader	header;
	unsigned int		response_length;
	char*				response_content;
} MessageRobotHotels, *MessageRobotHotelsPtr;

#endif // !_SMART_HOTEL_MESSAGE_H_