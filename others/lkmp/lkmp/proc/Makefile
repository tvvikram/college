obj-m += proc1.o
obj-m += proc2.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

log:
	cat /var/log/messages

logc:
	cat > /var/log/messages
	make log

cat:
	cat /proc/buffer1k

ins:
	insmod proc$(ARGS).ko

rm:
	rmmod proc$(ARGS).ko

info:
	modinfo proc$(ARGS).ko

insall:
	insmod proc1.ko
	insmod proc2.ko

rmall:
	rmmod proc*.ko

infoall:
	modinfo proc1.ko
	modinfo proc2.ko
