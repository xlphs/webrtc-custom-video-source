#include "stdafx.h"

#include "defaults.h"

#include <stdlib.h>
#include <string.h>

#include <winsock2.h>

#include "webrtc/rtc_base/arraysize.h"

const char kAudioLabel[] = "audio_label";
const char kVideoLabel[] = "video_label";
const char kStreamLabel[] = "stream_label";
const uint16_t kDefaultServerPort = 8080;

std::string GetEnvVarOrDefault(const char* env_var_name,
	const char* default_value) {
	std::string value;
	const char* env_var = getenv(env_var_name);
	if (env_var)
		value = env_var;

	if (value.empty())
		value = default_value;

	return value;
}

std::string GetPeerConnectionString() {
	return GetEnvVarOrDefault("WEBRTC_CONNECT", "stun:stun.l.google.com:19302");
}

std::string GetDefaultServerName() {
	return GetEnvVarOrDefault("WEBRTC_SERVER", "localhost");
}

std::string GetPeerName() {
	char computer_name[256];
	std::string ret(GetEnvVarOrDefault("USERNAME", "user"));
	ret += '@';
	if (gethostname(computer_name, arraysize(computer_name)) == 0) {
		ret += computer_name;
	} else {
		ret += "host";
	}
	return ret;
}
