// peer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "dmoguids.lib")
#pragma comment(lib, "wmcodecdspuuid.lib")
#pragma comment(lib, "amstrmid.lib")
#pragma comment(lib, "msdmo.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "secur32.lib")

#include "flagdefs.h"
#include "conductor.h"
#include "main_wnd.h"
#include "peer_connection_client.h"

#include "webrtc/rtc_base/checks.h"
#include "webrtc/rtc_base/ssladapter.h"
#include "webrtc/rtc_base/win32socketinit.h"
#include "webrtc/rtc_base/win32socketserver.h"

int PASCAL wWinMain(HINSTANCE instance, HINSTANCE prev_instance,
	wchar_t* cmd_line, int cmd_show) {
	rtc::EnsureWinsockInit();
	rtc::Win32SocketServer w32_ss;
	rtc::Win32Thread w32_thread(&w32_ss);
	rtc::ThreadManager::Instance()->SetCurrentThread(&w32_thread);

	rtc::WindowsCommandLineArguments win_args;
	int argc = win_args.argc();
	char **argv = win_args.argv();

	rtc::FlagList::SetFlagsFromCommandLine(&argc, argv, true);
	if (FLAG_help) {
		rtc::FlagList::Print(NULL, false);
		return 0;
	}

	// Abort if the user specifies a port that is outside the allowed
	// range [1, 65535].
	if ((FLAG_port < 1) || (FLAG_port > 65535)) {
		printf("Error: %i is not a valid port.\n", FLAG_port);
		return -1;
	}

	MainWnd wnd(FLAG_server, FLAG_port, FLAG_autoconnect, FLAG_autocall);
	if (!wnd.Create()) {
		RTC_NOTREACHED();
		return -1;
	}

	rtc::InitializeSSL();
	PeerConnectionClient client;
	rtc::scoped_refptr<Conductor> conductor(
		new rtc::RefCountedObject<Conductor>(&client, &wnd));

	conductor->receiveOnly = FLAG_receive;

	// Main loop.
	MSG msg;
	BOOL gm;
	while ((gm = ::GetMessage(&msg, NULL, 0, 0)) != 0 && gm != -1) {
		if (!wnd.PreTranslateMessage(&msg)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	if (conductor->connection_active() || client.is_connected()) {
		while ((conductor->connection_active() || client.is_connected()) &&
			(gm = ::GetMessage(&msg, NULL, 0, 0)) != 0 && gm != -1) {
			if (!wnd.PreTranslateMessage(&msg)) {
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
	}

	rtc::CleanupSSL();
	return 0;
}

