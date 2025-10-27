#ifndef _SMART_HOTEL_SERVER_H_
#define _SMART_HOTEL_SERVER_H_
#pragma once

#include <queue>
#include <mutex>
#include <atomic>
#include <string>
#include <smart_adv.h>
#include <boost/asio.hpp>
#include "httplib.h"
#include "message.h"
#include "service_ipc.h"

class SmartHotelServer
	: public SmartThreadTask
{
public:
	SmartHotelServer();
protected:
	virtual ~SmartHotelServer();
public:
	int Init();
	void Dispose();
	int Run();
protected:
	int InitMessageServer();
	int InitHttpServer();
protected:
	virtual void OnThreadHandle(void* parameter) override;
	virtual int HandleMessage(LPCSTR ipc_name,
		LPCVOID message_buffer,
		DWORD   message_length,
		LPVOID  answer_buffer,
		DWORD   answer_length);
protected:
	int HandleMessageRobotStarted(LPCSTR ipc_name,
		LPCVOID message_buffer,
		DWORD   message_length,
		LPVOID  answer_buffer,
		DWORD   answer_length);
	int HandleMessageRobotStopped(LPCSTR ipc_name,
		LPCVOID message_buffer,
		DWORD   message_length,
		LPVOID  answer_buffer,
		DWORD   answer_length);
	int HandleMessageRobotNeedSmsAuth(LPCSTR ipc_name,
		LPCVOID message_buffer,
		DWORD   message_length,
		LPVOID  answer_buffer,
		DWORD   answer_length);
	int HandleMessageRobotHotelListReady(LPCSTR ipc_name,
		LPCVOID message_buffer,
		DWORD   message_length,
		LPVOID  answer_buffer,
		DWORD   answer_length);
protected:
	int StartRobot(const char* id, unsigned int type);
	int StopRobot(const char* id);
	int QueryRobot(const char* id);
protected:
	int AddRobotMessageClient(const char* id);
	void RemoveRobotMessageClient(const char* id);
	SmartMessageClient* GetRobotMessageClient(const char* id);
private:
	static void WINAPI IpcMessageCallback(LPCSTR ipc_name,
		LPCVOID ipc_context,
		LPCVOID message_buffer,
		DWORD   message_length,
		LPVOID  answer_buffer,
		DWORD   answer_length);
private:
	SmartThreadPool*							_threadpool;
	httplib::Server*							_http_server;
	boost::shared_ptr<boost::asio::io_context>	_io_context;
	boost::shared_ptr<boost::asio::signal_set>	_shutdown_signals;
	std::map<std::string, SmartMessageClient*>	_robot_message_clients;
	std::mutex									_robot_message_clients_lock;
};

#endif // !_SMART_HOTEL_SERVER_H_