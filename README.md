# Kernelmodule_gpioToggle

***gpio terminal toggle with kernelmodule***

## Usage
# install rpi kernel headers library
```bash
sudo apt-get install raspberrypi-kernel-headers
 ```
 # go to correct folder
 ```bash
cd leds_edges_kmod
```
# make project
 ```bash
make
```
# start module with parameters
 ```bash
sudo insmod clargmod.ko toggleSpeed=x ioPins=x,x 
```
# check logs
 ```bash
dmesg
```
# remove kernel module
 ```bash
sudo rmmod clargmod.ko
```
