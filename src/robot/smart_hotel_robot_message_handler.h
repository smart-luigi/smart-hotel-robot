#ifndef _SMART_HOTEL_ROBOT_MESSAGE_HANDLER_H_
#define _SMART_HOTEL_ROBOT_MESSAGE_HANDLER_H_
#pragma once

#include <smart_cef_client.h>
#include "smart_hotel_robot_context.h"

class SmartHotelRobotMessageHandler
	: public CefMessageRouterBrowserSide::Handler
{
public:
	SmartHotelRobotMessageHandler(SmartHotelRobotContext* context);
	virtual ~SmartHotelRobotMessageHandler();
protected:
    virtual bool OnQuery(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        int64_t query_id,
        const CefString& request,
        bool persistent,
        CefRefPtr<Callback> callback) override;
protected:
    void SearchForHotels(GumboNode* node, SmartHotelRobot* robot);
    void GetHotel(GumboNode* node, MessageRobotHotel& result);
    const char* GetHotelImage(GumboNode* node);
    const char* GetHotelTitle(GumboNode* node);
    const char* GetHotelScore(GumboNode* node);
    const char* GetHotelFeedback(GumboNode* node);
    const char* GetHotelStar(GumboNode* node);
    const char* GetHotelAddress(GumboNode* node);
    const char* GetHotelPrice(GumboNode* node);
    const char* GetHotelOrigPrice(GumboNode* node);
    const char* GetHotelBought(GumboNode* node);
protected:
    SmartHotelRobotContext* _context;
};

#endif // !_SMART_HOTEL_ROBOT_MESSAGE_HANDLER_H_