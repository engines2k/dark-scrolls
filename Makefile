SDLCFLAGS=-IC:/msys64/mingw64/include/SDL2 -Dmain=SDL_main
SDLLIBS=-LC:/msys64/mingw64/lib -lmingw32 -mwindows -lSDL2main -lSDL2 -lSDL2_ttf

dark_scrolls: main.cpp
	g++ -g -Wall $(SDLCFLAGS) -o $@ $? $(SDLLIBS)

# Cleans up executables.
clean:
	rm dark_scrolls.exe
