#include "CommandList.h"

std::vector<Jigsaw::Graphics::GPUBuffer*>& Jigsaw::Graphics::CommandList::GetLoadedBuffers() {
	return this->loading_buffers;
}
