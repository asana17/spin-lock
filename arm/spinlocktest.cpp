#include <chrono>
#include <pthread.h>
#include <cmath>
#include "spinlock.h"

int num_threads = 4;
int x = 0;
int loop = pow(2, 23)/num_threads;

arch_spin_lock_t *lock = 0;

void* f(void *arg) {
	for (int i = 0; i < loop; i++) {
    arch_spin_lock(lock);
		x += 1;
    arch_spin_unlock(lock);
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
	double elapsed = chrono::duration_cast<chrono::milliseconds>(end-start).count();
	cout << x << endl;
	cout << "elapsed " << elapsed << " ms" << endl;
}
