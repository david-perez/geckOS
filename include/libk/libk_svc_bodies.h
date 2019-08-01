#ifndef LIBK_SVC_CALLS_H
#define LIBK_SVC_CALLS_H

#define SYSCALL1(ret_type, svc_name, svc_nr, t1, p1)     void svc_name(t1 p1) { \
    register t1 r0 asm("r0") = p1; \
    \
    __asm__ volatile("svc %[nr]\n" \
            : "=r" (r0) \
            : [nr] "i" (svc_nr), "r" (r0) \
            : "memory", "r1", "r2", "r3", "r12", "lr"); \
}

#define SYSCALL2(ret_type, svc_name, svc_nr, t1, p1, t2, p2)     void svc_name(t1 p1, t2 p2) { \
    register t1 r0 asm("r0") = p1; \
    register t2 r1 asm("r1") = p2; \
    \
    __asm__ volatile("svc %[nr]\n" \
            : "=r" (r0) \
            : [nr] "i" (svc_nr), "r" (r0), "r" (r1) \
            : "memory", "r2", "r3", "r12", "lr"); \
}

#define SYSCALL3(ret_type, svc_name, svc_nr, t1, p1, t2, p2, t3, p3)     void svc_name(t1 p1, t2 p2, t3 p3) { \
    register t1 r0 asm("r0") = p1; \
    register t2 r1 asm("r1") = p2; \
    register t3 r2 asm("r2") = p3; \
    \
    __asm__ volatile("svc %[nr]\n" \
            : "=r" (r0) \
            : [nr] "i" (svc_nr), "r" (r0), "r" (r1), "r" (r2) \
            : "memory", "r3", "r12", "lr"); \
}

#endif // LIBK_SVC_CALLS_H
