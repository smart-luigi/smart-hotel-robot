#include "framework.h"
#include "resource.h"
#include <smart_base.h>
#include <smart_cef_client.h>
#include "default_cef_resource_request_handler.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPWSTR cmdline, int cmdshow)
{
	int result = ERROR_SUCCESS;

	do
	{
		if (!SmartLogInitialize())
		{
			result = ERROR_LOG_STATE_INVALID;
			break;
		}

		char app_cache_path[MAX_PATH] = { 0 };
		if (!SmartFsGetAppPathA(app_cache_path, "caches"))
		{
			result = ERROR_DIRECTORY_NOT_SUPPORTED;
			break;
		}

		CefClientApplicationConfig config;
		ZeroMemory(&config, sizeof(CefClientApplicationConfig));
		config.instance = instance;
		::lstrcpyA(config.window_title, "²âÊÔCEF¿Í»§¶Ë");

		config.window_icon_normal = IDI_APP_ICON;
		config.window_icon_small = IDI_APP_ICON_SMALL;
		config.width = 476;
		config.height = 800;
		config.x = (GetSystemMetrics(SM_CXSCREEN) - config.width) / 2;
		config.y = (GetSystemMetrics(SM_CYSCREEN) - config.height) / 2;
		config.always_on_top = false;
		config.with_captionbar = true;
		config.with_maximizebox = false;
		config.enable_singleton = false;
		config.with_osr = false;
		config.with_dev = false;
		
		//lstrcpyA(config.root_cache_path, app_cache_path);
		lstrcpyA(config.url, "https://i.meituan.com/awp/h5/hotel/list/list.html");
		//lstrcpyA(config.url, "https://i.meituan.com/awp/h5/hotel/search/search.html");

		CefClientApplication* app = CefClientApplicationManager::CreateClientApp(instance, &config);
		if (app == nullptr)
		{
			result = ERROR_NOT_ENOUGH_MEMORY;
			break;
		}

		app->SetBrowserResourceHandler(new DefaultResourceRequestHandler);

		result = app->Execute();

		app->Dispose();

	} while (false);

	SmartLogShutdown();

#ifdef _DEBUG
	assert(SmartMemCheck());
#endif // _DEBUG

	return result;
}
