#ifndef _CUSTOMVIDEOTRACKSOURCE_H_
#define _CUSTOMVIDEOTRACKSOURCE_H_

#include <webrtc/media/base/adaptedvideotracksource.h>

#include <memory>
#include <thread>

class CustomVideoTrackSource : public rtc::AdaptedVideoTrackSource {
public:
	CustomVideoTrackSource();
	~CustomVideoTrackSource();

	bool is_screencast() const { return false; }
	rtc::Optional<bool> needs_denoising() const {
		return rtc::Optional<bool>(false);
	}

	SourceState state() const { return SourceState(); }
	bool remote() const { return false; }

	void OnFrame(const webrtc::VideoFrame& frame);
	
	bool runThread;

private:
	std::unique_ptr<std::thread> readerThread;

};

#endif
