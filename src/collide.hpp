#pragma once
#include "util.hpp"
#include "pos.hpp"

struct ReactorCollideType;

struct ActivatorCollideType: public BitFlag<ActivatorCollideType> {
  public:
    using CounterPart = ReactorCollideType;
    constexpr ActivatorCollideType() : BitFlag<ActivatorCollideType>() {}
    explicit constexpr ActivatorCollideType(int raw) : BitFlag<ActivatorCollideType>(raw) {}
    constexpr ActivatorCollideType(const BitFlag<ActivatorCollideType>& inner) : BitFlag<ActivatorCollideType>(inner) {}

    static const ActivatorCollideType WALL;
    static const ActivatorCollideType HIT;
    ReactorCollideType activates() const;

    ActivatorCollideType operator&(const ActivatorCollideType& other) const {
      return this->BitFlag<ActivatorCollideType>::operator&(other);
    }
    ActivatorCollideType operator&(ReactorCollideType counter_part) const;
};

constexpr ActivatorCollideType ActivatorCollideType::WALL(0x1);
constexpr ActivatorCollideType ActivatorCollideType::HIT(0x2);

struct ReactorCollideType: public BitFlag<ReactorCollideType> {
  public:
    using CounterPart = ActivatorCollideType;
    constexpr ReactorCollideType() : BitFlag<ReactorCollideType>() {}
    explicit constexpr ReactorCollideType(int raw) : BitFlag<ReactorCollideType>(raw) {}
    constexpr ReactorCollideType(const BitFlag<ReactorCollideType>& inner) : BitFlag<ReactorCollideType>(inner) {}

    static const ReactorCollideType WALL;
    static const ReactorCollideType HURT;
    ActivatorCollideType activated_by() const;

    ReactorCollideType operator&(const ReactorCollideType& other) const {
      return this->BitFlag<ReactorCollideType>::operator&(other);
    }
    ReactorCollideType operator&(ActivatorCollideType counter_part) const;
};

constexpr ReactorCollideType ReactorCollideType::WALL(0x1);
constexpr ReactorCollideType ReactorCollideType::HURT(0x2);

inline ReactorCollideType ActivatorCollideType::activates() const {
  return ReactorCollideType(inner);
}

inline ActivatorCollideType ReactorCollideType::activated_by() const {
  return ActivatorCollideType(inner);
}

inline ActivatorCollideType ActivatorCollideType::operator&(ReactorCollideType counter_part) const {
  return *this & counter_part.activated_by();
}

inline ReactorCollideType ReactorCollideType::operator&(ActivatorCollideType counter_part) const {
  return *this & counter_part.activates();
}

template <typename CollideType> class CollideBox {
  public:
    CollideBox(CollideType type, int width, int height) {
      this->width = width;
      this->height = height;
    }

    bool collides_with(const Pos& here, const CollideBox<typename CollideType::CounterPart>& other, const Pos& there) {
      int this_x = here.x;
      int this_y = here.y;
      int other_x = there.x;
      int other_y = there.y;

      return (this->type & other.type) != 0 &&
        here.layer == there.layer &&
        this_x < other_x + other.width &&
        this_x + this->width > other_x &&
        this_y < other_y + other.height &&
        this_y + this->height > other_y;
    }
    CollideType type;
    int width;
    int height;
};

using ActivatorCollideBox = CollideBox<ActivatorCollideType>;
using ReactorCollideBox = CollideBox<ReactorCollideType>;

//Template functions are not fully checked unless they are called
inline void _check_activator_collide_box() {
  ReactorCollideBox react(ReactorCollideType(), 0, 0);
  ActivatorCollideBox activate(ActivatorCollideType(), 0, 0);
  Pos test_pos = {.layer = 0, .x = 0, .y = 0};
  react.collides_with(test_pos, activate, test_pos);
}
