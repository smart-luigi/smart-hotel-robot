#include "smart_hotel_robot.h"

SmartHotelRobot::SmartHotelRobot(const char* id, SmartHotelRobotType type, const char* topic)
	: _robot_id(id)
	, _robot_type(type)
	, _robot_topic(topic)
{
	
}

SmartHotelRobot::~SmartHotelRobot()
{

}

const char* SmartHotelRobot::GetRobotId()
{
	return _robot_id.c_str();
}

SmartHotelRobotType SmartHotelRobot::GetRobotType()
{
	return _robot_type;
}

const char* SmartHotelRobot::GetRobotTopic()
{
	return _robot_topic.c_str();
}

SmartHotelRobots::SmartHotelRobots()
{

}

SmartHotelRobots::~SmartHotelRobots()
{
	DestroyRobots();
}

void SmartHotelRobots::AddRobot(const char* id, SmartHotelRobotType type)
{
	std::string topic;
	CreateRobotTopic(id, type, topic);

	std::lock_guard<std::mutex> lock(_robots_lock);
	std::map<std::string, SmartHotelRobot*>::iterator it = _robots.find(topic);
	if (it == _robots.end())
	{
		_robots.insert(std::pair<std::string, SmartHotelRobot*>(topic.c_str(), new SmartHotelRobot(id, type, topic.c_str())));
	}
}

void SmartHotelRobots::RemoveRobot(const char* id, SmartHotelRobotType type)
{
	std::string topic;
	CreateRobotTopic(id, type, topic);

	std::lock_guard<std::mutex> lock(_robots_lock);
	std::map<std::string, SmartHotelRobot*>::iterator it = _robots.find(topic);
	if (it != _robots.end())
	{
		SmartHotelRobot* robot = it->second;
		if (robot)
		{
			delete robot;
			robot = nullptr;
		}

		_robots.erase(it);
	}
}

SmartHotelRobot* SmartHotelRobots::GetRobot(const char* id, SmartHotelRobotType type)
{
	std::string topic;
	CreateRobotTopic(id, type, topic);

	{
		std::lock_guard<std::mutex> lock(_robots_lock);
		std::map<std::string, SmartHotelRobot*>::iterator it = _robots.find(topic);
		if (it != _robots.end())
			return it->second;
	}

	return nullptr;
}

void SmartHotelRobots::CreateRobotTopic(const char* id, SmartHotelRobotType type, std::string& topic)
{
	topic.clear();
	topic.append(MESSAGE_TOPIC_ROBOT);
	topic.append("-");
	topic.append(id);
	topic.append("-");
	topic.append(std::to_string(type));
}

void SmartHotelRobots::DestroyRobots()
{
	for (std::map<std::string, SmartHotelRobot*>::iterator it = _robots.begin(); it != _robots.end(); it++)
	{
		SmartHotelRobot* robot = it->second;
		if (robot)
		{
			delete robot;
			robot = nullptr;
		}
	}

	_robots.clear();
}