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
	void OnHttpHandleRobotStart(const httplib::Request& req, httplib::Response& res);
	void OnHttpHandleRobotStop(const httplib::Request& req, httplib::Response& res);
	void OnHttpHandleRobotAuthorizeAccountStart(const httplib::Request& req, httplib::Response& res);
	void OnHttpHandleRobotAuthorizeAccount(const httplib::Request& req, httplib::Response& res);
	void OnHttpHandleRobotAuthorizeCodeStart(const httplib::Request& req, httplib::Response& res);
	void OnHttpHandleRobotAuthorizeCode(const httplib::Request& req, httplib::Response& res);
	void OnHttpHandleRobotQueryStatus(const httplib::Request& req, httplib::Response& res);
	void OnHttpHandleRobotQueryAccount(const httplib::Request& req, httplib::Response& res);
	void OnHttpHandleRobotQueryHotels(const httplib::Request& req, httplib::Response& res);
	bool OnHttpHandleValidateParameters(const httplib::Request& req, httplib::Response& res, std::string& phone, unsigned int* type);
	void OnHttpHandleError(const httplib::Request& req, httplib::Response& res);
protected:
	int HandleRobotStart(const char* id, unsigned int type);
	int HandleRobotStop(const char* id, unsigned int type);
	int HandleRobotAuthorizeAccountStart(const char* id, unsigned int type, char* response, unsigned int response_length);
	int HandleRobotAuthorizeAccount(const char* id, const char* password, unsigned int type, char* response, unsigned int response_length);
	int HandleRobotAuthorizeCodeStart(const char* id, unsigned int type, char* response, unsigned int response_length);
	int HandleRobotAuthorizeCode(const char* id, const char* code, unsigned int type, char* response, unsigned int response_length);
	int HandleRobotQueryAccount(const char* id, unsigned int type, char* response, unsigned int response_length);
	int HandleRobotQueryStatus(const char* id, unsigned int type, char* response, unsigned int response_length);
	int HandleRobotQueryHotels(const char* id, unsigned int type, char* response, unsigned int response_length);
protected:
	int SendRobotMessage(const char* id, unsigned int type, LPCVOID message_buffer, DWORD message_length);
	int SendRobotMessage(const char* id, unsigned int type, LPCVOID message_buffer, DWORD message_length, PVOID answer_buffer, DWORD answer_length);
	int SendRobotMessage(const char* id, unsigned int type, LPCVOID message_buffer, DWORD message_length, PVOID answer_buffer, DWORD answer_length, DWORD answer_timeout);
protected:
	void GetRobotMessageTopic(const char* id, std::string& topic);
protected:
	void CreateErrorResponse(int code, const char* message, httplib::Response& res);
	void CreateSuccessResponse(httplib::Response& res);
	void CreateSuccessResponse(const char* data, httplib::Response& res);
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