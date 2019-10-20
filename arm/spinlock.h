#ifndef SPINLOCK_H_
#define SPINLOCK_H_
#include <iostream>
#include <atomic>

using namespace std;

static inline void arch_spin_lock(arch_spin_lock_t *lock) {
  unsigned long tmp;

  __asm__ __volatile__(
    "1: ldrex %0, [%1]\n"
    "   teq %0, #0\n"
    WFE("ne")
    "   strexeq %0, %2, [%1]\n"
    "   teqeq %0, #0\n"
    "   bne 1b"
    : "=&r" (tmp)
    : "r" (%lock->lock), "r" (1)
    : "cc");

  smp_mb();
}

static inline void arch_spin_unlock(arch_spin_lock_t *lock) {
  smp_mb();

  __asm__ __volatile__(
      "  str %1, [%0]\n"
      :
      : "r" (&lock->lock), "r" (0)
      : "cc");
  dsb_sev();
}

static inline void dsb_sev(void) {
  #if __LINUX_ARM_ARCH__ >= 7
    __asm__ __volatile__(
        "dsb\n"
      SEV
    );
  # else
    __asm__ __volatile__(
        "mcr p15, 0, %0, c7, c10, 4\n"
        SEV
        : : "r" (0)
    );
  # endif
}




#endif /*SPINLOCK_H_*/
