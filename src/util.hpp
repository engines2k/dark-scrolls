#pragma once
#include <SDL2/SDL.h>
#include <cstddef>
#include <type_traits>

template <typename SelfType> class BitFlag {
  public:
    // A zero const can be implicitly converted to nullptr_t
    // Allows (a & b) == 0 to work
    bool operator==(std::nullptr_t null) const {
      return this->inner == 0;
    }
    bool operator!=(std::nullptr_t null) const {
      return !(*this == null);
    }
    bool operator==(const SelfType& other) const {
      return this->inner == other.inner;
    }
    bool operator!=(const SelfType& other) const {
      return !(*this == other);
    }
    constexpr SelfType operator|(const SelfType& other) const {
      SelfType ret = *this;
      ret.inner |= other.inner;
      return ret;
    }
    SelfType& operator|=(const SelfType& other) {
      *this = *this | other;
      return static_cast<SelfType&>(*this);
    }
    constexpr SelfType operator&(const SelfType& other) const {
      SelfType ret = *this;
      ret.inner &= other.inner;
      return ret;
    }
    SelfType& operator&=(const SelfType& other) {
      *this = *this & other;
      return static_cast<SelfType&>(*this);
    }
    constexpr SelfType operator^(const SelfType& other) const {
      SelfType ret = *this;
      ret.inner ^= other.inner;
      return ret;
    }
    SelfType& operator^=(const SelfType& other) {
      *this = *this ^ other;
      return static_cast<SelfType&>(*this);
    }
  protected:
    constexpr BitFlag<SelfType>() : inner(0) {}
    explicit constexpr BitFlag<SelfType>(int raw) : inner(raw) {}
    int inner;
};

class Empty {};

char scancode_to_char(SDL_Scancode code);
