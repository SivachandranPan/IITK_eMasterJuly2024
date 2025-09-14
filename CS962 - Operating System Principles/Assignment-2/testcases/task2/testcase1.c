#include<ulib.h>

static void func()
{
	int pid = getpid();
	walk_pt(&pid);
	printf("===================\n");
}

int main(u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5)
{

	printf("===================\n");
	walk_pt(&func);
	printf("===================\n");
        
	func();
	return 0;
}
