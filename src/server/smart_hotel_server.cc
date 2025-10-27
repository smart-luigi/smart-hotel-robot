#include <boost/smart_ptr.hpp>
#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <smart_base.h>
#include "message.h"
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
			[](const httplib::Request& req, httplib::Response& res)
			{
				const char* fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
				char buf[BUFSIZ];
				snprintf(buf, sizeof(buf), fmt, res.status);
				res.set_content(buf, "text/html");
			}
		);

		_http_server->Get("/smart-hotel/api/start-robot",
			[=](const httplib::Request& req, httplib::Response& res)
			{
				if (!req.has_param("phone"))
				{
					std::string output = R"({"code": 11, "message" : "parameter phone not found"})";
					res.set_content(output.c_str(), "application/json");
					return;
				}

				if (!req.has_param("type"))
				{
					std::string output = R"({"code": 11, "message" : "parameter type not found"})";
					res.set_content(output.c_str(), "application/json");
					return;
				}

				std::string phone = req.get_param_value("phone");
				if (phone.empty())
				{
					std::string output = R"({"code": 12, "message" : "invalid phone parameter"})";
					res.set_content(output.c_str(), "application/json");
					return;
				}

				std::string type = req.get_param_value("type");
				if (phone.empty())
				{
					std::string output = R"({"code": 12, "message" : "invalid type parameter"})";
					res.set_content(output.c_str(), "application/json");
					return;
				}

				unsigned int robot_type = StrToIntA(type.c_str());
				if (robot_type != TYPE_ROBOT_MEITUAN && robot_type != TYPE_ROBOT_CTRIP)
				{
					std::string output = R"({"code": 12, "message" : "invalid type parameter"})";
					res.set_content(output.c_str(), "application/json");
					return;
				}

				int result = StartRobot(phone.c_str(), robot_type);
				if (result == ERROR_SUCCESS)
				{
					std::string output = R"({"code" : 0, "message": "success"})";
					res.set_content(output.c_str(), "application/json");
				}
				else
				{
					std::string output = R"({"code" : 13, "message": "start robot failed"})";
					res.set_content(output.c_str(), "application/json");
				}
			}
		);

		_http_server->Get("/smart-hotel/api/stop-robot",
			[=](const httplib::Request& req, httplib::Response& res)
			{
				if (!req.has_param("phone"))
				{
					std::string output = R"({"code": 11, "message" : "parameter phone not found"})";
					res.set_content(output.c_str(), "application/json");
					return;
				}

				std::string phone = req.get_param_value("phone");
				if (phone.empty())
				{
					std::string output = R"({"code": 12, "message" : "invalid phone parameter"})";
					res.set_content(output.c_str(), "application/json");
					return;
				}

				int result = StopRobot(phone.c_str());
				if (result == ERROR_SUCCESS)
				{
					std::string output = R"({"code" : 0, "message": "success"})";
					res.set_content(output.c_str(), "application/json");
				}
				else
				{
					std::string output = R"({"code" : 13, "message": "start robot failed"})";
					res.set_content(output.c_str(), "application/json");
				}
			}
		);

		_http_server->Get("/smart-hotel/api/query-robot",
			[=](const httplib::Request& req, httplib::Response& res)
			{
				if (!req.has_param("phone"))
				{
					std::string output = R"({"code": 11, "message" : "parameter phone not found"})";
					res.set_content(output.c_str(), "application/json");
					return;
				}

				std::string phone = req.get_param_value("phone");
				if (phone.empty())
				{
					std::string output = R"({"code": 12, "message" : "invalid phone parameter"})";
					res.set_content(output.c_str(), "application/json");
					return;
				}

				int result = StopRobot(phone.c_str());
				if (result == ERROR_SUCCESS)
				{
					std::string output = R"({"code" : 0, "message": "success"})";
					res.set_content(output.c_str(), "application/json");
				}
				else
				{
					std::string output = R"({"code" : 13, "message": "start robot failed"})";
					res.set_content(output.c_str(), "application/json");
				}
			}
		);

		_http_server->Get("/smart-hotel/api/query-hotels",
			[=](const httplib::Request& req, httplib::Response& res)
			{
				if (!req.has_param("phone"))
				{
					std::string output = R"({"code": 11, "message" : "parameter phone not found"})";
					res.set_content(output.c_str(), "application/json");
					return;
				}

				std::string phone = req.get_param_value("phone");
				if (phone.empty())
				{
					std::string output = R"({"code": 12, "message" : "invalid phone parameter"})";
					res.set_content(output.c_str(), "application/json");
					return;
				}

				std::string output = R"({"code" : 0, "message": "success"})";
				res.set_content(output.c_str(), "application/json");
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
	case MESSAGE_ROBOT_NEED_SMS_AUTH:
		result = HandleMessageRobotNeedSmsAuth(ipc_name, message_buffer, message_length, answer_buffer, answer_length);
		break;
	case MESSAGE_ROBOT_HOTEL_LIST_READY:
		result = HandleMessageRobotHotelListReady(ipc_name, message_buffer, message_length, answer_buffer, answer_length);
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
		AddRobotMessageClient(message->id);
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
		RemoveRobotMessageClient(message->id);
	}
	
	return result;
}

int SmartHotelServer::HandleMessageRobotNeedSmsAuth(LPCSTR ipc_name,
	LPCVOID message_buffer,
	DWORD   message_length,
	LPVOID  answer_buffer,
	DWORD   answer_length)
{
	int result = ERROR_SUCCESS;

	return result;
}

int SmartHotelServer::HandleMessageRobotHotelListReady(LPCSTR ipc_name,
	LPCVOID message_buffer,
	DWORD   message_length,
	LPVOID  answer_buffer,
	DWORD   answer_length)
{
	int result = ERROR_SUCCESS;


	return result;
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

int SmartHotelServer::StopRobot(const char* id)
{
	int result = ERROR_SUCCESS;

	do
	{
		SmartMessageClient* message_client = GetRobotMessageClient(id);
		if (message_client == nullptr)
		{
			result = ERROR_OBJECT_NOT_FOUND;
			break;
		}

		int id_length = lstrlenA(id) + 1;
		result = message_client->SendMessage(MESSAGE_ROBOT_STOP, id, id_length);

	} while (false);
	
	return result;
}

int SmartHotelServer::QueryRobot(const char* id)
{
	int result = ERROR_SUCCESS;

	do
	{
		SmartMessageClient* message_client = GetRobotMessageClient(id);
		if (message_client == nullptr)
		{
			result = ERROR_OBJECT_NOT_FOUND;
			break;
		}

		int id_length = lstrlenA(id) + 1;
		result = message_client->PostMessage(MESSAGE_ROBOT_QUERY, id, id_length);

	} while (false);

	return result;
}

int SmartHotelServer::AddRobotMessageClient(const char* id)
{
	int result = ERROR_SUCCESS;
	SmartMessageClient* message_client = nullptr;

	do
	{
		message_client = SmartMessageFactory::CreateMessageClient();
		if (message_client == nullptr)
		{
			result = ERROR_SERVER_SHUTDOWN_IN_PROGRESS;
			break;
		}

		std::string topic_robot_name;
		topic_robot_name.append(MESSAGE_TOPIC_ROBOT);
		topic_robot_name.append("-");
		topic_robot_name.append(id);
		SmartMessageTopic* topic_robot = SmartMessageFactory::CreateMessageTopic(topic_robot_name.c_str(), MESSAGE_RANGE_BEGIN, MESSAGE_RANGE_END);
		if (topic_robot == nullptr)
		{
			result = ERROR_DATA_NOT_ACCEPTED;
			break;
		}

		message_client->Configure(nullptr, topic_robot, MESSAGE_SIZE);

		result = message_client->Init();
		if (result != ERROR_SUCCESS)
			break;

		std::lock_guard<std::mutex> lock(_robot_message_clients_lock);
		_robot_message_clients.insert(std::pair<std::string, SmartMessageClient*>(id, message_client));

	} while (false);

	if (result != ERROR_SUCCESS)
	{
		if (message_client)
		{
			message_client->Dispose();
			message_client = nullptr;
		}
	}

	return result;
}

void SmartHotelServer::RemoveRobotMessageClient(const char* id)
{
	std::map<std::string, SmartMessageClient*>::iterator it = _robot_message_clients.find(id);
	if (it == _robot_message_clients.end())
		return;

	SmartMessageClient* message_client = it->second;
	if (message_client == nullptr)
	{
		_robot_message_clients.erase(it);
		return;
	}

	if (message_client)
	{
		message_client->Dispose();
		message_client = nullptr;
	}

	_robot_message_clients.erase(it);
}

SmartMessageClient* SmartHotelServer::GetRobotMessageClient(const char* id)
{
	std::lock_guard<std::mutex> lock(_robot_message_clients_lock);

	std::map<std::string, SmartMessageClient*>::iterator it = _robot_message_clients.find(id);
	if (it == _robot_message_clients.end())
		return nullptr;

	return it->second;
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