#pragma once

#include "Halide.h"

class BlendGenerator : public Halide::Generator<BlendGenerator>
{
public:
	Input<Buffer<uint8_t>> Foreground{"Foreground",2};
	Input<Buffer<uint8_t>> Background{"Background",2};
	//Input<Buffer<uint8_t>> Opacity{ "Opacity",2 };
	Output<Buffer<int32_t>> BlendResult{ "BlendResult",2 };

private:
	Var x, y;

public:
	void generate()
	{
		BlendResult(x, y) = x + y;
	}

	void schedule()
	{
		BlendResult.vectorize(x, 16).parallel(y);
	}

};

