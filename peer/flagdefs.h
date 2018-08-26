#ifndef WEBRTC_EXAMPLES_PEERCONNECTION_CLIENT_FLAGDEFS_H_
#define WEBRTC_EXAMPLES_PEERCONNECTION_CLIENT_FLAGDEFS_H_

#include "webrtc/rtc_base/flags.h"

extern const uint16_t kDefaultServerPort;  // From defaults.[h|cc]

																					 // Define flags for the peerconnect_client testing tool, in a separate
																					 // header file so that they can be shared across the different main.cc's
																					 // for each platform.

DEFINE_bool(help, false, "Prints this message");
DEFINE_bool(autoconnect, false, "Connect to the server without user "
	"intervention.");
DEFINE_string(server, "localhost", "The server to connect to.");
DEFINE_int(port, kDefaultServerPort,
	"The port on which the server is listening.");
DEFINE_bool(autocall, false, "Call the first available other client on "
	"the server without user intervention.  Note: this flag should only be set "
	"to true on one of the two clients.");
DEFINE_bool(receive, false,
	"Only receive, do not send streams");

#endif  // WEBRTC_EXAMPLES_PEERCONNECTION_CLIENT_FLAGDEFS_H_
