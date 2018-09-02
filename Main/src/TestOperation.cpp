#include "TestOperation.h"
#include "Texture.h"
#include "Halide.h"

ComputeReturnInformation TestOperation::ComputeInternal(const std::vector<WeakTexture>& img)
{
	Halide::Var x,y;
	Halide::Func make_blank;
	make_blank(x, y) = Halide::cast<uint8_t>(255);
    
	SharedTexture b = std::make_shared<Texture>(make_blank.realize(128, 128));
	return { b, {b} };
}
