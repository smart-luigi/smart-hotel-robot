#include <windows.h>
#include <shellapi.h>
#include <smart_base.h>
#include "smart_hotel_server.h"

int main(int argc, char* argv[])
{
	int result = ERROR_SUCCESS;
	SmartHotelServer* server = nullptr;

	do
	{
		if (!SmartLogInitialize())
		{
			result = ERROR_LOG_STATE_INVALID;
			break;
		}

		SmartLogInfo("Startup Smart Hotel Server....");

		server = new SmartHotelServer;
		if (server == nullptr)
		{
			result = ERROR_SERVER_SHUTDOWN_IN_PROGRESS;
			break;
		}

		result = server->Init();
		if (result != ERROR_SUCCESS)
			break;

		result = server->Run();

	} while (false);

	if (server)
	{
		server->Dispose();
		server = nullptr;
	}

	SmartLogShutdown();

#ifdef _DEBUG
	assert(SmartMemCheck());
#endif // _DEBUG

	return result;
}