#include<ulib.h>

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{
  char *addr1 = mmap(NULL, 8192, PROT_READ|PROT_WRITE, 0);
  if((long)addr1 < 0)
  {
    printf("Test case failed\n");
    return 1;
  }
  pmap();

  int retval = munmap(addr1, 8192);

  if(retval < 0)
  {
    printf("Test case failed\n");
    return 1;
  }
  
  addr1 = mmap(NULL, 4096, PROT_READ|PROT_WRITE, 0);
  if((long)addr1 < 0)
  {
    printf("Test case failed\n");
    return 1;
  }

  char *addr2 = mmap(NULL, 16384, PROT_READ, 0);
  if((long)addr1 < 0)
  {
    printf("Test case failed\n");
    return 1;
  }
  pmap();
  
  retval = munmap(addr2, 16384);

  if(retval < 0)
  {
    printf("Test case failed\n");
    return 1;
  }
  pmap();
  return 0;
}
