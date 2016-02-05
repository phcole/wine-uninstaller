EXE_COMPILER="i686-w64-mingw32-gcc"
EXE_NAME="app-uninstaller.exe"

all: uninstaller
	
uninstaller: shelllinkwrapper.cpp main.cpp
	$(EXE_COMPILER) $? -lole32 -luuid -lstdc++ -o$(EXE_NAME)

.PHONY: clean

clean:
	rm -rfv $(EXE_NAME)