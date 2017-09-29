# Getting Started

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
## Back up 
Make a snapshot archive of the folder
```
cd /home/
sudo tar czf pi_9_29_9AM.tar.gz pi
```
This creates a tar archive called pi_home.tar.gz in /home/. You should copy this file to a USB stick or transfer it to another machine on your network.

### SD card image
```
sudo dd bs=4M if=/dev/sdb of=raspbian.img
```
This will create an image file on your computer which you can use to write to another SD card
```
sudo dd bs=4M if=raspbian.img of=/dev/sdb
```
To compress, you can pipe the output of dd to gzip to get a compressed file that is significantly smaller than the original size:
```
sudo dd bs=4M if=/dev/sdb | gzip > raspbian.img.gz
```
To restore, pipe the output of gunzip to dd:
```
gunzip --stdout raspbian.img.gz | sudo dd bs=4M of=/dev/sdb
```
If you are using a Mac, the commands used are almost exactly the same, but 4M in the above examples should be replaced with 4m, with a lower case letter.




