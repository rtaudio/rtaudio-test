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

	SignalBuffer playBuffer(nChannels, 1024*48*6);
	SignalBufferObserver observer(playBuffer);


	for (int i = 0; i < nChannels; i++) {
		autil::test::generateMusic(playBuffer.getPtrTQ(i), playBuffer.size);
	}

	autil::fileio::writeWave("music.wav", std::vector<float>(playBuffer.getPtrTQ(0), playBuffer.getPtrTQ(0) + playBuffer.size - 1));

	autil::AudioDriver::Request req(&driver);

	req
		.addSignal(&playBuffer, "out", autil::AudioDriver::Connect::ToPlayback)
		.addObserver(&observer)
		.execute();

	//using namespace std::chrono_literals;
	//std::this_thread::sleep_for(5s);

	observer.waitForCommit();

	req.remove(&playBuffer).remove(&observer).execute();

	driver.muteOthers(false);


    return 0;
}

