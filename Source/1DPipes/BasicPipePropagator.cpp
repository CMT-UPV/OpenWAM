#include "BasicPipePropagator.hpp"

void TBasicPipePropagator::Connect(TBasicPipe * pipe)
{
	FPipe = pipe;
	pipe->FPropagator = this;
}
