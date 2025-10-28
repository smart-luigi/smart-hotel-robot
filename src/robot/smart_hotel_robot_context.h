#ifndef _SMART_HOTEL_ROBOT_CONTEXT_H_
#define _SMART_HOTEL_ROBOT_CONTEXT_H_
#pragma once

#include <string>
#include <smart_adv.h>
#include <smart_cef_client.h>
#include "smart_hotel_robot.h"

class SmartHotelRobotContext
{
public:
	
public:
	SmartHotelRobotContext();
protected:
	virtual ~SmartHotelRobotContext();
public:
	int Init(int argc, wchar_t* argv[]);
	void Dispose();
public:
	const char* GetWindowTitle();
	const char* GetSingletonName();
	const char* GetLoginUrl();
	const char* GetListUrl();
	const char* GetDataUrl();
	const char* GetCacheEnviromentId();
	const char* GetCacheEnviromentPath();
	SmartHotelRobotType GetCacheEnviromentType();
	bool ValidateCacheEnviroment();
	SmartHotelRobot* GetHotelRobot();
	CefClientApplication* GetApplication();
	void SetApplication(CefClientApplication* application);
public:
	int SendServerMessage(LPCVOID message_buffer, DWORD message_length);
	int SendServerMessage(LPCVOID message_buffer, DWORD message_length, PVOID answer_buffer, DWORD answer_length);
	int SendServerMessage(LPCVOID message_buffer, DWORD message_length, PVOID answer_buffer, DWORD answer_length, DWORD answer_timeout);
	int SendServerMessageRobotStarted();
	int SendServerMessageRobotStopped();
	int SendServerMessageRobotNeedSmsAuthorize(char* sms, DWORD sms_length);
protected:
	int InitCacheEnviroment(int argc, wchar_t** argv, const wchar_t* cache_env_id, const wchar_t* cache_env_type);
	int InitHotelRobot();
protected:
	int HandleServer
protected:
	virtual int HandleMessage(LPCSTR ipc_name,
		LPCVOID message_buffer,
		DWORD   message_length,
		LPVOID  answer_buffer,
		DWORD   answer_length);
protected:
	static void WINAPI IpcMessageCallback(LPCSTR ipc_name,
		LPCVOID ipc_context,
		LPCVOID message_buffer,
		DWORD   message_length,
		LPVOID  answer_buffer,
		DWORD   answer_length);
protected:
	SmartThreadPool*		_threadpool;
	CefClientApplication*	_application;
	std::string				_window_title;
	std::string				_singleton_name;
	std::string				_cache_env_id;
	std::string				_cache_env_path;
	std::string				_topic_robot_name;
	SmartHotelRobotType		_cache_env_type;
	SmartHotelRobot*		_hotel_robot;
};

#endif // !_SMART_HOTEL_ROBOT_CONTEXT_H_
