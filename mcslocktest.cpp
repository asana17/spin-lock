#include <pthread.h>
#include "mcslock3.h"
int num_threads = 4;
int x = 0;

mcs_lock global_lock = NULL;
void* f(void *arg) {
	for (int i = 0; i < 100000; i++) {
		aquire_lock(global_lock);
		x += 1;
		release_lock(global_lock);
	}
	return NULL;
}
int main() {
	pthread_t pthread[num_threads];
	for (int i = 0; i < num_threads; i++) {
		pthread_create(&pthread[i], NULL, f, NULL);
	}
	for (int i = 0; i < num_threads; i++) {
		pthread_join(pthread[i], NULL);
	}
	cout << x << endl;
}

