// rtaudio-test.cpp : Defines the entry point for the console application.
//

#include <algorithm>

#include <cstdint>

#include <autil/audio_driver.h>
#include <autil/test.h>
#include <autil/file_io.h>
#include <thread>
#include <chrono>
#include <iostream>


int main()
{
	std::cout << "starting audio driver...";
	autil::AudioDriver driver("rtaudio-testing");
	driver.muteOthers(true);
	std::cout << "started" << std::endl;

	int np = driver.getNumPlaybackChannels();
	int nc = driver.getNumCaptureChannels();
	int nChannels = (std::min)(np, nc);

	std::cout << "nChannels:" << nChannels << std::endl;

	const int len = 2 << 14;

	SignalBuffer playBuffer(nChannels, len);
	SignalBuffer inBuffer(nc, playBuffer.size);


	SignalBufferObserver observer(playBuffer, inBuffer);


	for (int i = 0; i < nChannels; i++) {
		autil::test::generateNoise(playBuffer.getPtrTQ(i), playBuffer.size);
	}

	

	autil::AudioDriver::Request req(&driver);

	req
		.addSignal(&playBuffer, "out", autil::AudioDriver::Connect::ToPlayback)
		.addSignal(&inBuffer, "in", autil::AudioDriver::Connect::ToCapture)
		.addObserver(&observer)
		.execute();

	//using namespace std::chrono_literals;
	//std::this_thread::sleep_for(5s);

	observer.waitForCommit();

	req
		.remove(&playBuffer)
		.remove(&inBuffer)
		.remove(&observer)
		.execute();

	driver.muteOthers(false);

	inBuffer.normalize();


	autil::fileio::writeWave("music.wav", std::vector<float>(playBuffer.getPtrTQ(0), playBuffer.getPtrTQ(0) + playBuffer.size - 1));
	autil::fileio::writeWave("music-recorded.wav", std::vector<float>(inBuffer.getPtrTQ(0), inBuffer.getPtrTQ(0) + inBuffer.size - 1));


    return 0;
}

