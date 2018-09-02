#pragma once

#include <cstdint>
#include <memory>
#include <tuple>

struct NodeInformation;

typedef uint64_t NodeHandle;

template <typename T>
struct Port : public std::pair<NodeHandle, unsigned int> {
	Port(NodeHandle handle, unsigned int index) : std::pair<NodeHandle,unsigned int>(handle,index){}
	Port(){}

  bool validate() {
    return first != 0; // Make sure we are still okay...
  }
};

struct Input;
struct Output;
class Texture;

typedef Port<Input> PortInputHandle;
typedef Port<Output> PortOutputHandle;
typedef std::shared_ptr<NodeInformation> SharedNodeInformation;
typedef std::shared_ptr<Texture> SharedTexture;
typedef std::weak_ptr<Texture> WeakTexture;
