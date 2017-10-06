## Kernel Building (First Time)
First install Git and the build dependencies:
```
sudo apt-get install git bc
sudo apt-get install ncurses-dev
```
Next get the sources, which will take some time:
```
git clone --depth=1 https://github.com/raspberrypi/linux
```

RASPBERRY PI 1, PI 0, PI 0W, AND COMPUTE MODULE DEFAULT BUILD CONFIGURATION
```
cd linux
KERNEL=kernel
make bcmrpi_defconfig
```
RASPBERRY PI 2, PI 3, AND COMPUTE MODULE 3 DEFAULT BUILD CONFIGURATION
```
cd linux
KERNEL=kernel7
make bcm2709_defconfig
make menuconfig    
```
Build and install kernel
```
make zImage modules dtbs -j4 
sudo make modules_install
sudo cp arch/arm/boot/dts/*.dtb /boot/
sudo cp arch/arm/boot/dts/overlays/*.dtb* /boot/overlays/
sudo cp arch/arm/boot/dts/overlays/README /boot/overlays/
```
Copy the built zImage to boot directory and rename 
```
sudo cp arch/arm/boot/zImage /boot/haole-kernel.img
```
Finally, we have to re-configure the boot configuration file to choose specific kernel to boot
```
sudo nano /boot/config.txt 
```
Add these 2 lines:
```
kernel=haole-kernel.img
#kernel=kernel.img          Note: This will tell the system to not boot the default kernel 
```
Reboot and check the new kernel version
```
sudo reboot
uname -r
```

## To add a syscall 

We need to edit 4 files inside the linux folder: 
```
linux/arch/arm/kernel/calls.S
linux/arch/arm/include/uapi/asm/unistd.h
linux/arch/arm/kernel/sys_arm.c
linux/include/linux/syscalls.h
```
1. File 1: linux/arch/arm/kernel/calls.S
    Go to index 59
    Edit this line as follow:
    ````
    change 
    CALL(sys_ni_syscall) 
    to 
    CALL(sys_hello)
    ```
2. File 2: linux/arch/arm/include/uapi/asm/unistd.h
    Go to index 59, add:
    ```
    #define __NR_hello                      (__NR_SYSCALL_BASE+ 59)
    ```
3. File 3: linux/arch/arm/kernel/sys_arm.c
    Add this function at the end of the file:
    ```
    asmlinkage long sys_haole()
    {
        printk("Hello World, Hao Le\n");
        return 0;
    }
    ```
4. File 4: linux/include/linux/syscalls.h
    Add this line at the end of the file:
    ```
    asmlinkage long sys_hello(void);
    ```
    
Now after you configure all these files, we can re-compile the kernel
```
sudo make zImage modules dtbs -j4
sudo make modules_install
sudo cp arch/arm/boot/dts/*.dtb /boot/
sudo cp arch/arm/boot/dts/overlays/*.dtb* /boot/overlays/
sudo cp arch/arm/boot/dts/overlays/README /boot/overlays/
sudo cp arch/arm/boot/zImage /boot/haole-kernel.img
sudo reboot
```

Once your system is back on, we need to write a function to invoke the syscall 
```
$mkdir userspace
cd userspace
nano test.c
```
In this test.c file we add:
```
#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>

int main()
{
        long int s = syscall(59);
        printf("System call returned %1d \n",s);
        return 0;
}

```
Exit and compile
```
$ gcc test.c
$ ./a.out
```
Check your system log
```
$dmesg
```




