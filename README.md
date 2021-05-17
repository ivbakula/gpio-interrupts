# GPIO interrupts on raspberry pi 4 

This is a really simple device driver for a device defined in device tree 
overlay (file *irq.dts*). The driver is built upon the *platform_device* bus.
Device consists of two GPIO lines, namely:
* GPIO 16 which serves as the output (high initially)
* GPIO 26 which serves as the interrupt in (low)

When wired up like in schematic, as shown in *wire up* section, pressing the 
switch sets irq line (gpio pin 26) high and triggers an interrupt 
(on rising edge). Interrupt is handled by interrupt service routine 
*gpio_irq_handler*, which inverts output on GPIO 16 turning the led on or off. 

Recommended reading:
* <linux_source_root>/Documentation/driver-api/driver-model/platform.rst
* <linux_source_root>/Documentation/gpio/intro.rst
* <linux_source_root>/Documentation/gpio/board.rst
* <linux_source_root>/Documentation/gpio/consumer.rst
* <linux_source_root>/Documentation/devicetree/usage-model.rst
* <linux_source_root>/Documentation/devicetree/overlay-notes.rst
* https://elinux.org/Device_Tree_Reference
* https://www.raspberrypi.org/documentation/configuration/device-tree.md

# Building on raspberri pi (raspbian buster) 

Install dependencies:
```bash
$ sudo apt-get install raspberrypi-kernel-headers build-essential bc git wget bison flex libssl-dev make libncurses-dev
$ sudo apt install device-tree-compiler
```
Build kernel module:
```bash
$ make
```
Build device tree overlay:
```bash
$ make overlay
```

Insert device tree overlay and kernel module:
```bash
$ sudo dtoverlay irq.dtbo
$ sudo insmod interrupt.ko 
```
# Wire up
<pre>
+3.3V                           GPIO16
  V     			  V 
  |				  |	
  |				  |
 \. (switch)			 <O> LED
  |				  |
  |				  |
  V				  V
GPIO26				 GND
</pre>


