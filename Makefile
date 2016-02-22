EXE_COMPILER=i686-w64-mingw32-gcc
SHELL_NAME=uninstall.sh
EXE_NAME=app-uninstaller.exe
EXTRA_FLAGS=-m32
DEB_PKG_NAME=deepin-wine-uninstaller

all: app-uninstaller
	
shelllinkwrapper.o: shelllinkwrapper.cpp
	$(EXE_COMPILER) -c $? $(EXTRA_FLAGS) -mwindows -municode -g -O2

registrywrapper.o: registrywrapper.cpp
	$(EXE_COMPILER) -c $? $(EXTRA_FLAGS) -mwindows -municode -g -O2

uninstalllist.o: uninstalllist.cpp
	$(EXE_COMPILER) -c $? $(EXTRA_FLAGS) -mwindows -municode -g -O2

main.o: main.cpp
	$(EXE_COMPILER) -c $? $(EXTRA_FLAGS) -mwindows -municode -g -O2

app-uninstaller: shelllinkwrapper.o registrywrapper.o uninstalllist.o main.o
	$(EXE_COMPILER) $? $(EXTRA_FLAGS) -static -mwindows -municode -lole32 -luuid -lstdc++ -lshlwapi -g -O2 -o$(EXE_NAME)

.PHONY: clean install uninstall builddeb strip-bin

clean:
	rm -rfv $(EXE_NAME)

strip-bin:
	strip -s $(EXE_NAME)

install: app-uninstaller strip-bin
	install -Dm755 $(SHELL_NAME) /opt/deepinwine/tools/$(SHELL_NAME)
	install -Dm644 $(EXE_NAME) /opt/deepinwine/tools/$(EXE_NAME)

uninstall:
	rm /opt/deepinwine/tools/$(SHELL_NAME)
	rm /opt/deepinwine/tools/$(EXE_NAME)

builddeb: app-uninstaller strip-bin
	cp debian/install.template debian/install
	sed -i "s#@CURRENT_PATH@#`pwd`#" debian/install
	fakeroot debian/build
	rm -r debian/$(DEB_PKG_NAME)* debian/install debian/files
