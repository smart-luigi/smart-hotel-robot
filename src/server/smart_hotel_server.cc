#include <boost/smart_ptr.hpp>
#include <boost/property_tree/json_parser.hpp>
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

		if (!SmartFsGetAppPathA(server_root, nullptr))
		{
			result = ERROR_PATH_NOT_FOUND;
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

		_http_server->Get("/smart-hotel/api/robot-start",
			[=](const httplib::Request& req, httplib::Response& res)
			{
				this->OnHttpHandleRobotStart(req, res);
			}
		);

		_http_server->Get("/smart-hotel/api/robot-stop",
			[=](const httplib::Request& req, httplib::Response& res)
			{
				this->OnHttpHandleRobotStop(req, res);
			}
		);

		_http_server->Get("/smart-hotel/api/robot-authorize-account-password",
			[=](const httplib::Request& req, httplib::Response& res)
			{
				this->OnHttpHandleRobotAuthorizeAccountPassword(req, res);
			}
		);

		_http_server->Get("/smart-hotel/api/robot-authorize-account",
			[=](const httplib::Request& req, httplib::Response& res)
			{
				this->OnHttpHandleRobotAuthorizeAccount(req, res);
			}
		);

		_http_server->Get("/smart-hotel/api/robot-authorize-code",
			[=](const httplib::Request& req, httplib::Response& res)
			{
				this->OnHttpHandleRobotAuthorizeCode(req, res);
			}
		);

		_http_server->Get("/smart-hotel/api/robot-query-account",
			[=](const httplib::Request& req, httplib::Response& res)
			{
				this->OnHttpHandleRobotQueryAccount(req, res);
			}
		);

		_http_server->Get("/smart-hotel/api/robot-query-status",
			[=](const httplib::Request& req, httplib::Response& res)
			{
				this->OnHttpHandleRobotQueryStatus(req, res);
			}
		);

		_http_server->Get("/smart-hotel/api/robot-query-hotels",
			[=](const httplib::Request& req, httplib::Response& res)
			{
				this->OnHttpHandleRobotQueryHotels(req, res);
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

	MessageRobotHeader* header = (MessageRobotHeader*)message_buffer;
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
		break;
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

	MessageRobotHeader* message = (MessageRobotHeader*)message_buffer;
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

	MessageRobotHeader* message = (MessageRobotHeader*)message_buffer;
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
		MessageRobotHeader* message = (MessageRobotHeader*)message_buffer;
		if (message)
		{
		}
	} while (false);

	return result;
}

void SmartHotelServer::OnHttpHandleRobotStart(const httplib::Request& req, httplib::Response& res)
{
	std::string response;

	do
	{
		std::string phone;
		unsigned int type = TYPE_ROBOT_UNKNOWN;
		if (!OnHttpHandleValidateParameters(req, res, phone, &type))
			break;

		int result = HandleRobotStart(phone.c_str(), type);
		if (result == ERROR_SUCCESS)
		{
			CreateSuccessResponse(res);
		}
		else
		{
			CreateErrorResponse(13, "robot start failed", res);
		}
	} while (false);
}

void SmartHotelServer::OnHttpHandleRobotStop(const httplib::Request& req, httplib::Response& res)
{
	std::string phone;
	unsigned int type = TYPE_ROBOT_UNKNOWN;
	if (!OnHttpHandleValidateParameters(req, res, phone, &type))
		return;

	int result = HandleRobotStop(phone.c_str(), type);
	if (result == ERROR_SUCCESS)
	{
		CreateSuccessResponse(res);
	}
	else
	{
		CreateErrorResponse(13, "robot stop failed", res);
	}
}

void SmartHotelServer::OnHttpHandleRobotAuthorizeAccountPassword(const httplib::Request& req, httplib::Response& res)
{
	std::string phone;
	unsigned int type = TYPE_ROBOT_UNKNOWN;
	if (!OnHttpHandleValidateParameters(req, res, phone, &type))
		return;

	if (!req.has_param("p"))
	{
		CreateErrorResponse(11, "parameter password not found", res);
		return;
	}

	std::string password = req.get_param_value("p");
	if (password.empty())
	{
		CreateErrorResponse(12, "invalid password parameter", res);
		return;
	}

	char* message = (char*)SmartMemAlloc(MESSAGE_SIZE);
	if (message == nullptr)
		return;

	int result = HandleRobotAuthorizeAccountPassword(phone.c_str(), password.c_str(), type, message, MESSAGE_SIZE);
	if (result == ERROR_SUCCESS)
	{
		CreateSuccessResponse(message, false, res);
	}
	else
	{
		CreateErrorResponse(13, "robot authorize account failed", res);
	}

	if (message)
	{
		SmartMemFree(message);
		message = nullptr;
	}
}

void SmartHotelServer::OnHttpHandleRobotAuthorizeAccount(const httplib::Request& req, httplib::Response& res)
{
	std::string phone;
	unsigned int type = TYPE_ROBOT_UNKNOWN;
	if (!OnHttpHandleValidateParameters(req, res, phone, &type))
		return;

	char* message = (char*)SmartMemAlloc(MESSAGE_SIZE);
	if (message == nullptr)
		return;

	int result = HandleRobotAuthorizeAccount(phone.c_str(), type, message, MESSAGE_SIZE);
	if (result == ERROR_SUCCESS)
	{
		CreateSuccessResponse(message, false, res);
	}
	else
	{
		CreateErrorResponse(13, "robot authorize code start failed", res);
	}

	if (message)
	{
		SmartMemFree(message);
		message = nullptr;
	}
}

void SmartHotelServer::OnHttpHandleRobotAuthorizeCode(const httplib::Request& req, httplib::Response& res)
{
	std::string phone;
	unsigned int type = TYPE_ROBOT_UNKNOWN;
	if (!OnHttpHandleValidateParameters(req, res, phone, &type))
		return;

	if (!req.has_param("code"))
	{
		CreateErrorResponse(11, "parameter code not found", res);
		return;
	}

	std::string code = req.get_param_value("code");
	if (code.empty())
	{
		CreateErrorResponse(12, "invalid code parameter", res);
		return;
	}

	char* message = (char*)SmartMemAlloc(MESSAGE_SIZE);
	if (message == nullptr)
		return;

	int result = HandleRobotAuthorizeCode(phone.c_str(), code.c_str(), type, message, MESSAGE_SIZE);
	if (result == ERROR_SUCCESS)
	{
		CreateSuccessResponse(message, false, res);
	}
	else
	{
		CreateErrorResponse(13, "robot authorize code failed", res);
	}

	if (message)
	{
		SmartMemFree(message);
		message = nullptr;
	}
}

void SmartHotelServer::OnHttpHandleRobotQueryAccount(const httplib::Request& req, httplib::Response& res)
{
	std::string phone;
	unsigned int type = TYPE_ROBOT_UNKNOWN;
	if (!OnHttpHandleValidateParameters(req, res, phone, &type))
		return;

	char* message = (char*)SmartMemAlloc(MESSAGE_SIZE);
	if (message == nullptr)
		return;

	int result = HandleRobotQueryAccount(phone.c_str(), type, message, MESSAGE_SIZE);
	if (result == ERROR_SUCCESS)
	{
		CreateSuccessResponse(message, false, res);
	}
	else
	{
		CreateErrorResponse(13, "robot query account failed", res);
	}

	if (message)
	{
		SmartMemFree(message);
		message = nullptr;
	}
}

void SmartHotelServer::OnHttpHandleRobotQueryStatus(const httplib::Request& req, httplib::Response& res)
{
	std::string phone;
	unsigned int type = TYPE_ROBOT_UNKNOWN;
	if (!OnHttpHandleValidateParameters(req, res, phone, &type))
		return;

	char* message = (char*)SmartMemAlloc(MESSAGE_SIZE);
	if (message == nullptr)
		return;

	int result = HandleRobotQueryStatus(phone.c_str(), type, message, MESSAGE_SIZE);
	if (result == ERROR_SUCCESS)
	{
		MessageRobotStatusPtr status_message = (MessageRobotStatusPtr)message;
		if (status_message == nullptr)
		{
			CreateErrorResponse(13, "robot query status failed", res);
		}
		else
		{
			CreateSuccessResponse(std::to_string(status_message->authorized).c_str(), false, res);
		}
	}
	else
	{
		CreateErrorResponse(13, "robot query status failed", res);
	}

	if (message)
	{
		SmartMemFree(message);
		message = nullptr;
	}
}

void SmartHotelServer::OnHttpHandleRobotQueryHotels(const httplib::Request& req, httplib::Response& res)
{
	std::string phone;
	unsigned int type = TYPE_ROBOT_UNKNOWN;
	if (!OnHttpHandleValidateParameters(req, res, phone, &type))
		return;

	char* message = (char*)SmartMemAlloc(MESSAGE_SIZE);
	if (message == nullptr)
		return;

	int result = HandleRobotQueryHotels(phone.c_str(), type, message, MESSAGE_SIZE);
	if (result == ERROR_SUCCESS)
	{
		CreateSuccessResponse(message, true, res);
	}
	else
	{
		CreateErrorResponse(13, "query robot hotels failed", res);
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
		CreateErrorResponse(11, "parameter phone not found", res);
		return false;
	}

	if (!req.has_param("type"))
	{
		CreateErrorResponse(11, "parameter type not found", res);
		return false;
	}

	std::string param_phone = req.get_param_value("phone");
	if (param_phone.empty())
	{
		CreateErrorResponse(12, "invalid phone parameter", res);
		return false;
	}

	std::string param_type = req.get_param_value("type");
	if (param_type.empty())
	{
		CreateErrorResponse(12, "invalid type parameter", res);
		return false;
	}

	unsigned int robot_type = StrToIntA(param_type.c_str());
	if (robot_type != TYPE_ROBOT_MEITUAN && robot_type != TYPE_ROBOT_CTRIP)
	{
		CreateErrorResponse(12, "invalid type parameter", res);
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

int SmartHotelServer::HandleRobotStart(const char* id, unsigned int type)
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

int SmartHotelServer::HandleRobotStop(const char* id, unsigned int type)
{
	MessageRobotHeader message;
	CreateMessageRobotHeader(&message, MESSAGE_ROBOT_STOP, id, type);
	return SendRobotMessage(id, type, &message, sizeof(MessageRobotHeader));
}

int SmartHotelServer::HandleRobotAuthorizeAccountPassword(const char* id, const char* password, unsigned int type, char* response, unsigned int response_length)
{
	MessageRobotAuthorizePassword message;
	CreateMessageRobotHeader(&message.header, MESSAGE_ROBOT_AUTHORIZE_ACCOUNT_PASSWORD, id, type);
	lstrcpyA((char*)message.password, password);
	return SendRobotMessage(id, type, &message, sizeof(MessageRobotAuthorizePassword), response, response_length);
}

int SmartHotelServer::HandleRobotAuthorizeAccount(const char* id, unsigned int type, char* response, unsigned int response_length)
{
	MessageRobotAuthorizeAccount message;
	CreateMessageRobotHeader(&message, MESSAGE_ROBOT_AUTHORIZE_ACCOUNT, id, type);
	return SendRobotMessage(id, type, &message, sizeof(MessageRobotAuthorizeAccount), response, response_length);
}

int SmartHotelServer::HandleRobotAuthorizeCode(const char* id, const char* code, unsigned int type, char* response, unsigned int response_length)
{
	MessageRobotAuthorizeCode message;
	CreateMessageRobotHeader(&message.header, MESSAGE_ROBOT_AUTHORIZE_CODE, id, type);
	lstrcpyA((char*)message.code, code);
	return SendRobotMessage(id, type, &message, sizeof(MessageRobotAuthorizeCode), response, response_length);
}

int SmartHotelServer::HandleRobotQueryStatus(const char* id, unsigned int type, char* response, unsigned int response_length)
{
	MessageRobotHeader message;
	CreateMessageRobotHeader(&message, MESSAGE_ROBOT_QUERY_STATUS, id, type);
	return SendRobotMessage(id, type, &message, sizeof(MessageRobotHeader), response, response_length);
}

int SmartHotelServer::HandleRobotQueryAccount(const char* id, unsigned int type, char* response, unsigned int response_length)
{
	MessageRobotHeader message;
	CreateMessageRobotHeader(&message, MESSAGE_ROBOT_QUERY_ACCOUNT, id, type);
	return SendRobotMessage(id, type, &message, sizeof(MessageRobotHeader), response, response_length);
}

int SmartHotelServer::HandleRobotQueryHotels(const char* id, unsigned int type, char* response, unsigned int response_length)
{
	MessageRobotHeader message;
	CreateMessageRobotHeader(&message, MESSAGE_ROBOT_QUERY_HOTELS, id, type);
	return SendRobotMessage(id, type, &message, sizeof(MessageRobotHeader), response, response_length);
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

void SmartHotelServer::CreateErrorResponse(int code, const char* message, httplib::Response& res)
{
	boost::property_tree::ptree response;
	response.put<int>("code", code);
	response.put<std::string>("message", message);

	std::ostringstream oss;
	boost::property_tree::write_json(oss, response);

	res.set_content(oss.str().c_str(), "application/json");
}

void SmartHotelServer::CreateSuccessResponse(httplib::Response& res)
{
	boost::property_tree::ptree response;
	response.put<int>("code", ERROR_SUCCESS);
	response.put<std::string>("message", "success");

	std::ostringstream oss;
	boost::property_tree::write_json(oss, response);

	res.set_content(oss.str().c_str(), "application/json");
}

void SmartHotelServer::CreateSuccessResponse(const char* data, bool array, httplib::Response& res)
{
	boost::property_tree::ptree response;
	response.put<int>("code", ERROR_SUCCESS);
	response.put<std::string>("message", "success");
	if (array)
	{
		response.put<std::string>("data", data);
	}
	else
	{
		response.put<std::string>("data", data);
	}
	
	std::ostringstream oss;
	boost::property_tree::write_json(oss, response);

	res.set_content(oss.str().c_str(), "application/json");
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