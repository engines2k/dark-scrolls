#include "collide.hpp"

static void do_nothing_on_recoil(Pos here, ReactorCollideBox reactor) {}

const OnCollideRecoilFn DO_NOTHING_ON_COLLIDE_RECOIL = &do_nothing_on_recoil;
