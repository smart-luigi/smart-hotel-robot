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
#include "service_ipc.h"
#include "smart_hotel_message.h"
#include "smart_hotel_robot.h"

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
	virtual int HandleMessageRobotStarted(LPCSTR ipc_name,
		LPCVOID message_buffer,
		DWORD   message_length,
		LPVOID  answer_buffer,
		DWORD   answer_length);
	virtual int HandleMessageRobotStopped(LPCSTR ipc_name,
		LPCVOID message_buffer,
		DWORD   message_length,
		LPVOID  answer_buffer,
		DWORD   answer_length);
	virtual int HandleMessageRobotAuthorized(LPCSTR ipc_name,
		LPCVOID message_buffer,
		DWORD   message_length,
		LPVOID  answer_buffer,
		DWORD   answer_length);
protected:
	void OnHttpHandleStartRobot(const httplib::Request& req, httplib::Response& res);
	void OnHttpHandleStopRobot(const httplib::Request& req, httplib::Response& res);
	void OnHttpHandleQueryRobotStatus(const httplib::Request& req, httplib::Response& res);
	void OnHttpHandleQueryRobotHotels(const httplib::Request& req, httplib::Response& res);
	void OnHttpHandleAuthorizingRobot(const httplib::Request& req, httplib::Response& res);
	void OnHttpHandleAuthorizeRobotSms(const httplib::Request& req, httplib::Response& res);
	bool OnHttpHandleValidateParameters(const httplib::Request& req, httplib::Response& res, std::string& phone, unsigned int* type);
	void OnHttpHandleError(const httplib::Request& req, httplib::Response& res);
protected:
	int StartRobot(const char* id, unsigned int type);
	int StopRobot(const char* id, unsigned int type);
	int QueryRobotStatus(const char* id, unsigned int type, char* response, unsigned int response_length);
	int QueryRobotHotels(const char* id, unsigned int type, char* response, unsigned int response_length);
	int AuthorizingRobot(const char* id, unsigned int type, char* response, unsigned int response_length);
	int AuthorizeRobotSms(const char* id, unsigned int type, char* response, unsigned int response_length);
protected:
	int SendRobotMessage(const char* id, unsigned int type, LPCVOID message_buffer, DWORD message_length);
	int SendRobotMessage(const char* id, unsigned int type, LPCVOID message_buffer, DWORD message_length, PVOID answer_buffer, DWORD answer_length);
	int SendRobotMessage(const char* id, unsigned int type, LPCVOID message_buffer, DWORD message_length, PVOID answer_buffer, DWORD answer_length, DWORD answer_timeout);
protected:
	void GetRobotMessageTopic(const char* id, std::string& topic);
protected:
	void CreateErrorResponse(int code, const char* message, std::string& response);
	void CreateSuccessResponse(std::string& response);
	void CreateSuccessResponse(const char* data, std::string& response);
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
	SmartHotelRobots							_robots;
};

#endif // !_SMART_HOTEL_SERVER_H_