EXE_COMPILER=i686-w64-mingw32-gcc
EXE_NAME="app-uninstaller.exe"
EXTRA_FLAGS=-m32

all: uninstaller
	
uninstaller: shelllinkwrapper.cpp registrywrapper.cpp uninstalllist.cpp main.cpp
	$(EXE_COMPILER) $? $(EXTRA_FLAGS) -mwindows -municode -lole32 -luuid -lstdc++ -lshlwapi -o$(EXE_NAME)

.PHONY: clean

clean:
	rm -rfv $(EXE_NAME)
