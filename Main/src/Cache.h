#pragma once
#include "Texture.h"
#include <cstdint>
#include <map>

typedef uint32_t handle;

class Cache {
public:
  virtual void Insert(Texture, handle &) = 0;
  virtual bool Get(handle, Texture &) = 0;
  virtual void KeepAlive(handle) = 0;
  virtual void DoNotKeepAlive(handle) = 0;
  virtual ~Cache() {}
};

class EndlessCache : public Cache {
public:
  std::map<handle, Texture> data_map;
  handle counter = 1;

  void Insert(Texture data, handle &d_handle) override {
    if(d_handle > 0)
      data_map[d_handle] = data;
    else {
      data_map[counter] = data;
      d_handle = counter++;
    }
  }

  bool Get(handle d_handle, Texture &out) override {
    auto it = data_map.find(d_handle);
    if(it != data_map.end()) {
      out = data_map[d_handle];
      return true;
    }
    return false;
  }

  void KeepAlive(handle h) override{};
  void DoNotKeepAlive(handle h) override{};

  void Debug() {
    for(auto &s : data_map)
      printf("Key: %i, Val: %p\n", s.first, &s.second);
  }
};
