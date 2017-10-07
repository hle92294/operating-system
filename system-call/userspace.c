#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#define __NR_sys_haole 59
int main()
{
	long int s = syscall(__NR_sys_haole);
	printf("System call returned %1d \n",s);
	return 0;
}
