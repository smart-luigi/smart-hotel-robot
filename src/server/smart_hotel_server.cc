#include <boost/smart_ptr.hpp>
#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <smart_base.h>
#include "smart_hotel_server.h"

#ifdef _DEBUG
#define	ROBOT_EXECUABLE	"smart-hotel-robotd.exe"
#else
#define	ROBOT_EXECUABLE	"smart-hotel-robot.exe"
#endif // _DEBUG

SmartHotelServer::SmartHotelServer()
	: _threadpool(nullptr)
	, _http_server(nullptr)
	, _io_context(nullptr)
	, _shutdown_signals(nullptr)
{

}

SmartHotelServer::~SmartHotelServer()
{
	if (_io_context)
	{
		_io_context->stop();
		_io_context.reset();
	}

	if (_http_server)
	{
		delete _http_server;
		_http_server = nullptr;
	}

	SmartLogInfo("Shutdown Message Server");

	DestroyIpcQueue(MESSAGE_TOPIC_SERVER);

	SmartLogInfo("Shutdown Server Threadpool");

	if (_threadpool)
	{
		_threadpool->Dispose();
		_threadpool = nullptr;
	}

	SmartLogInfo("Shutdown Server");
}

int SmartHotelServer::Init()
{
	int result = ERROR_SUCCESS;

	do
	{
		_threadpool = SmartThreadPoolFactory::CreateThreadPool();
		if (_threadpool == nullptr)
		{
			result = ERROR_THREAD_NOT_IN_PROCESS;
			break;
		}

		result = _threadpool->Init();
		if (result != ERROR_SUCCESS)
			break;

		SmartLogInfo("Initialized Server Threadpool");

		_io_context = boost::make_shared<boost::asio::io_context>();
		if (_io_context == nullptr)
		{
			result = ERROR_IO_INCOMPLETE;
			break;
		}

		result = InitMessageServer();
		if (result != ERROR_SUCCESS)
			break;

		SmartLogInfo("Initialized Message Server");

		result = InitHttpServer();
		if (result != ERROR_SUCCESS)
			break;

		SmartLogInfo("Initialized Http Server");

		_shutdown_signals = boost::make_shared<boost::asio::signal_set>(*_io_context);
		if (_shutdown_signals == nullptr)
		{
			result = ERROR_SIGNAL_REFUSED;
			break;
		}

		_shutdown_signals->add(SIGINT);
		_shutdown_signals->add(SIGTERM);
#if defined(SIGQUIT)
		_signals->add(SIGQUIT);
#endif // defined(SIGQUIT)

		_shutdown_signals->async_wait(
			[this](boost::system::error_code ec, int signo)
			{
				if (_http_server)
				{
					_http_server->stop();
				}
			}
		);
	} while (false);

	return result;
}

void SmartHotelServer::Dispose()
{
	delete this;
}

int SmartHotelServer::Run()
{
	int result = ERROR_SUCCESS;

	do
	{
		SmartLogInfo("Startup Message Server");

		_threadpool->Execute(this, this);

		_io_context->run();

	} while (false);

	return result;
}

int SmartHotelServer::InitMessageServer()
{
	int result = ERROR_SUCCESS;

	do
	{
		if (!CreateIpcQueue(MESSAGE_TOPIC_SERVER, this, IpcMessageCallback))
		{
			result = ERROR_CALLBACK_SUPPLIED_INVALID_DATA;
			break;
		}
	} while (false);

	return result;
}

int SmartHotelServer::InitHttpServer()
{
	int result = ERROR_SUCCESS;
	char* server_root = nullptr;

	do
	{
		server_root = (char*)SmartMemAlloc(MAX_PATH);
		if (server_root == nullptr)
		{
			result = ERROR_NOT_ENOUGH_MEMORY;
			break;
		}

		if (!SmartFsGetAppPathA(server_root, "doc"))
		{
			result = ERROR_PATH_NOT_FOUND;
			break;
		}

		if (!SmartFsCreateDirectoryA(server_root))
		{
			result = ERROR_DIRECTORY_NOT_SUPPORTED;
			break;
		}

		_http_server = new httplib::Server();
		if (_http_server == nullptr)
		{
			result = ERROR_SERVER_DISABLED;
			break;
		}

		_http_server->set_tcp_nodelay(true);

		if (!_http_server->set_base_dir(server_root, "/smart-hotel"))
		{
			result = ERROR_SERVICE_NEVER_STARTED;
			break;
		}

		if (!_http_server->set_mount_point("/smart-hotel", server_root))
		{
			result = ERROR_SERVICE_NEVER_STARTED;
			break;
		}

		_http_server->set_error_handler(
			[=](const httplib::Request& req, httplib::Response& res)
			{
				this->OnHttpHandleError(req, res);
			}
		);

		_http_server->Get("/smart-hotel/api/start-robot",
			[=](const httplib::Request& req, httplib::Response& res)
			{
				this->OnHttpHandleStartRobot(req, res);
			}
		);

		_http_server->Get("/smart-hotel/api/stop-robot",
			[=](const httplib::Request& req, httplib::Response& res)
			{
				this->OnHttpHandleStopRobot(req, res);
			}
		);

		_http_server->Get("/smart-hotel/api/query-robot-status",
			[=](const httplib::Request& req, httplib::Response& res)
			{
				this->OnHttpHandleQueryRobotStatus(req, res);
			}
		);

		_http_server->Get("/smart-hotel/api/authorize-robot-start",
			[=](const httplib::Request& req, httplib::Response& res)
			{
				this->OnHttpHandleQueryRobotStatus(req, res);
			}
		);

		_http_server->Get("/smart-hotel/api/authorize-robot-sms",
			[=](const httplib::Request& req, httplib::Response& res)
			{
				this->OnHttpHandleQueryRobotStatus(req, res);
			}
		);

		_http_server->Get("/smart-hotel/api/query-robot-hotels",
			[=](const httplib::Request& req, httplib::Response& res)
			{
				this->OnHttpHandleQueryRobotHotels(req, res);
			}
		);
	} while (false);

	if (server_root)
	{
		SmartMemFree(server_root);
		server_root = nullptr;
	}

	return result;
}

void SmartHotelServer::OnThreadHandle(void* parameter)
{
	SmartLogInfo("Startup Http Server On Port 49090");

	while (true)
	{
		if (!_http_server->listen("0.0.0.0", 49090))
			break;
	}
	
	SmartLogInfo("Shutdown Http Server");
}

int SmartHotelServer::HandleMessage(LPCSTR ipc_name,
	LPCVOID message_buffer,
	DWORD   message_length,
	LPVOID  answer_buffer,
	DWORD   answer_length)
{
	int result = ERROR_SUCCESS;

	RobotMessageHeader* header = (RobotMessageHeader*)message_buffer;
	if (header == nullptr)
		return ERROR_DATA_LOST_REPAIR;

	unsigned int message = header->message;
	switch (message)
	{
	case MESSAGE_ROBOT_STARTED:
		result = HandleMessageRobotStarted(ipc_name, message_buffer, message_length, answer_buffer, answer_length);
		break;
	case MESSAGE_ROBOT_STOPPED:
		result = HandleMessageRobotStopped(ipc_name, message_buffer, message_length, answer_buffer, answer_length);
	case MESSAGE_ROBOT_AUTHORIZED:
		result = HandleMessageRobotAuthorized(ipc_name, message_buffer, message_length, answer_buffer, answer_length);
		break;
	default:
		break;
	}

	return result;
}

int SmartHotelServer::HandleMessageRobotStarted(LPCSTR ipc_name,
	LPCVOID message_buffer,
	DWORD   message_length,
	LPVOID  answer_buffer,
	DWORD   answer_length)
{
	int result = ERROR_SUCCESS;

	RobotMessageHeader* message = (RobotMessageHeader*)message_buffer;
	if (message)
	{
		SmartLogInfo("Hotel Robot [%s-%d] Started", message->id, message->type);
		_robots.AddRobot(message->id, (SmartHotelRobotType)message->type);
	}

	return result;
}

int SmartHotelServer::HandleMessageRobotStopped(LPCSTR ipc_name,
	LPCVOID message_buffer,
	DWORD   message_length,
	LPVOID  answer_buffer,
	DWORD   answer_length)
{
	int result = ERROR_SUCCESS;

	RobotMessageHeader* message = (RobotMessageHeader*)message_buffer;
	if (message)
	{
		SmartLogInfo("Hotel Robot [%s-%d] Stopped", message->id, message->type);
		_robots.RemoveRobot(message->id, (SmartHotelRobotType)message->type);
	}
	
	return result;
}

int SmartHotelServer::HandleMessageRobotAuthorized(LPCSTR ipc_name,
	LPCVOID message_buffer,
	DWORD   message_length,
	LPVOID  answer_buffer,
	DWORD   answer_length)
{
	int result = ERROR_SUCCESS;

	do
	{
		RobotMessageHeader* message = (RobotMessageHeader*)message_buffer;
		if (message)
		{
			SmartLogInfo("Hotel Robot [%s-%d] ", message->id, message->type);
		}
	} while (false);

	return result;
}

void SmartHotelServer::OnHttpHandleStartRobot(const httplib::Request& req, httplib::Response& res)
{
	std::string phone;
	unsigned int type = TYPE_ROBOT_UNKNOWN;
	if (!OnHttpHandleValidateParameters(req, res, phone, &type))
		return;

	int result = StartRobot(phone.c_str(), type);
	if (result == ERROR_SUCCESS)
	{
		std::string response;
		CreateSuccessResponse(response);
		res.set_content(response.c_str(), "application/json");
	}
	else
	{
		std::string response;
		CreateErrorResponse(13, "start robot failed", response);
		res.set_content(response.c_str(), "application/json");
	}
}

void SmartHotelServer::OnHttpHandleStopRobot(const httplib::Request& req, httplib::Response& res)
{
	std::string phone;
	unsigned int type = TYPE_ROBOT_UNKNOWN;
	if (!OnHttpHandleValidateParameters(req, res, phone, &type))
		return;

	int result = StopRobot(phone.c_str(), type);
	if (result == ERROR_SUCCESS)
	{
		std::string response;
		CreateSuccessResponse(response);
		res.set_content(response.c_str(), "application/json");
	}
	else
	{
		std::string response;
		CreateErrorResponse(13, "stop robot failed", response);
		res.set_content(response.c_str(), "application/json");
	}
}

void SmartHotelServer::OnHttpHandleQueryRobotStatus(const httplib::Request& req, httplib::Response& res)
{
	std::string phone;
	unsigned int type = TYPE_ROBOT_UNKNOWN;
	if (!OnHttpHandleValidateParameters(req, res, phone, &type))
		return;

	char* message = (char*)SmartMemAlloc(MESSAGE_SIZE);
	if (message == nullptr)
		return;

	int result = QueryRobotStatus(phone.c_str(), type, message, MESSAGE_SIZE);
	if (result == ERROR_SUCCESS)
	{
		std::string response;
		CreateSuccessResponse(message, response);
		res.set_content(response.c_str(), "application/json");
	}
	else
	{
		std::string response;
		CreateErrorResponse(13, "query robot status failed", response);
		res.set_content(response.c_str(), "application/json");
	}

	if (message)
	{
		SmartMemFree(message);
		message = nullptr;
	}
}

void SmartHotelServer::OnHttpHandleQueryRobotHotels(const httplib::Request& req, httplib::Response& res)
{
	std::string phone;
	unsigned int type = TYPE_ROBOT_UNKNOWN;
	if (!OnHttpHandleValidateParameters(req, res, phone, &type))
		return;

	char* message = (char*)SmartMemAlloc(MESSAGE_SIZE);
	if (message == nullptr)
		return;

	int result = QueryRobotHotels(phone.c_str(), type, message, MESSAGE_SIZE);
	if (result == ERROR_SUCCESS)
	{
		std::string response;
		CreateSuccessResponse(message, response);
		res.set_content(response.c_str(), "application/json");
	}
	else
	{
		std::string response;
		CreateErrorResponse(13, "query robot hotels failed", response);
		res.set_content(response.c_str(), "application/json");
	}

	if (message)
	{
		SmartMemFree(message);
		message = nullptr;
	}
}

void SmartHotelServer::OnHttpHandleAuthorizingRobot(const httplib::Request& req, httplib::Response& res)
{
	std::string phone;
	unsigned int type = TYPE_ROBOT_UNKNOWN;
	if (!OnHttpHandleValidateParameters(req, res, phone, &type))
		return;

	char* message = (char*)SmartMemAlloc(MESSAGE_SIZE);
	if (message == nullptr)
		return;

	int result = AuthorizingRobot(phone.c_str(), type, message, MESSAGE_SIZE);
	if (result == ERROR_SUCCESS)
	{
		std::string response;
		CreateSuccessResponse(message, response);
		res.set_content(response.c_str(), "application/json");
	}
	else
	{
		std::string response;
		CreateErrorResponse(13, "authorize robot start failed", response);
		res.set_content(response.c_str(), "application/json");
	}

	if (message)
	{
		SmartMemFree(message);
		message = nullptr;
	}
}

void SmartHotelServer::OnHttpHandleAuthorizeRobotSms(const httplib::Request& req, httplib::Response& res)
{
	std::string phone;
	unsigned int type = TYPE_ROBOT_UNKNOWN;
	if (!OnHttpHandleValidateParameters(req, res, phone, &type))
		return;

	char* message = (char*)SmartMemAlloc(MESSAGE_SIZE);
	if (message == nullptr)
		return;

	int result = AuthorizeRobotSms(phone.c_str(), type, message, MESSAGE_SIZE);
	if (result == ERROR_SUCCESS)
	{
		std::string response;
		CreateSuccessResponse(message, response);
		res.set_content(response.c_str(), "application/json");
	}
	else
	{
		std::string response;
		CreateErrorResponse(13, "authorize robot sms failed", response);
		res.set_content(response.c_str(), "application/json");
	}

	if (message)
	{
		SmartMemFree(message);
		message = nullptr;
	}
}

bool SmartHotelServer::OnHttpHandleValidateParameters(const httplib::Request& req, httplib::Response& res, std::string& phone, unsigned int* type)
{
	if (!req.has_param("phone"))
	{
		std::string output = R"({"code": 11, "message" : "parameter phone not found"})";
		res.set_content(output.c_str(), "application/json");
		return false;
	}

	if (!req.has_param("type"))
	{
		std::string output = R"({"code": 11, "message" : "parameter type not found"})";
		res.set_content(output.c_str(), "application/json");
		return false;
	}

	std::string param_phone = req.get_param_value("phone");
	if (param_phone.empty())
	{
		std::string output = R"({"code": 12, "message" : "invalid phone parameter"})";
		res.set_content(output.c_str(), "application/json");
		return false;
	}

	std::string param_type = req.get_param_value("type");
	if (param_type.empty())
	{
		std::string output = R"({"code": 12, "message" : "invalid type parameter"})";
		res.set_content(output.c_str(), "application/json");
		return false;
	}

	unsigned int robot_type = StrToIntA(param_type.c_str());
	if (robot_type != TYPE_ROBOT_MEITUAN && robot_type != TYPE_ROBOT_CTRIP)
	{
		std::string output = R"({"code": 12, "message" : "invalid type parameter"})";
		res.set_content(output.c_str(), "application/json");
		return false;
	}

	phone.clear();
	phone.assign(param_phone);

	if (type)
	{
		*type = robot_type;
	}

	return true;
}

void SmartHotelServer::OnHttpHandleError(const httplib::Request& req, httplib::Response& res)
{
	const char* fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
	char buf[BUFSIZ];
	snprintf(buf, sizeof(buf), fmt, res.status);
	res.set_content(buf, "text/html");
}

int SmartHotelServer::StartRobot(const char* id, unsigned int type)
{
	int result = ERROR_SUCCESS;
	char robot_path[MAX_PATH] = { 0 };

	do
	{
		if (!SmartFsGetAppPathA(robot_path, ROBOT_EXECUABLE))
		{
			result = ERROR_FILE_NOT_FOUND;
			break;
		}

		std::string commnad;
		commnad.append(robot_path);
		commnad.append(" ");
		commnad.append("--id").append("=").append(id);
		commnad.append(" ");
		commnad.append("--t").append("=").append(std::to_string(type));

		PROCESS_INFORMATION pi;
		result = SmartProcessManager::CreateNormalProcessA(commnad.c_str(), &pi);
		if (result != ERROR_SUCCESS)
			break;

	} while (false);

	return result;
}

int SmartHotelServer::StopRobot(const char* id, unsigned int type)
{
	RobotMessageHeader message;
	CreateRobotMessageHeader(&message, MESSAGE_ROBOT_STOP, id, type);
	return SendRobotMessage(id, type, &message, sizeof(RobotMessageHeader));
}

int SmartHotelServer::QueryRobotStatus(const char* id, unsigned int type, char* response, unsigned int response_length)
{
	RobotMessageHeader message;
	CreateRobotMessageHeader(&message, MESSAGE_ROBOT_QUERY_STATUS, id, type);
	return SendRobotMessage(id, type, &message, sizeof(RobotMessageHeader), response, response_length);
}

int SmartHotelServer::QueryRobotHotels(const char* id, unsigned int type, char* response, unsigned int response_length)
{
	RobotMessageHeader message;
	CreateRobotMessageHeader(&message, MESSAGE_ROBOT_QUERY_HOTELS, id, type);
	return SendRobotMessage(id, type, &message, sizeof(RobotMessageHeader), response, response_length);
}

int SmartHotelServer::AuthorizingRobot(const char* id, unsigned int type, char* response, unsigned int response_length)
{
	RobotMessageHeader message;
	CreateRobotMessageHeader(&message, MESSAGE_ROBOT_AUTHORIZING, id, type);
	return SendRobotMessage(id, type, &message, sizeof(RobotMessageHeader), response, response_length);
}

int SmartHotelServer::AuthorizeRobotSms(const char* id, unsigned int type, char* response, unsigned int response_length)
{
	RobotMessageHeader message;
	CreateRobotMessageHeader(&message, MESSAGE_ROBOT_AUTHORIZE_SMS, id, type);
	return SendRobotMessage(id, type, &message, sizeof(RobotMessageHeader), response, response_length);
}

int SmartHotelServer::SendRobotMessage(const char* id, unsigned int type, LPCVOID message_buffer, DWORD message_length)
{
	SmartHotelRobot* robot = _robots.GetRobot(id, (SmartHotelRobotType)type);
	if (robot == nullptr)
		return ERROR_OBJECT_NOT_FOUND;

	if (!SendIpcMessage(robot->GetRobotTopic(), message_buffer, message_length))
		return ERROR_DATA_NOT_ACCEPTED;

	return ERROR_SUCCESS;
}

int SmartHotelServer::SendRobotMessage(const char* id, unsigned int type, LPCVOID message_buffer, DWORD message_length, PVOID answer_buffer, DWORD answer_length)
{
	SmartHotelRobot* robot = _robots.GetRobot(id, (SmartHotelRobotType)type);
	if (robot == nullptr)
		return ERROR_OBJECT_NOT_FOUND;

	if (!SendIpcMessage(robot->GetRobotTopic(), message_buffer, message_length, answer_buffer, answer_length))
		return ERROR_DATA_NOT_ACCEPTED;

	return ERROR_SUCCESS;
}

int SmartHotelServer::SendRobotMessage(const char* id, unsigned int type, LPCVOID message_buffer, DWORD message_length, PVOID answer_buffer, DWORD answer_length, DWORD answer_timeout)
{
	SmartHotelRobot* robot = _robots.GetRobot(id, (SmartHotelRobotType)type);
	if (robot == nullptr)
		return ERROR_OBJECT_NOT_FOUND;

	if (!SendIpcMessage(robot->GetRobotTopic(), message_buffer, message_length, answer_buffer, answer_length, answer_timeout))
		return ERROR_DATA_NOT_ACCEPTED;

	return ERROR_SUCCESS;
}

void SmartHotelServer::GetRobotMessageTopic(const char* id, std::string& topic)
{
	topic.clear();
	topic.append(MESSAGE_TOPIC_ROBOT);
	topic.append("-");
	topic.append(id);
}

void SmartHotelServer::CreateErrorResponse(int code, const char* message, std::string& response)
{
	response.clear();
	response.append("{");
	response.append("\"code\"");
	response.append(":");
	response.append(std::to_string(code));
	response.append(",");
	response.append("\"message\"");
	response.append(":");
	response.append("\"");
	response.append(message);
	response.append("\"");
	response.append("}");
}

void SmartHotelServer::CreateSuccessResponse(std::string& response)
{
	response.clear();
	response.append("{");
	response.append("\"code\"");
	response.append(":");
	response.append(std::to_string(0));
	response.append(",");
	response.append("\"message\"");
	response.append(":");
	response.append("\"");
	response.append("success");
	response.append("\"");
	response.append("}");
}

void SmartHotelServer::CreateSuccessResponse(const char* data, std::string& response)
{
	response.clear();
	response.append("{");
	response.append("\"code\"");
	response.append(":");
	response.append(std::to_string(0));
	response.append(",");
	response.append("\"message\"");
	response.append(":");
	response.append("\"");
	response.append("success");
	response.append("\"");
	response.append(",");
	response.append("\"data\"");
	response.append(":");
	response.append("\"");
	response.append(data);
	response.append("\"");
	response.append("}");
}

void WINAPI SmartHotelServer::IpcMessageCallback(LPCSTR ipc_name,
	LPCVOID ipc_context,
	LPCVOID message_buffer,
	DWORD   message_length,
	LPVOID  answer_buffer,
	DWORD   answer_length)
{
	SmartHotelServer* server = (SmartHotelServer*)ipc_context;
	if (server == nullptr)
		return;

	server->HandleMessage(ipc_name, message_buffer, message_length, answer_buffer, answer_length);
}