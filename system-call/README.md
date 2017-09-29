## Set up

```
sudo apt-get update
sudo apt-get upgrade
```
Network configuration:
```
sudo nano /etc/wpa_supplicant/wpa_supplicant.conf
```

Go to the bottom of the file and add the following:
```
network={
    ssid="testing"
    psk="testingPassword"
}
```

## Kernel Building
First install Git and the build dependencies:
```
sudo apt-get install git bc
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
```
Build and install kernel (take long time)
```
make -j4 zImage modules dtbs
sudo make modules_install
sudo cp arch/arm/boot/dts/*.dtb /boot/
sudo cp arch/arm/boot/dts/overlays/*.dtb* /boot/overlays/
sudo cp arch/arm/boot/dts/overlays/README /boot/overlays/
sudo cp arch/arm/boot/zImage /boot/$KERNEL.im
```



