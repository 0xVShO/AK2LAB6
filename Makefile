obj-m += hello1.o
obj-m += hello2.o

ccflags-y += -I$(src)/inc -g

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
