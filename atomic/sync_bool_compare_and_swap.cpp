#include <iostream>
#include <chrono>
#include <omp.h>

using namespace std;

double inst();

int main() {
	int T = 1000;
	double sum = 0;
	for (int i = 0; i < T; i++) {
		sum += inst();
	}
	cout << "sync_bool_compare_and_swap: " << sum/static_cast<double>(T) << " ms" << endl;
}

double inst() {
	int loop = 100000;
	bool val = 0;
	auto start = chrono::system_clock::now();
	#pragma omp parallel shared(val)
	#pragma omp for
	for (int t = 0; t < loop; t++) {
		__sync_bool_compare_and_swap(&val, 0, 0);
	}
	auto end = chrono::system_clock::now();
	auto dur = end - start;
	double elapsed = static_cast<double>(chrono::duration_cast<chrono::milliseconds>(dur).count());
	return elapsed/static_cast<double>(loop);
}
