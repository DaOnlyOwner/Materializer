#pragma once

#include "IOperation.h"
#include "Definitions.h"

class TestOperation : public BaseOperation<TestOperation>
{
public:
	ComputeReturnInformation ComputeInternal(const std::vector<WeakTexture>& img) override;
};