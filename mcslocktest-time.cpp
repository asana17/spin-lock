#include <pthread.h>
#include <cmath>
#include <chrono>
#include "mcslock3.h"

int num_threads = 64;
int x = 0;
int loop = pow(2, 17)/num_threads;

mcs_lock global_lock = NULL;
void* f(void *arg) {
	for (int i = 0; i < loop; i++) {
		aquire_lock(global_lock);
		x += 1;
		release_lock(global_lock);
	}
	return NULL;
}
int main() {
	pthread_t pthread[num_threads];
  auto start = chrono::system_clock::now();
	for (int i = 0; i < num_threads; i++) {
		pthread_create(&pthread[i], NULL, f, NULL);
	}
	for (int i = 0; i < num_threads; i++) {
		pthread_join(pthread[i], NULL);
	}
  auto end = chrono::system_clock::now();
  double elapsed = chrono::duration_cast<chrono::microseconds>(end-start).count();
	cout << x << endl;
  cout << "elapsed " << elapsed/1000.0 << " millisec" << endl;
}
