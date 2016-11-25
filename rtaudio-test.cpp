// rtaudio-test.cpp : Defines the entry point for the console application.
//

#include <algorithm>

#include <cstdint>

#include <thread>
#include <chrono>
#include <iostream>


#include <autil/audio_driver.h>
#include <autil/test.h>
#include <autil/file_io.h>
#include <autil/fft.h>
#include <autil/logging.h>
#include <autil/net.h>

void latencyTest(autil::AudioDriver &driver) {

	int nChannels = (std::min)(driver.getNumPlaybackChannels(), driver.getNumCaptureChannels());

	LOG(logINFO) << "nChannels:  " << nChannels;
	LOG(logINFO) << "Block size: " << driver.getBlockSize();


	SignalBuffer playBuffer("exitation", nChannels, 2 << 14);
	SignalBuffer inBuffer("recorded", nChannels, playBuffer.size);
	SignalBufferObserver calibrationObserver;
	
	for (int i = 0; i < nChannels; i++) {
		//autil::test::generateNoise(playBuffer.getPtrTQ(i), playBuffer.size);
		autil::test::generateSweep(playBuffer.getPtrTQ(i), playBuffer.size);
	}

	autil::AudioDriver::Request req(&driver);

	req
		.addBuffer(&playBuffer, autil::AudioDriver::Connect::ToPlayback)
		.addBuffer(&inBuffer, autil::AudioDriver::Connect::ToCapture);

	req.executeAndObserve(&calibrationObserver);
	calibrationObserver.waitForCommit();
	req.undo();

	inBuffer.normalize();

	auto fft = autil::fft::getFFT(playBuffer.size);
	auto ifft = autil::fft::getIFFT(playBuffer.size);

	std::vector<autil::complex_float> specE((playBuffer.size + 1)), specR((playBuffer.size + 1));
	for (int i = 0; i < nChannels; i++) {
		fft(playBuffer.getPtrTQ(i), &specE);
		fft(inBuffer.getPtrTQ(i), &specR);

		auto fD = specR / specE;
		auto tD = ifft(fD);

		normalize(&tD);

		autil::UdpSocket soc("192.168.137.1", 25025 + 1);
		autil::fileio::writeWave("d" + std::to_string(i) + "-" + std::to_string(driver.getBlockSize()) + ".wav", tD);

		size_t delay;
		auto peakVal = absmax(tD, &delay);


		auto q = peakVal*peakVal * static_cast<float>(tD.size()) / (energy(tD) * 4.0f);

		// mask out dirac and compute side energy
		int maskSpan = static_cast<int>(0.5f / 1000.0f * (float)driver.getSampleRate()); // 0.5ms;
		std::fill_n(tD.begin() + (std::max)(0, static_cast<int>(delay) - maskSpan), maskSpan*2, 0);

		auto sidePeak = absmax(tD);
		auto nonPeakEnergy = energy(tD);

		if ((sidePeak / peakVal) > 0.0001) {
			LOG(logERROR) << "Impulse response has high side peak, probably due to buffer X runs";
		}

		if (nonPeakEnergy > 0.01) {
			LOG(logERROR) << "Impulse response has high side energy, probably due to buffer X runs";
		}
		if (q < 1000) {
			LOG(logWARNING) << "High noise!";
		}

		if (q < 20) {
			LOG(logERROR) << "Signal unusable!";
		}

		float snr = 0;
		float n = 0, s = 0, d;

		for (size_t j = delay; j < playBuffer.size; j++) {
			 d = playBuffer.getPtrTQ(i)[j-delay] - inBuffer.getPtrTQ(i)[j];
			 n += d*d;
			 s += playBuffer.getPtrTQ(i)[j - delay] * playBuffer.getPtrTQ(i)[j - delay];
		}

		std::cout << "s:" << s << " n:" << n << std::endl;
		std::cout << "delay: " << delay << " q: " << std::round(10 * std::log10(q)) << " dB" << std::endl;
		std::cout << "nonpeakenergy:" << nonPeakEnergy << std::endl;


		// check spectrum
		auto fD_abs = autil::abs(fD); // real values

		soc.Send({ &tD, &fD_abs });

		// drop 2nd half and take 80% of spectrum
		fD_abs = std::vector<float>(fD_abs.begin(), fD_abs.end() - static_cast<int>(fD_abs.size() / 2.5));
		
		//10 * std::log10(autil::max(fD_abs) / autil::min(fD_abs));

		delay = 0;
		autil::fileio::writeWave("e.wav", std::vector<float>(playBuffer.getPtrTQ(0), playBuffer.getPtrTQ(0) + playBuffer.size - 1 - delay));
		autil::fileio::writeWave("r" + std::to_string(driver.getBlockSize()) +".wav", std::vector<float>(inBuffer.getPtrTQ(0) + delay, inBuffer.getPtrTQ(0) + inBuffer.size - 1));

		break;
		
		SignalBuffer streamPlayBuffer("exitation-stream", nChannels, 2 << 14, delay);
		SignalBuffer streamInBuffer("recorded-stream", nChannels, streamPlayBuffer.size);
		SignalBufferObserver streamObserver;


		for (int i = 0; i < nChannels; i++) {
			autil::test::generateNoise(streamPlayBuffer.getPtrTQ(i), streamPlayBuffer.size);

		}


		while (true) {
			req
				.addBuffer(&streamPlayBuffer, autil::AudioDriver::Connect::ToPlayback)
				.addBuffer(&streamInBuffer, autil::AudioDriver::Connect::ToCapture);
			req.executeAndObserve(&streamObserver);
			streamObserver.waitForCommit();

			for (int c = 0; c < nChannels; c++) {
				for (size_t i = 0; i < streamPlayBuffer.size; i++) {
					//playBuffer.getPtrTS(c);
					//streamInBuffer.getPtrTS(c);
				}
			}
		}

		
	}


	

	
	//std::vector<autil::SignalStreamer> streamers;

	/*
	for (int i = 0; i < nChannels; i++) {
		autil::SignalStreamer streamer([i](const float * in, float * out, size_t nframes) {
			out[i] = 1;
			return true;
		});
		//req.addStreamer(&streamer, "streamer", autil::AudioDriver::Connect::ToPlayback | autil::AudioDriver::Connect::ToCapture, i);
		streamers.emplace_back(std::move(streamer));
	}
	*/

	//req.execute().wait();

	//std::for_each(streamers.begin(), streamers.end(), [](auto &s) {s.waitForEnd(); });
	
}

int main1()
{
	autil::AudioDriver driver("rtaudio-testing");
	driver.muteOthers(true);




	std::vector<int> blockSizes{ 8, 16, 32, 48, 64, 96, 128, 256 };

	for (auto bs : blockSizes) {
		try { driver.setBlockSize(bs); }
		catch (std::exception e) {
			continue;
		}
		latencyTest			(driver);
	}


	driver.muteOthers(false);

	


	//autil::fileio::writeWave("music.wav", std::vector<float>(playBuffer.getPtrTQ(0), playBuffer.getPtrTQ(0) + playBuffer.size - 1));
	//autil::fileio::writeWave("music-recorded.wav", std::vector<float>(inBuffer.getPtrTQ(0), inBuffer.getPtrTQ(0) + inBuffer.size - 1));
	


    return 0;
}
