#pragma once
#include <memory>
#include <vector>
#include "Definitions.h"

class Texture;
class BaseNode;

struct ComputeReturnInformation
{
	SharedTexture ShowcaseImg;
	std::vector<SharedTexture> Output;
};

// TODO: Get rid of the dynamic memory allocation
class IOperation {
public:
  virtual ComputeReturnInformation ComputeInternal(const std::vector<WeakTexture>& img ) = 0;
  virtual std::unique_ptr<IOperation> Clone() const = 0;
  virtual ~IOperation() {}
};

template <typename Derived>
class BaseOperation : public IOperation {
public:
	std::unique_ptr<IOperation> Clone() const {
		return std::make_unique<Derived>(static_cast<Derived const&>(*this));
	}
};