OUTPUT = rawdrv

3DODEV	= C:/3DODev
ARMDEV	= C:/ARM251
DEBUG	= 1

STACKSIZE 	= 8192

CC		= wine $(ARMDEV)/bin/armcc
AS 		= wine $(ARMDEV)/bin/armasm
LD		= wine $(ARMDEV)/bin/armlink
LIB             = wine $(ARMDEV)/bin/armlib
RM		= rm
MODBIN          = modbin
MAKEBANNER	= wine $(3DODEV)/bin/MakeBanner

CCFLAGS = -bigend -za1 -zps0 -zi4 -fa -fh -fx -fpu none -arch 3 -apcs '/softfp/nofp/swstackcheck'
ASFLAGS = -bigend -fpu none -arch 3 -apcs '/softfp/nofp/swstackcheck'
INCPATH	= -I $(3DODEV)/includes/
LDFLAGS = -aif -reloc -ro-base 0x00 -dupok -remove -nodebug -verbose
LIBPATH	= $(3DODEV)/libs

LIBS 	= $(LIBPATH)/clib.lib \
	  $(LIBPATH)/cstartup.o

SRC_S = $(wildcard src/*.s)
SRC_C = $(wildcard src/*.c)

OBJ  += $(SRC_S:src/%.s=build/.src/%.s.o)
OBJ  += $(SRC_C:src/%.c=build/.src/%.c.o)

all: mkdirs signed-rawdrv rawio.lib

rawdrv.unsigned: build/kernel_calls.s.o build/rawdrv.c.o
	$(LD) $(LDFLAGS) $? $(LIBS) -o build/$@

rawio.lib: build/rawio.c.o
	$(LIB) -c build/$@ $<

signed-rawdrv: rawdrv.unsigned
	$(MODBIN) --stack=$(STACKSIZE) --flags=0x2 --sign=3do --name=rawdrv build/rawdrv.unsigned build/rawdrv

build/kernel_calls.s.o: src/kernel_calls.s
	$(AS) $(INCPATH) $(ASFLAGS) $< -o $@

build/rawdrv.c.o: src/rawdrv.c src/kernel_calls.s src/kernel_calls.h
	$(CC) $(INCPATH) $(CCFLAGS) -c $< -o $@

build/rawio.c.o: src/rawio.c
	$(CC) $(INCPATH) $(CCFLAGS) -c $< -o $@

clean:
	$(RM) -fv build/*

mkdirs:
	mkdir -p build

.PHONY: mkdirs
