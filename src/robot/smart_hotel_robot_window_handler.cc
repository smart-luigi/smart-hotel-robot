#include "precomp.h"
#include "smart_hotel_robot_window_handler.h"

SmartHotelRobotWindowHandler::SmartHotelRobotWindowHandler(SmartHotelRobotContext* context)
	: _context(context)
{

}

SmartHotelRobotWindowHandler::~SmartHotelRobotWindowHandler()
{

}

LRESULT SmartHotelRobotWindowHandler::HandleWindowMessage(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	LRESULT result = ERROR_SUCCESS;

	switch (message)
	{
	case WM_CREATE:
		result = _context->PostMessageRobotStarted();
		break;
	case WM_DESTROY:
		result = _context->PostMessageRobotStopped();
		PostMessage(hwnd, WM_EXIT, 0, 0);
		break;
	case WM_EXIT:
		ExitProcess(ERROR_SUCCESS);
		break;
	case WM_SIMULATE_START:
		result = HandleMessageSimulateStart(hwnd);
		break;
	default:
		break;
	}
	
	return DefWindowProc(hwnd, message, wparam, lparam);
}

int SmartHotelRobotWindowHandler::HandleMessageSimulateStart(HWND hwnd)
{
	RECT rc;
	if (GetWindowRect(hwnd, &rc))
	{
		POINT pt;
		pt.x = rc.right - 16;
		pt.y = rc.top + 64;
		int height = rc.bottom - rc.top;
		SmartMouseSimulator::MouseMove(pt.x, pt.y);

		SmartMouseSimulator::MouseLeftDown(pt.x, pt.y);
		Sleep(1000);

		SmartMouseSimulator::MouseMove(pt.x, pt.y + height - 64);
		Sleep(1000);

		SmartMouseSimulator::MouseLeftUp(pt.x, pt.y + height - 64);
	}

	return ERROR_SUCCESS;
}