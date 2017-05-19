# liblightmodbus - a lightweight, multiplatform Modbus library
# Copyright (C) 2016 Jacek Wieczorek <mrjjot@gmail.com>

# This file is part of liblightmodbus.

# Liblightmodbus is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# Liblightmodbus is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# This is makefile for liblightmodbus - a lightweight Modbus library
# It creates object files and a static library, so you can link it to your project
# If you want, liblightmodbus can be installed on your system using 'make install', or later removed using 'make uninstall'
# If you want to build it yourself for platform like AVR, this is not the makefile you are looking for
# Beside, files building order is cruicial - modules first, then base, and finally linking, otherwise things will go wrong

# This makefile is part of source deb package

compileHeader = \
	echo "[\033[32;1mcompiling\033[0m] \033[03m$(1)\033[0m" >&2

linkHeader = \
	echo "[\033[33;1mlinking\033[0m] \033[03m$(1)\033[0m" >&2

errorHeader = \
	echo "[\033[31;1merror\033[0m] \033[03m$(1)\033[0m" >&2

cleanHeader = \
	echo "[\033[36;1mcleaning up\033[0m] \033[03m$(1)\033[0m" >&2

infoHeader = \
	echo "[\033[36;1minfo\033[0m] \033[03m$(1)\033[0m" >&2

CC = gcc
CFLAGS = -Wall -I./include

LD = ld
LDFLAGS =

MASTERFLAGS =
SLAVEFLAGS =

MODULES =
MMODULES = master-registers master-coils
SMODULES = slave-registers slave-coils

STATICMEM_SRESPONSE =
STATICMEM_MREQUEST =
STATICMEM_MDATA =

ifndef STATICMEM_SRESPONSE
$(warning "dynamic memory for slave response")
else
$(warning "static memory for slave response")
CFLAGS += -DLIGHTMODBUS_STATIC_MEM_SLAVE_RESPONSE=$(STATICMEM_SRESPONSE)
endif

ifndef STATICMEM_MREQUEST
$(warning "dynamic memory for master request")
else
$(warning "static memory for master request")
CFLAGS += -DLIGHTMODBUS_STATIC_MEM_MASTER_REQUEST=$(STATICMEM_MREQUEST)
endif

ifndef STATICMEM_MDATA
$(warning "dynamic memory for master data")
else
$(warning "static memory for master data")
CFLAGS += -DLIGHTMODBUS_STATIC_MEM_MASTER_DATA=$(STATICMEM_MDATA)
endif

ifndef MMODULES
$(warning "MMODULES not specified!")
else
MODULES += $(MMODULES) master-base master-link
endif

ifndef SMODULES
$(warning "SMODULES not specified!")
else
MODULES += $(SMODULES) slave-base slave-link
endif

all: $(MODULES)
all: clean FORCE core
	$(call linkHeader,full object file)
	echo "LINKING Library full object file (obj/lightmodbus.o)" >> build.log
	$(LD) $(LDFLAGS) -r obj/core.o obj/master.o obj/slave.o -o obj/lightmodbus.o
	$(call linkHeader,static library file)
	echo "CREATING Static library file (lib/liblightmodbus.a)" >> build.log
	ar -cvq lib/liblightmodbus.a obj/lightmodbus.o
	ar -t  lib/liblightmodbus.a
	echo -n "\n\nBuild success - " >> build.log
	date >> build.log
	$(call infoHeader,build finished successfully)

install:
	$(call infoHeader,installing liblightmodbus)
	-mkdir -p $(DESTDIR)/usr
	-mkdir -p $(DESTDIR)/usr/include
	cp -R include/lightmodbus $(DESTDIR)/usr/include
	-mkdir $(DESTDIR)/usr/lib
	cp -R lib/liblightmodbus.a $(DESTDIR)/usr/lib

uninstall:
	$(call infoHeader,removing liblightmodbus)
	rm -rf $(DESTDIR)/usr/include/lightmodbus
	rm -f $(DESTDIR)/usr/lib/liblightmodbus.a

FORCE:
	$(call infoHeader,starting build)
	-touch build.log
	echo -n "Architecture: " > build.log
	arch >> build.log
	echo -n "Build started - " >> build.log
	date >> build.log
	echo -n "\n\n" >> build.log
	-mkdir obj
	-mkdir obj/slave
	-mkdir obj/master
	-mkdir lib

clean:
	$(call infoHeader,cleaning up build environment)
	-find . -name "*.gch" -type f -delete
	-rm -rf smodules.tmp mmodules.tmp
	-rm -rf obj
	-rm -rf lib
	-rm -f build.log
	-rm -f *.gcno
	-rm -f *.gcda
	-rm -f *.o
	-rm -f coverage-test
	-rm -f coverage-test.log
	-rm -f valgrind.xml
	-rm -f massif.out

################################################################################

core: src/core.c include/lightmodbus/core.h
	$(call compileHeader,core module)
	echo "COMPILING Core module (obj/core.o)" >> build.log
	$(CC) $(CFLAGS) -c src/core.c -o obj/core.o

master-base: src/master.c include/lightmodbus/master.h
	$(call compileHeader,master base module)
	echo "COMPILING Master module (obj/master/mbase.o)" >> build.log
	$(CC) $(CFLAGS) `cat mmodules.tmp` -c src/master.c -o obj/master/mbase.o

master-registers: src/master/mpregs.c include/lightmodbus/master/mpregs.h src/master/mbregs.c include/lightmodbus/master/mbregs.h
	$(call compileHeader,master registers module)
	echo " -DLIGHTMODBUS_MASTER_REGISTERS=1" >> mmodules.tmp
	echo "COMPILING Master registers module (obj/master/mregisters.o)" >> build.log
	$(CC) $(CFLAGS) -c src/master/mpregs.c -o obj/master/mpregs.o
	$(CC) $(CFLAGS) -c src/master/mbregs.c -o obj/master/mbregs.o

master-coils: src/master/mpcoils.c include/lightmodbus/master/mpcoils.h src/master/mbcoils.c include/lightmodbus/master/mbcoils.h
	$(call compileHeader,master coils module)
	echo " -DLIGHTMODBUS_MASTER_COILS=1" >> mmodules.tmp
	echo "COMPILING Master coils module (obj/master/mcoils.o)" >> build.log
	$(CC) $(CFLAGS) -c src/master/mpcoils.c -o obj/master/mpcoils.o
	$(CC) $(CFLAGS) -c src/master/mbcoils.c -o obj/master/mbcoils.o

master-link:
	$(call linkHeader,master modules)
	echo "LINKING Master module (obj/master.o)" >> build.log
	$(LD) $(LDFLAGS) -r obj/master/*.o -o obj/master.o

slave-base: src/slave.c include/lightmodbus/slave.h
	$(call compileHeader,slave base module)
	echo "COMPILING Slave module (obj/slave/sbase.o)" >> build.log
	$(CC) $(CFLAGS) `cat smodules.tmp` -c src/slave.c -o obj/slave/sbase.o

slave-registers: src/slave/sregs.c include/lightmodbus/slave/sregs.h
	$(call compileHeader,slave registers module)
	echo " -DLIGHTMODBUS_SLAVE_REGISTERS=1" >> smodules.tmp
	echo "COMPILING Slave registers module (obj/slave/sregisters.o)" >> build.log
	$(CC) $(CFLAGS) -c src/slave/sregs.c -o obj/slave/sregs.o

slave-coils: src/slave/scoils.c include/lightmodbus/slave/scoils.h
	$(call compileHeader,slave coils module)
	echo " -DLIGHTMODBUS_SLAVE_COILS=1" >> smodules.tmp
	echo "COMPILING Slave coils module (obj/slave/scoils.o)" >> build.log
	$(CC) $(CFLAGS) -c src/slave/scoils.c -o obj/slave/scoils.o

slave-link:
	$(call linkHeader,slave modules)
	echo "LINKING Slave module (obj/slave.o)" >> build.log
	$(LD) $(LDFLAGS) -r obj/slave/*.o -o obj/slave.o
