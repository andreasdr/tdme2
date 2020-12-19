#include <tdme/engine/subsystems/rendering/ObjectBuffer.h>

#include <vector>

#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/Engine.h>
#include <tdme/utilities/ByteBuffer.h>
#include <tdme/utilities/Console.h>

using tdme::engine::subsystems::rendering::ObjectBuffer;

using std::vector;

using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;
using tdme::utilities::ByteBuffer;
using tdme::utilities::Console;

vector<ByteBuffer*> ObjectBuffer::byteBuffers;

void ObjectBuffer::initialize() {
	auto renderer = Engine::renderer;
	byteBuffers.resize(renderer->isSupportingMultithreadedRendering() == true?Engine::getThreadCount():1);
}

void ObjectBuffer::dispose() {
	for (auto byteBuffer: byteBuffers) {
		delete byteBuffer;
	}
	byteBuffers.clear();
}

ByteBuffer* ObjectBuffer::getByteBuffer(void* context, int32_t bytes)
{
	auto renderer = Engine::renderer;
	auto contextIdx = renderer->getContextIndex(context);
	if (byteBuffers[contextIdx] == nullptr || bytes > byteBuffers[contextIdx]->getCapacity()) {
		if (byteBuffers[contextIdx] != nullptr) {
			delete byteBuffers[contextIdx];
		}
		byteBuffers[contextIdx] = ByteBuffer::allocate(bytes);
	} else {
		byteBuffers[contextIdx]->clear();
	}
	return byteBuffers[contextIdx];
}
