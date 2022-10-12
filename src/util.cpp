#include "util.hpp"

char scancode_to_char(SDL_Scancode code) {
  switch (code) {
      case SDL_SCANCODE_A:
        return 'A';
      case SDL_SCANCODE_B:
        return 'B';
      case SDL_SCANCODE_C:
        return 'C';
      case SDL_SCANCODE_D:
        return 'D';
      case SDL_SCANCODE_E:
        return 'E';
      case SDL_SCANCODE_F:
        return 'F';
      case SDL_SCANCODE_G:
        return 'G';
      case SDL_SCANCODE_H:
        return 'H';
      case SDL_SCANCODE_I:
        return 'I';
      case SDL_SCANCODE_J:
        return 'J';
      case SDL_SCANCODE_K:
        return 'K';
      case SDL_SCANCODE_L:
        return 'L';
      case SDL_SCANCODE_M:
        return 'M';
      case SDL_SCANCODE_N:
        return 'N';
      case SDL_SCANCODE_O:
        return 'O';
      case SDL_SCANCODE_P:
        return 'P';
      case SDL_SCANCODE_Q:
        return 'Q';
      case SDL_SCANCODE_R:
        return 'R';
      case SDL_SCANCODE_S:
        return 'S';
      case SDL_SCANCODE_T:
        return 'T';
      case SDL_SCANCODE_U:
        return 'U';
      case SDL_SCANCODE_V:
        return 'V';
      case SDL_SCANCODE_W:
        return 'W';
      case SDL_SCANCODE_X:
        return 'X';
      case SDL_SCANCODE_Y:
        return 'Y';
      case SDL_SCANCODE_Z:
        return 'Z';
      case SDL_SCANCODE_SPACE:
        return '_';
      default:
        return '\0';
  }
}
