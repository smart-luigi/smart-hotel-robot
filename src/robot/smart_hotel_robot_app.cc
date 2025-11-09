#include "precomp.h"
#include "smart_hotel_robot_context.h"
#include "smart_hotel_robot_window_handler.h"
#include "smart_hotel_robot_resource_handler.h"
#include "smart_hotel_robot_message_handler.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPWSTR cmdline, int cmdshow)
{
	int result = ERROR_SUCCESS;
	LPWSTR* argv = nullptr;
	CefClientApplication* app = nullptr;
	char* number_a = nullptr;
	SmartHotelRobotContext* context = nullptr;

	do
	{
		if (!SmartLogInitialize())
		{
			result = ERROR_LOG_STATE_INVALID;
			break;
		}
		
		int app_type = CefClientApplicationManager::GetClientAppType();
		if (app_type == CEF_APP_TYPE_BROWSER)
		{
			int argc = 0;
			argv = CommandLineToArgvW(cmdline, &argc);
			if (argv == nullptr)
			{
				result = ERROR_INVALID_PARAMETER;
				break;
			}

			context = new SmartHotelRobotContext;
			if (context == nullptr)
			{
				result = ERROR_OBJECT_NOT_FOUND;
				break;
			}

			result = context->Init(argc, argv);
			if (result != ERROR_SUCCESS)
				break;
		}

		CefClientApplicationConfig config;
		RtlZeroMemory(&config, sizeof(CefClientApplicationConfig));
		config.instance = instance;
		config.window_icon_normal = IDI_ICON_NORMAL;
		config.window_icon_small = IDI_ICON_SMALL;
		config.width = 476;
		config.height = 800;
		config.x = (GetSystemMetrics(SM_CXSCREEN) - config.width) / 2;
		config.y = (GetSystemMetrics(SM_CYSCREEN) - config.height) / 2;
		config.always_on_top = false;
		config.with_captionbar = true;
		config.with_maximizebox = false;
		config.enable_singleton = false;
		config.with_osr = false;
		config.with_dev = true;
		if (context && context->ValidateCacheEnviroment())
		{
			lstrcpyA(config.window_title, context->GetWindowTitle());
			lstrcpyA(config.singleton_name, context->GetSingletonName());
			lstrcpyA(config.url, context->GetListUrl());
			lstrcpyA(config.root_cache_path, context->GetCacheEnviromentPath());
		}
		
		app = CefClientApplicationManager::CreateClientApp(instance, &config);
		if (app == nullptr)
		{
			result = ERROR_NOT_ENOUGH_MEMORY;
			break;
		}

		if (app_type == CEF_APP_TYPE_BROWSER)
		{
			if (context)
			{
				context->SetApplication(app);
			}
		}
		
		app->SetWindowMessageHandler(new SmartHotelRobotWindowHandler(context));
		app->SetBrowserResourceHandler(new SmartHotelRobotResourceHandler(context));
		app->AddBrowserMessageHandler(new SmartHotelRobotMessageHandler(context));

		result = app->Execute();

	} while (false);

	if (context)
	{
		context->Dispose();
		context = nullptr;
	}

	if (app)
	{
		app->Dispose();
		app = nullptr;
	}

	if (argv)
	{
		LocalFree(argv);
		argv = nullptr;
	}

	SmartLogShutdown();

#ifdef _DEBUG
	assert(SmartMemCheck());
#endif // _DEBUG

	return result;
}