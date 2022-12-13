#include "animation.hpp"
#include "game.hpp"
#include "media_manager.hpp"
#include "pos.hpp"
#include "collide.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <fstream>
#include <iostream>


AnimationFrame::AnimationFrame(int fn, std::string frame_path,
                               std::string sound_path = "NOSOUND") {
  this->frame_path = frame_path;
  this->sound_path = sound_path;
  this->sprite_offset = {0, 0};
  this->velocity = 0;
}

AnimationFrame::AnimationFrame(int fn, std::string frame_path,
                               std::string sound_path, Translation sprite_offset) {
  this->frame_path = frame_path;
  this->sound_path = sound_path;
  this->sprite_offset = {sprite_offset.x * SUBPIXELS_IN_PIXEL,
                         sprite_offset.y * SUBPIXELS_IN_PIXEL};
  this->velocity = 0;
}

AnimationFrame::AnimationFrame(int fn, std::string frame_path,
                               std::string sound_path, Translation sprite_offset,
                               int velocity) {
  this->frame_path = frame_path;
  this->sound_path = sound_path;
  this->sprite_offset = {sprite_offset.x * SUBPIXELS_IN_PIXEL,
                         sprite_offset.y * SUBPIXELS_IN_PIXEL};
  this->velocity = velocity;
}

AnimationFrame::~AnimationFrame() {}

Animation::Animation(Game &game, int nframes, bool l = false) : game(game) {
  loops = l;

  current_frame = 0;
  start_tick = game.frame_counter.rendered_frames;
  animation_l = nframes;
}

Animation::Animation(const Animation &a) : game(a.game) {
  current_frame = 0;
  start_tick = game.frame_counter.rendered_frames;
  frames = a.frames;
  animation_l = a.animation_l;
  loops = a.loops;
}

SDL_Texture *Animation::play() {
  int frame_index = ((int)game.frame_counter.rendered_frames - start_tick) % animation_l;
  // Set keyframe
  if (frames.find(frame_index) != frames.end()) {
    current_frame = frame_index;
    // Set reactors
    if(frames[current_frame]->reactors.size() > 0)
      cur_reactors = frames[current_frame]->reactors;
    else {
      cur_reactors = default_reactors;
      //std::cout << default_reactors.size() <<std::endl;
    }

    // Play sound
    std::string p = frames[current_frame]->sound_path;
    if (strcmp(p.c_str(), "NOSOUND")) {
      Mix_Chunk *s = game.media.readWAV(p);
      Mix_PlayChannel(-1, s, 0);
    }
  }

  SDL_Texture *tex = game.media.readTexture(frames[current_frame]->frame_path);
  return tex;
}

void Animation::set_frame(int fn, std::string fpath, std::string spath) {
  frames.emplace(fn, std::make_shared<AnimationFrame>(fn, fpath, spath));
}

void Animation::set_frame(int fn, std::string fpath, std::string spath,
                          Translation sprite_offset) {
  frames.emplace(fn, std::make_shared<AnimationFrame>(fn, fpath, spath, sprite_offset));
}

void Animation::set_frame(int fn, std::string fpath, std::string spath,
                          Translation sprite_offset, int velocity) {
  frames.emplace(fn, std::make_shared<AnimationFrame>(fn, fpath, spath, sprite_offset, velocity));
}


AnimationFrame Animation::frame() { 
  return *frames[current_frame];
}

std::vector<ReactorCollideBox> Animation::current_reactors() { return cur_reactors; }

std::vector<ActivatorCollideBox> Animation::frame_activators() {
  std::vector<ActivatorCollideBox> empty;
  if (frames.find(current_frame) != frames.end())
    return frames[current_frame]->activators;
  return empty;
}

void Animation::add_activator(int fn, ActivatorCollideBox box) {
  frames[fn]->activators.push_back(box);
}

void Animation::add_reactor(int fn, ReactorCollideBox box) {
  frames[fn]->reactors.push_back(box);
}

void Animation::reset() {
  current_frame = 0;
  start_tick = game.frame_counter.rendered_frames;
}

bool Animation::is_over() {
  if (!loops)
    return (int)game.frame_counter.rendered_frames - start_tick >= animation_l - 1;
  return false;
}

FrameData::FrameData(Game &game): game(game) {}

ActivatorCollideBox FrameData::readActivatorBox(json j) {
  ActivatorCollideType collide_type;
  for(auto &type : j["type"]) {
    if(type == "WALL") 
      collide_type = collide_type | ActivatorCollideType::WALL;
    if(type == "HIT_EVIL") 
      collide_type = collide_type | ActivatorCollideType::HIT_EVIL;
    if(type == "HIT_GOOD")
      collide_type = collide_type | ActivatorCollideType::HIT_GOOD;
    if(type == "HIT_ALL")
      collide_type = collide_type | ActivatorCollideType::HIT_ALL;
    if(type == "INTERACT")
      collide_type = collide_type | ActivatorCollideType::INTERACT;
  }
  json dims = j["dimensions"];
    ActivatorCollideBox activator = ActivatorCollideBox(
    collide_type,
    int(dims[0])*SUBPIXELS_IN_PIXEL,
    int(dims[2])*SUBPIXELS_IN_PIXEL,
    int(dims[1])*SUBPIXELS_IN_PIXEL,
    int(dims[3])*SUBPIXELS_IN_PIXEL);

    if(j.find("damage") != j.end()) {
      CollideDamageProps damage;
      damage.hp_delt = int(j["damage"]);
      activator.damage = damage;
    }
    return activator;
}

ReactorCollideBox FrameData::readReactorBox(json j) {
  ReactorCollideType collide_type;
  for(auto &type : j["type"]) {
    if(type == "WALL") 
      collide_type = collide_type | ReactorCollideType::WALL;
    if(type == "HURT_BY_EVIL") 
      collide_type = collide_type | ReactorCollideType::HURT_BY_EVIL;
    if(type == "HURT_BY_GOOD")
      collide_type = collide_type | ReactorCollideType::HURT_BY_GOOD;
    if(type == "HURT_BY_ANY")
      collide_type = collide_type | ReactorCollideType::HURT_BY_ANY;
    if(type == "INTERACTABLE")
      collide_type = collide_type | ReactorCollideType::INTERACTABLE;
  }
  json dims = j["dimensions"];
  return ReactorCollideBox(
    collide_type,
    int(dims[0])*SUBPIXELS_IN_PIXEL,
    int(dims[2])*SUBPIXELS_IN_PIXEL,
    int(dims[1])*SUBPIXELS_IN_PIXEL,
    int(dims[3])*SUBPIXELS_IN_PIXEL);
}

void FrameData::readAnimations(const char *filepath, const char *spritename) {
  std::ifstream animation_file(filepath);           // Check if filepath exists
  if (!animation_file) {
    std::cerr << "Error opening animation file." << std::endl;
    abort();
  }

  json data = json::parse(animation_file);   // Read in to JSON instance

  json sprite_data;
  for(auto &sd : data) {                     // Find correct animation object
    if(sd["spritename"] == spritename) {
      sprite_data = sd;
      break;
    }
  }
  
  if(sprite_data.size() == 0) {
    std::cerr << "No framedata for " << spritename << " found in " << filepath << std::endl;
    abort();
  }

  std::vector<ReactorCollideBox> default_reactors;
  std::vector<ActivatorCollideBox> default_activators;
  // Read in default activators, reactors 
  if(sprite_data.find("default_reactors") != sprite_data.end()) {
    for(auto &reactor : sprite_data["default_reactors"]) {
       default_reactors.push_back(readReactorBox(reactor));
    }
  }
  if(sprite_data.find("default_activators") != sprite_data.end()) {
    for(auto &activator : sprite_data["default_activators"]) {
      default_activators.push_back(readActivatorBox(activator));
    }
  }


  // Read in each animation
  for(auto &animation : sprite_data["animations"]) {
    // Create new Animation instance
    Animation new_anim(game, animation["length"], animation["loops"]);
    new_anim.name = animation["name"];
    new_anim.loops = animation["loops"];

    // Set start frame for animation looping (optional)
    if(animation.find("loop_start") != animation.end()) {
      new_anim.loop_start = animation["loop_start"];
    }

    // Set immobility for animation (optional)
    if(animation.find("immobilizes") != animation.end()) {
      new_anim.immobilizes = animation["immobilizes"];
    }

    // Read in keyframes data
    for(auto frame : animation["keyframes"]) {
      std::string img_path = std::string("data/sprite/") + frame["img"].get<std::string>();
      // Create animation frame with required values
      int fn = int(frame["frame"]);    
      std::shared_ptr new_frame = std::make_shared<AnimationFrame>(AnimationFrame(fn, img_path));
      new_frame->frame_number = fn; // Not sure why this is necesary, but it is.

      // Sound for frame (optional)
      if(frame.find("sound") != frame.end()) {
        std::string sound_path = ("data/sound/" + frame["sound"].get<std::string>()).c_str();
        new_frame->sound_path = sound_path;
      }
      // Sprite offset for frame (optional)
      if(frame.find("imgoffset") != frame.end()) {
        Translation offset = {frame["imgoffset"][0], frame["imgoffset"][1]};
        new_frame->sprite_offset = offset;
      }
      // Set invulnerability for frame (optional)
      if(frame.find("invulnerable") != frame.end()) {
        new_frame->invulnerable = frame["invulnerable"];
      }
      // Set activators for frame (optional)
      if(frame.find("activators") != frame.end()) {
        for(auto &activator : frame["activators"]) {
          new_frame->activators.push_back(readActivatorBox(activator));
        }
      }
      // Set reactors for frame (optional)
      if(frame.find("reactors") != frame.end()) {
        for(auto &reactor : frame["reactors"]) {
          new_frame->reactors.push_back(readReactorBox(reactor));
        }
      }
      // Set velocity for frame (optional)
      if(frame.find("velocity") != frame.end()) {
          new_frame->velocity = int(frame["velocity"]) * SUBPIXELS_IN_PIXEL;
      }

      new_anim.frames.emplace(new_frame->frame_number, new_frame);
    }
    new_anim.default_reactors = default_reactors;
    new_anim.default_activators = default_activators;
    animations.push_back(new_anim);
  }
  // Add default collisions to all animations
  for(auto &anim : animations) {
    anim.default_reactors = default_reactors;
    anim.default_activators = default_activators;
  }
}
