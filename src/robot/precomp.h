#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <shlwapi.h>
#include <shellapi.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <thread>

#include <boost/algorithm/string.hpp>
#include <gumbo.h>
#include <smart_base.h>
#include <smart_adv.h>
#include <smart_cef_client.h>
#include "resource.h"
#include "service_ipc.h"
#include "smart_hotel_message.h"