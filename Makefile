obj-m += cadriver.o

MDIR = /lib/modules/$(shell uname -r)/build/
WDIR = $(shell pwd)


cadriver.ko: cadriver.c
	make --directory=$(MDIR) M=$(WDIR) modules

.PHONY: clean
clean:
	make --directory=$(MDIR) M=$(WDIR) clean
