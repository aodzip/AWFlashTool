PROJECT = flashtool
SOURCEDIR = ./src

DISK = /dev/sdb

# DEFINES = -D PC_DEBUG
# CC = gcc
# PLATFORMLINK = -l SDL2

CC = /home/aodzip/buildroot-2020.02/output/host/bin/arm-linux-gcc
PLATFORMLINK =

INCLUDES = -I $(SOURCEDIR) -I ./$(SOURCEDIR)/vendor/littlevgl
OPTIMIZE = -Wl,--build-id=none -s -flto
CFLAGS = $(DEFINES) $(INCLUDES) $(OPTIMIZE)
LINKS = $(OPTIMIZE) -l util -l m -l pthread $(PLATFORMLINK)
SOURCES = $(shell find "$(SOURCEDIR)" -name "*.cpp" -o -name "*.c" -o -name "*.S")
OBJECTS = $(patsubst %.S, %.o, $(patsubst %.c, %.o, $(patsubst %.cpp, %.o, $(SOURCES))))

all: $(PROJECT)

$(PROJECT) : $(OBJECTS)
	$(CC) -o $(PROJECT) $(OBJECTS) $(CFLAGS) $(LINKS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	find src -name "*.o"  | xargs rm -f
	rm -f $(PROJECT)

flash: all
	sudo mount $(DISK)2 /mnt

	sudo cp res/S99Application /mnt/etc/init.d/

	sudo mkdir -p /mnt/opt/flashtool
	sudo cp flashtool /mnt/opt/flashtool/

	sudo mkdir -p /mnt/opt/flashtool/script
	sudo cp script/* /mnt/opt/flashtool/script/

	sudo mkdir -p /mnt/opt/flashtool/images
	
	sudo umount /mnt
	sync
