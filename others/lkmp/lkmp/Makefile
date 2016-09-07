obj-m += hello-1.o
obj-m += hello-2.o
obj-m += hello-3.o
obj-m += hello-4.o
obj-m += hello-5.o
obj-m += startstop.o
startstop-objs := start.o stop.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

log:
	cat /var/log/messages

logc:
	cat > /var/log/messages
	make log

ins:
	insmod hello-$(ARGS).ko

insSS:
	insmod startstop.ko

rm:
	rmmod hello-$(ARGS).ko

rmSS:
	rmmod startstop.ko

info:
	modinfo hello-$(ARGS).ko

infoSS:
	modinfo startstop.ko

insall:
	insmod hello-1.ko
	insmod hello-2.ko
	insmod hello-3.ko
	insmod hello-4.ko
	insmod hello-5.ko
	insmod startstop.ko

rmall:
	rmmod hello-*.ko
	rmmod startstop.ko

infoall:
	modinfo hello-*.ko
	modinfo startstop.ko
