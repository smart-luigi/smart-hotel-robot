#ifndef _SMART_HOTEL_ROBOT_H_
#define _SMART_HOTEL_ROBOT_H_
#pragma once

#include <string>
#include <map>
#include <mutex>
#include "smart_hotel_message.h"

class SmartHotelRobot
{
public:
	SmartHotelRobot(const char* id, SmartHotelRobotType type, const char* topic);
	virtual ~SmartHotelRobot();
public:
	const char* GetRobotId();
	SmartHotelRobotType GetRobotType();
	const char* GetRobotTopic();
private:
	std::string				_robot_id;
	SmartHotelRobotType		_robot_type;
	std::string				_robot_topic;
};

class SmartHotelRobots
{
public:
	SmartHotelRobots();
	virtual ~SmartHotelRobots();
public:
	void AddRobot(const char* id, SmartHotelRobotType type);
	void RemoveRobot(const char* id, SmartHotelRobotType type);
	SmartHotelRobot* GetRobot(const char* id, SmartHotelRobotType type);
private:
	void CreateRobotTopic(const char* id, SmartHotelRobotType type, std::string& topic);
	void DestroyRobots();
private:
	std::map<std::string, SmartHotelRobot*>	_robots;
	std::mutex								_robots_lock;
};

#endif // !_SMART_HOTEL_ROBOT_H_