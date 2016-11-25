#include <iostream>
#include <vector>
#include <chrono>
#include <string.h>

#include <autil/test.h>

int main(int argc, char **argv)
{
	int blockSize = 128;

	std::cout << "blockSize " << blockSize << std::endl;

	if (argc  > 1) {
		blockSize = atoi(argv[1]);
	}

	if (blockSize < 8 || blockSize > 1024 * 10) {
		std::cerr << "please specify block within [8, 10240]!" << std::endl;
		return 1;
	}

	std::vector<float *> blocks(200000);
	int s = blocks.size();

	auto begin = std::chrono::high_resolution_clock::now();
	for (auto &b : blocks) {
		b = new float[blockSize];
	}
	auto end = std::chrono::high_resolution_clock::now();

	std::cout << "per block allocation: " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / blocks.size()) << "ns" << std::endl;

	begin = std::chrono::high_resolution_clock::now();
	for (auto &b : blocks) {
		// just fill with noise
		for (int i = 0; i < blockSize; i++) {
			b[i] = (float)autil::test::fastRand() / (float)s;
		}
	}
	end = std::chrono::high_resolution_clock::now();

	std::cout << "per block rand fill: " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / blocks.size()) << "ns" << std::endl;


	int N = 10000000;

	begin = std::chrono::high_resolution_clock::now();
	// randomly copy around samples
	for (int i = 0; i < N; i++) {
		int a = autil::test::fastRand() % s;
		int b = autil::test::fastRand() % s;
		memcpy(blocks[a], blocks[b], blockSize * sizeof(float));
	}
	end = std::chrono::high_resolution_clock::now();

	std::cout << "per memcpy: " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / N) << "ns" << std::endl;
	

	getchar();
	return 0;
}