ifneq ($(KERNELRELEASE),)
	obj-m := interrupt.o

else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
	OVERLAY=irq_led
	OVERLAY_SRC=$(OVERLAY).dts
	OVERLAY_BIN=$(OVERLAY).dtbo

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm $(OVERLAY_BIN)

overlay:
	dtc -@ -Hepapr -I dts -O dtb -o $(OVERLAY_BIN) $(OVERLAY_SRC)

endif