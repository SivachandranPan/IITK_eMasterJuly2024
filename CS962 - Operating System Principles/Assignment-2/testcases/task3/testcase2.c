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
  pmap();
  
  retval = munmap(addr1, 8192);

  if(retval == 0)
  {
    printf("Test case failed\n");
    return 1;
  }
  printf("Test case passed\n");
  return 0;
}
