#include <iostream>

using namespace std;

struct qnode {struct qnode* next; bool locked;};
typedef qnode* lock;

void aquire_lock(lock, qnode*);
void release_lock(lock, qnode*);

void aquire_lock(lock* L, qnode* I) {
	I->next = nullptr;
	qnode* predecessor = __sync_val_compare_and_swap(L,*L, I);
	if (predecessor != nullptr) {
		I->locked = true;
		predecessor->next = I;
		while (I->locked) {};
	}
}
void release_lock(lock* L, qnode* I) {
	if (I->next == nullptr) {
		if (__sync_bool_compare_and_swap(L, I, nullptr)) {
			return;
		}
	while (I->next == nullptr) {};
	}
	I->next->locked = false;
}

int main() {
	lock l = nullptr;
	qnode q;
	int loop = 100;
	int x = 0;
  #pragma omp parallel shared(l, x) private(q)
  #pragma omp for
	for (int i = 0; i < loop; i++) {
		lock* L = &l;
		qnode* I = &q;
		aquire_lock(L,I);
		x += 1;
		cout << x << endl;
		release_lock(L,I);
	}
}

