#ifndef MCSLOCK3_H_
#define MCSLOCK3_H_
#include <iostream>

using namespace std;

#define COMPILER_BALLIER() __asm__ __volatile__("" : : : "memory")
#define CPU_RELAX() __asm__ __volatile__("pause\n" : : :"memory")
#define CAS(address, exp, target) __sync_bool_compare_and_swap(address, exp, target)
#define ATOMIC_EXCHANGE(address, val) __atomic_exchange_n(address, val, __ATOMIC_SEQ_CST)
#define CPU_BALLIER() __sync_synchronize()

struct mcs_lock_node{
	volatile int waiting;
	mcs_lock_node *volatile next;
};

typedef mcs_lock_node *volatile mcs_lock;

static mcs_lock_node* get_my_mcs_node() {
	static __thread mcs_lock_node my_mcs_node;
	return &my_mcs_node;
}

static void aquire_lock(mcs_lock &lock) {
	mcs_lock_node *me = get_my_mcs_node();
	mcs_lock_node *tmp = me;
	mcs_lock_node *pre;
	me->next = NULL;
	pre = ATOMIC_EXCHANGE(&lock, tmp);
	if (pre == NULL) {
		return;
	}
	me->waiting = 1;
	COMPILER_BALLIER();
	pre->next = me;
	while (me->waiting) {
		CPU_RELAX();
	}
	CPU_BALLIER();
}

static void release_lock(mcs_lock &lock) {
	CPU_BALLIER();
	mcs_lock_node *me = get_my_mcs_node();
	mcs_lock_node *tmp = me;
	if (me->next == NULL) {
		if (CAS(&lock, tmp, NULL)) {
			return;
		}
		while (me->next == NULL) {
			CPU_RELAX();
		}
	}
	me->next->waiting = 0;
}

#endif /*MCSLOCK3_H_*/
