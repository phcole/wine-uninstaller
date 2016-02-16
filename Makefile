EXE_COMPILER=i686-w64-mingw32-gcc
SHELL_NAME=uninstall.sh
EXE_NAME=app-uninstaller.exe
EXTRA_FLAGS=-m32

all: app-uninstaller
	
app-uninstaller: shelllinkwrapper.cpp registrywrapper.cpp uninstalllist.cpp main.cpp
	$(EXE_COMPILER) $? $(EXTRA_FLAGS) -static -mwindows -municode -lole32 -luuid -lstdc++ -lshlwapi -o$(EXE_NAME)

.PHONY: clean install uninstall

clean:
	rm -rfv $(EXE_NAME)

install:
	strip -s $(EXE_NAME)
	install -Dm755 $(SHELL_NAME) /opt/deepinwine/tools/$(SHELL_NAME)
	install -Dm644 $(EXE_NAME) /opt/deepinwine/tools/$(EXE_NAME)

uninstall:
	rm /opt/deepinwine/tools/$(SHELL_NAME)
	rm /opt/deepinwine/tools/$(EXE_NAME)
