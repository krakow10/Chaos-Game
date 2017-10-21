// ChaosGame.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "lodepng.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <iostream>
const double pi = 3.1415926535897932385;
const int breaks = 5;//How many status updates to do during the iterations.

void encodeOneStep(const char* filename, std::vector<unsigned char>& image, unsigned width, unsigned height)
{
	//Encode the image
	unsigned error = lodepng::encode(filename, image, width, height);

	//if there's an error, display it
	if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}

int main(int argc, char* argv[])
{
	unsigned N = 3;//wow the compiler really doesn't like N=0
	unsigned size = 512;//Image size x and y
	unsigned long long int it_mul = 256;//Iteration multiplier, supposedly iterations per pixel

	for (int arg = 1; arg < argc; ++arg) {
		//std::cout << "Argument #" << arg << " " << argv[arg] << std::endl;
		if (strcmp(argv[arg], "-n") == 0)
			N = atoi(argv[++arg]);
		else if (strcmp(argv[arg], "-size") == 0)
			size = atoi(argv[++arg]);
		else if (strcmp(argv[arg], "-mul") == 0)
			it_mul = atoi(argv[++arg]);
	}
	std::cout << "N: " << N << std::endl;
	std::cout << "Size: " << size << std::endl;
	std::cout << "Mul: " << it_mul << std::endl;
	unsigned long long int it = (N < 3 ? it_mul* size*size*pi / 4 : it_mul * size*size * N*sin(2 * pi / N) / 8)/breaks;//reduce iterations to scale with polygon area.
	std::cout << "Iterations: " << breaks*it << std::endl;
	srand(time(NULL));
	unsigned int *histogram = new unsigned int[size*size];
	double cx = 0;
	double cy = 0;
	unsigned int m = 0;
	for (int b = 0; b < breaks; ++b) {
		if (N < 3) {//Circle
			for (unsigned long long i = 0; i < it; ++i) {
				double t = 2 * pi*rand() / RAND_MAX;
				cx = 0.5*(cx + size*(1 + cos(t)) / 2);
				cy = 0.5*(cy + size*(1 + sin(t)) / 2);
				unsigned int v = ++histogram[int(cy)*size + int(cx)];
				if (v > m) {
					m = v;
				}
			}
		}
		else {//Polygon
			double *px = new double[N];
			double *py = new double[N];
			for (unsigned i = 0; i < N; ++i) {
				px[i] = size*(1 + cos(pi*(2 * i + 1) / N)) / 2;
				py[i] = size*(1 + sin(pi*(2 * i + 1) / N)) / 2;
			}
			for (unsigned long long i = 0; i < it; ++i) {
				int choice = rand() % N;
				cx = 0.5*(cx + px[choice]);
				cy = 0.5*(cy + py[choice]);
				unsigned int v = ++histogram[int(cy)*size + int(cx)];
				if (v > m) {
					m = v;
				}
			}
		}
		std::cout << "Status update " << b+1 << "/" << breaks << std::endl;
	}
	printf("Max value: %d\n", m);
	std::cout << "Encoding..." << std::endl;
	std::vector<unsigned char> image;
	image.resize(size * size * 4);
	for (unsigned y = 0; y < size; ++y)
		for (unsigned x = 0; x < size; ++x)
		{
			unsigned char c = char(256 * histogram[x*size + y] / m);
			image[4 * size * y + 4 * x + 0] = c;
			image[4 * size * y + 4 * x + 1] = c;
			image[4 * size * y + 4 * x + 2] = c;
			image[4 * size * y + 4 * x + 3] = 255;
		}
	char number[14];
	int len = sprintf_s(number,"%d.png", N);
	encodeOneStep(number, image, size, size);
    return 0;
}

