// wrtcvideo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define WEBRTC_WIN

#include "webrtc/rtc_base/flags.h"
#include "webrtc/api/peerconnectioninterface.h"

#include "DataSocket.h"
#include "PeerChannel.h"

static const size_t kMaxConnections = (FD_SETSIZE - 2);

void HandleBrowserRequest(DataSocket* ds, bool* quit) {
	assert(ds && ds->valid());
	assert(quit);

	const std::string& path = ds->request_path();

	*quit = (path.compare("/quit") == 0);

	if (*quit) {
		ds->Send("200 OK", true, "text/html", "",
			"<html><body>Quitting...</body></html>");
	} else if (ds->method() == DataSocket::OPTIONS) {
		// We'll get this when a browsers do cross-resource-sharing requests.
		// The headers to allow cross-origin script support will be set inside
		// Send.
		ds->Send("200 OK", true, "", "", "");
	} else {
		// Here we could write some useful output back to the browser depending on
		// the path.
		printf("Received an invalid request: %s\n", ds->request_path().c_str());
		ds->Send("500 Sorry", true, "text/html", "",
			"<html><body>Sorry, not yet implemented</body></html>");
	}
}


int main()
{
	int SRV_PORT = 8080;

	ListeningSocket listener;
	if (!listener.Create()) {
		printf("Failed to create server socket\n");
		return -1;
	} else if (!listener.Listen(SRV_PORT)) {
		printf("Failed to listen on server socket\n");
		return -1;
	}
	printf("Server listening on port %i\n", SRV_PORT);

	PeerChannel clients;
	typedef std::vector<DataSocket*> SocketArray;
	SocketArray sockets;
	bool quit = false;
	while (!quit) {
		fd_set socket_set;
		FD_ZERO(&socket_set);
		if (listener.valid())
			FD_SET(listener.socket(), &socket_set);

		for (SocketArray::iterator i = sockets.begin(); i != sockets.end(); ++i)
			FD_SET((*i)->socket(), &socket_set);

		struct timeval timeout = { 10, 0 };
		if (select(FD_SETSIZE, &socket_set, NULL, NULL, &timeout) == SOCKET_ERROR) {
			printf("select failed\n");
			break;
		}

		for (SocketArray::iterator i = sockets.begin(); i != sockets.end(); ++i) {
			DataSocket* s = *i;
			bool socket_done = true;
			if (FD_ISSET(s->socket(), &socket_set)) {
				if (s->OnDataAvailable(&socket_done) && s->request_received()) {
					ChannelMember* member = clients.Lookup(s);
					if (member || PeerChannel::IsPeerConnection(s)) {
						if (!member) {
							if (s->PathEquals("/sign_in")) {
								clients.AddMember(s);
							} else {
								printf("No member found for: %s\n",
									s->request_path().c_str());
								s->Send("500 Error", true, "text/plain", "",
									"Peer most likely gone.");
							}
						} else if (member->is_wait_request(s)) {
							// no need to do anything.
							socket_done = false;
						} else {
							ChannelMember* target = clients.IsTargetedRequest(s);
							if (target) {
								member->ForwardRequestToPeer(s, target);
							} else if (s->PathEquals("/sign_out")) {
								s->Send("200 OK", true, "text/plain", "", "");
							} else {
								printf("Couldn't find target for request: %s\n",
									s->request_path().c_str());
								s->Send("500 Error", true, "text/plain", "",
									"Peer most likely gone.");
							}
						}
					} else {
						HandleBrowserRequest(s, &quit);
						if (quit) {
							printf("Quitting...\n");
							FD_CLR(listener.socket(), &socket_set);
							listener.Close();
							clients.CloseAll();
						}
					}
				}
			} else {
				socket_done = false;
			}

			if (socket_done) {
				printf("Disconnecting socket\n");
				clients.OnClosing(s);
				assert(s->valid());  // Close must not have been called yet.
				FD_CLR(s->socket(), &socket_set);
				delete (*i);
				i = sockets.erase(i);
				if (i == sockets.end())
					break;
			}
		}

		clients.CheckForTimeout();

		if (FD_ISSET(listener.socket(), &socket_set)) {
			DataSocket* s = listener.Accept();
			if (sockets.size() >= kMaxConnections) {
				delete s;  // sorry, that's all we can take.
				printf("Connection limit reached\n");
			} else {
				sockets.push_back(s);
				printf("New connection...\n");
			}
		}
	}

	for (SocketArray::iterator i = sockets.begin(); i != sockets.end(); ++i)
		delete (*i);
	sockets.clear();

	return 0;
}

