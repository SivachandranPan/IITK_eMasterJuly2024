#include<ulib.h>
int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{

	char *ptr = mmap(NULL, 8192, PROT_READ | PROT_WRITE, 0);

	printf("===================\n");
	walk_pt(ptr);
	walk_pt(ptr+4096);
	printf("===================\n");
	
	*ptr = 'c';
	walk_pt(ptr);
	walk_pt(ptr+4096);
	printf("===================\n");

	*(ptr+4096) = 'e';
	walk_pt(ptr+4096);
	printf("===================\n");

	return 0;
}
