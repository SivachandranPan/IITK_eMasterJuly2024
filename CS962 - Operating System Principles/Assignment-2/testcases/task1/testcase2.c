#include<ulib.h>

int main (u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) {

    int a = 10;
    pmaps();
    char *ptr1 = mmap(NULL, 4096, PROT_WRITE, 0);
    if((long)ptr1< 0)
    {
        printf("MMAP FAILED\n");
        return -1;
    }
    pmaps();
    munmap(ptr1, 4096);
    return 0;

}
