#include "precomp.h"
#include "meituan/smart_hotel_robot_meituan.h"
#include "ctrip/smart_hotel_robot_ctrip.h"
#include "smart_hotel_robot_context.h"

SmartHotelRobotContext::SmartHotelRobotContext()
	: _threadpool(nullptr)
	, _window_title("ÖÇÄÜ¾Æµê»úÆ÷ÈË")
	, _singleton_name("Global\\{D23FADD1-BA72-4363-AE32-ADF0AA09FC67}")
	, _cache_env_type(TYPE_ROBOT_UNKNOWN)
	, _hotel_robot(nullptr)
{

}

SmartHotelRobotContext::~SmartHotelRobotContext()
{
	if (_hotel_robot)
	{
		_hotel_robot->Dispose();
		_hotel_robot = nullptr;
	}

	DestroyIpcQueue(_topic_robot_name.c_str());

	if (_threadpool)
	{
		_threadpool->Dispose();
		_threadpool = nullptr;
	}
}

int SmartHotelRobotContext::Init(int argc, wchar_t* argv[])
{
	int result = ERROR_SUCCESS;

	do
	{
		_threadpool = SmartThreadPoolFactory::CreateThreadPool();
		if (_threadpool == nullptr)
		{
			result = ERROR_OBJECT_NOT_FOUND;
			break;
		}

		result = _threadpool->Init();
		if (result != ERROR_SUCCESS)
			break;

		LPCTSTR cache_env_id = SmartSysGetCommandOption(argc, argv, L"--id");
		if (cache_env_id == nullptr)
		{
			result = ERROR_PARAMETER_QUOTA_EXCEEDED;
			break;
		}

		LPCTSTR cache_env_type = SmartSysGetCommandOption(argc, argv, L"--t");
		if (cache_env_type == nullptr)
		{
			result = ERROR_PARAMETER_QUOTA_EXCEEDED;
			break;
		}

		result = InitCacheEnviroment(argc, argv, cache_env_id, cache_env_type);
		if (result != ERROR_SUCCESS)
			break;

		_topic_robot_name.clear();
		_topic_robot_name.append(MESSAGE_TOPIC_ROBOT);
		_topic_robot_name.append("-");
		_topic_robot_name.append(_cache_env_id.c_str());
		_topic_robot_name.append("-");
		_topic_robot_name.append(std::to_string(_cache_env_type));
		if (!CreateIpcQueue(_topic_robot_name.c_str(), this, IpcMessageCallback))
		{
			result = ERROR_CALLBACK_SUPPLIED_INVALID_DATA;
			break;
		}

		result = InitHotelRobot();
		if (result != ERROR_SUCCESS)
			break;

	} while (false);

	return result;
}

void SmartHotelRobotContext::Dispose()
{
	delete this;
}

const char* SmartHotelRobotContext::GetWindowTitle()
{
	return _window_title.c_str();
}

const char* SmartHotelRobotContext::GetSingletonName()
{
	return _singleton_name.c_str();
}

const char* SmartHotelRobotContext::GetLoginUrl()
{
	return _hotel_robot->GetLoginUrl();
}

const char* SmartHotelRobotContext::GetListUrl()
{
	return _hotel_robot->GetListUrl();
}

const char* SmartHotelRobotContext::GetDataUrl()
{
	return _hotel_robot->GetDataUrl();
}

const char* SmartHotelRobotContext::GetCacheEnviromentId()
{
	return _cache_env_id.c_str();
}

const char* SmartHotelRobotContext::GetCacheEnviromentPath()
{
	return _cache_env_path.c_str();
}

SmartHotelRobotType SmartHotelRobotContext::GetCacheEnviromentType()
{
	return _cache_env_type;
}

bool SmartHotelRobotContext::ValidateCacheEnviroment()
{
	if (_cache_env_id.empty())
		return false;

	if (_cache_env_path.empty())
		return false;

	if(_cache_env_type == TYPE_ROBOT_UNKNOWN)
		return false;

	return true;
}

SmartHotelRobot* SmartHotelRobotContext::GetHotelRobot()
{
	return _hotel_robot;
}

CefClientApplication* SmartHotelRobotContext::GetApplication()
{
	return _application;
}

void SmartHotelRobotContext::SetApplication(CefClientApplication* application)
{
	_application = application;
}

int SmartHotelRobotContext::SendServerMessage(LPCVOID message_buffer, DWORD message_length)
{
	if (!SendIpcMessage(MESSAGE_TOPIC_SERVER, message_buffer, message_length))
		return ERROR_DATA_NOT_ACCEPTED;

	return ERROR_SUCCESS;
}

int SmartHotelRobotContext::SendServerMessage(LPCVOID message_buffer, DWORD message_length, PVOID answer_buffer, DWORD answer_length)
{
	if (!SendIpcMessage(MESSAGE_TOPIC_SERVER, message_buffer, message_length, answer_buffer, answer_length))
		return ERROR_DATA_NOT_ACCEPTED;

	return ERROR_SUCCESS;
}

int SmartHotelRobotContext::SendServerMessage(LPCVOID message_buffer, DWORD message_length, PVOID answer_buffer, DWORD answer_length, DWORD answer_timeout)
{
	if (!SendIpcMessage(MESSAGE_TOPIC_SERVER, message_buffer, message_length, answer_buffer, answer_length, answer_timeout))
		return ERROR_DATA_NOT_ACCEPTED;

	return ERROR_SUCCESS;
}

int SmartHotelRobotContext::SendServerMessageRobotStarted()
{
	RobotMessageHeader message;
	CreateRobotMessageHeader(&message, MESSAGE_ROBOT_STARTED, _cache_env_id.c_str(), _cache_env_type);
	return SendServerMessage(&message, sizeof(RobotMessageHeader));
}

int SmartHotelRobotContext::SendServerMessageRobotStopped()
{
	RobotMessageHeader message;
	CreateRobotMessageHeader(&message, MESSAGE_ROBOT_STOPPED, _cache_env_id.c_str(), _cache_env_type);
	return SendServerMessage(&message, sizeof(RobotMessageHeader));
}

int SmartHotelRobotContext::SendServerMessageRobotAuthorizeCompleted(char* sms, DWORD sms_length)
{
	return ERROR_SUCCESS;
}

int SmartHotelRobotContext::InitCacheEnviroment(int argc, wchar_t** argv, const wchar_t* cache_env_id, const wchar_t* cache_env_type)
{
	int result = ERROR_SUCCESS;
	char* cache_env_id_a = nullptr;
	TCHAR* cache_env_path = nullptr;
	wchar_t* cache_env_path_w = nullptr;
	char* cache_env_path_a = nullptr;

	do
	{
		if (argc == 0)
		{
			result = ERROR_PARAMETER_QUOTA_EXCEEDED;
			break;
		}

		int cache_env_id_length = lstrlen(cache_env_id);
		cache_env_id_a = (char*)SmartMemAlloc(cache_env_id_length);
		if (cache_env_id_a == nullptr)
		{
			result = ERROR_NOT_ENOUGH_MEMORY;
			break;
		}

		int cache_env_id_a_length = 0;
		if (!SmartStrW2A(cache_env_id_a, &cache_env_id_a_length, cache_env_id))
		{
			result = ERROR_BUFFER_OVERFLOW;
			break;
		}

		cache_env_path = (TCHAR*)SmartMemAlloc(MAX_PATH * sizeof(TCHAR));
		if (cache_env_path == nullptr)
		{
			result = ERROR_NOT_ENOUGH_MEMORY;
			break;
		}

		cache_env_path_w = (TCHAR*)SmartMemAlloc(MAX_PATH * sizeof(wchar_t));
		if (cache_env_path_w == nullptr)
		{
			result = ERROR_NOT_ENOUGH_MEMORY;
			break;
		}

		if (!SmartFsGetAppPath(cache_env_path, TEXT("caches")))
		{
			result = ERROR_PATH_NOT_FOUND;
			break;
		}

		PathCombine(cache_env_path_w, cache_env_path, cache_env_id);

		int cache_env_path_w_length = lstrlen(cache_env_path_w);
		cache_env_path_a = (char*)SmartMemAlloc(cache_env_path_w_length + 1);
		if (cache_env_path_a == nullptr)
		{
			result = ERROR_NOT_ENOUGH_MEMORY;
			break;
		}

		int cache_env_path_a_length = 0;
		if (!SmartStrW2A(cache_env_path_a, &cache_env_path_a_length, cache_env_path_w))
		{
			result = ERROR_BUFFER_OVERFLOW;
			break;
		}

		_cache_env_id.assign(cache_env_id_a);

		_cache_env_path.assign(cache_env_path_a);

		_cache_env_type = (SmartHotelRobotType)StrToIntW(cache_env_type);

	} while (false);

	if (cache_env_path_a)
	{
		SmartMemFree(cache_env_path_a);
		cache_env_path_a = nullptr;
	}

	if (cache_env_path_w)
	{
		SmartMemFree(cache_env_path_w);
		cache_env_path_w = nullptr;
	}

	if (cache_env_path)
	{
		SmartMemFree(cache_env_path);
		cache_env_path = nullptr;
	}

	if (cache_env_id_a)
	{
		SmartMemFree(cache_env_id_a);
		cache_env_id_a = nullptr;
	}

	return result;
}

int SmartHotelRobotContext::InitHotelRobot()
{
	int result = ERROR_SUCCESS;

	do
	{
		if (_cache_env_type == TYPE_ROBOT_UNKNOWN)
		{
			result = ERROR_DATA_NOT_ACCEPTED;
			break;
		}

		switch (_cache_env_type)
		{
		case TYPE_ROBOT_MEITUAN:
			_hotel_robot = new SmartHotelRobotMeituan(this);
			break;
		case TYPE_ROBOT_CTRIP:
			_hotel_robot = new SmartHotelRobotCtrip(this);
			break;
		}

		result = _hotel_robot->Init();

	} while (false);

	return result;
}

int SmartHotelRobotContext::HandleMessage(LPCSTR ipc_name,
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
	case MESSAGE_ROBOT_STOP:
		if (_application)
		{
			PostMessage(_application->GetRootWindowHandle(), WM_DESTROY, 0, 0);
		}
		break;
	case MESSAGE_ROBOT_AUTHORIZING:
		_hotel_robot->StartAuthorizing(message_buffer, message_length, answer_buffer, answer_length);
		break;
	case MESSAGE_ROBOT_AUTHORIZE_SMS:
		_hotel_robot->StartAuthorizeSms(message_buffer, message_length, answer_buffer, answer_length);
		break;
	case MESSAGE_ROBOT_QUERY_STATUS:
		_hotel_robot->QueryStatus(message_buffer, message_length, answer_buffer, answer_length);
		break;
	case MESSAGE_ROBOT_QUERY_HOTELS:
		_hotel_robot->QueryHotels(message_buffer, message_length, answer_buffer, answer_length);
		break;
	default:
		break;
	}

	return result;
}

void WINAPI SmartHotelRobotContext::IpcMessageCallback(LPCSTR ipc_name,
	LPCVOID ipc_context,
	LPCVOID message_buffer,
	DWORD   message_length,
	LPVOID  answer_buffer,
	DWORD   answer_length)
{
	SmartHotelRobotContext* context = (SmartHotelRobotContext*)ipc_context;
	if (context == nullptr)
		return;

	context->HandleMessage(ipc_name, message_buffer, message_length, answer_buffer, answer_length);
}