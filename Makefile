# Copyright (C) 2016 Deepin Technology Co., Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.

EXE_COMPILER=i686-w64-mingw32-gcc
SHELL_NAME=uninstall.sh
EXE_NAME=app-uninstaller.exe
EXTRA_FLAGS=-m32
DEB_PKG_NAME=deepin-wine-uninstaller
INTERM=build/

all: makedir app-uninstaller
	
$(INTERM)shelllinkwrapper.o: shelllinkwrapper.cpp
	$(EXE_COMPILER) -c $? $(EXTRA_FLAGS) -mwindows -municode -g -O2 -o$(INTERM)shelllinkwrapper.o

$(INTERM)registrywrapper.o: registrywrapper.cpp
	$(EXE_COMPILER) -c $? $(EXTRA_FLAGS) -mwindows -municode -g -O2 -o$(INTERM)registrywrapper.o

$(INTERM)uninstalllist.o: uninstalllist.cpp
	$(EXE_COMPILER) -c $? $(EXTRA_FLAGS) -mwindows -municode -g -O2 -o$(INTERM)uninstalllist.o

$(INTERM)main.o: main.cpp
	$(EXE_COMPILER) -c $? $(EXTRA_FLAGS) -mwindows -municode -g -O2 -o$(INTERM)main.o

app-uninstaller: $(INTERM)shelllinkwrapper.o $(INTERM)registrywrapper.o $(INTERM)uninstalllist.o $(INTERM)main.o
	$(EXE_COMPILER) $? $(EXTRA_FLAGS) -static -mwindows -municode -lole32 -luuid -lstdc++ -lshlwapi -g -O2 -o$(EXE_NAME)

.PHONY: makedir clean install uninstall builddeb strip-bin

makedir:
	if [ ! -d "$(INTERM)" ]; then mkdir $(INTERM); fi

clean:
	rm -rfv $(EXE_NAME) $(INTERM)*.o

strip-bin:
	strip -s $(EXE_NAME)

install: all strip-bin
	install -Dm755 $(SHELL_NAME) /opt/deepinwine/tools/$(SHELL_NAME)
	install -Dm644 $(EXE_NAME) /opt/deepinwine/tools/$(EXE_NAME)

uninstall:
	rm /opt/deepinwine/tools/$(SHELL_NAME)
	rm /opt/deepinwine/tools/$(EXE_NAME)

builddeb: all strip-bin
	cp debian/install.template debian/install
	sed -i "s#@CURRENT_PATH@#`pwd`#" debian/install
	fakeroot debian/build
	rm -r debian/$(DEB_PKG_NAME)* debian/install debian/files
