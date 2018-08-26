#include "stdafx.h"
#include "time_utils.h"
#include "CustomVideoTrackSource.h"

#include "webrtc/api/video/i420_buffer.h"
#include "webrtc/api/video/video_frame.h"

void supplyYUVframes(CustomVideoTrackSource *ref);

CustomVideoTrackSource::CustomVideoTrackSource() {
	runThread = true;
	readerThread.reset(new std::thread(supplyYUVframes, this));
}

CustomVideoTrackSource::~CustomVideoTrackSource() {
	runThread = false;
	readerThread->join();
}

void CustomVideoTrackSource::OnFrame(const webrtc::VideoFrame& frame) {
	AdaptedVideoTrackSource::OnFrame(frame);
}

// Copied from frame_utils.cc
rtc::scoped_refptr<webrtc::I420Buffer> ReadI420Buffer(int width, int height, FILE *f) {
	int half_width = (width + 1) / 2;
	rtc::scoped_refptr<webrtc::I420Buffer> buffer(
		// Explicit stride, no padding between rows.
		webrtc::I420Buffer::Create(width, height, width, half_width, half_width));
	size_t size_y = static_cast<size_t>(width) * height;
	size_t size_uv = static_cast<size_t>(half_width) * ((height + 1) / 2);

	if (fread(buffer->MutableDataY(), 1, size_y, f) < size_y)
		return nullptr;
	if (fread(buffer->MutableDataU(), 1, size_uv, f) < size_uv)
		return nullptr;
	if (fread(buffer->MutableDataV(), 1, size_uv, f) < size_uv)
		return nullptr;
	return buffer;
}

void supplyYUVframes(CustomVideoTrackSource *ref) {

	FILE *fh = fopen("c:/Developer/wrtcvideo/x64/Release/Static_152_100.yuv", "rb");

	int width = 152;
	int height = 100;

	unsigned long timestamp = 0;

	while (ref->runThread) {

		rtc::scoped_refptr<webrtc::I420Buffer> buffer = ReadI420Buffer(width, height, fh);
		if (buffer == nullptr) {
			fseek(fh, SEEK_SET, 0);
			buffer = ReadI420Buffer(width, height, fh);
		}

		// 20 fps, so 1000 milliseconds / 20 = 50
		timestamp += 50;

		webrtc::VideoFrame frame(std::move(buffer), static_cast<webrtc::VideoRotation>(0), timestamp);
		ref->OnFrame(frame);

		sleep(50);
	}

	fclose(fh);
}
