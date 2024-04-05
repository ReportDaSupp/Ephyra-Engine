/** \ file indirectBuffer.h */
#pragma once
#include <cstdint>

namespace Engine
{
	struct DrawElementsIndirectCommand
	{
		uint32_t drawCount;
		uint32_t instanceCount;
		uint32_t firstIndex;
		uint32_t firstVertex;
		uint32_t firstInstance;
	};

	class IndirectBuffer
	{
	public:
		virtual ~IndirectBuffer() = default;
		virtual inline uint32_t getID() const = 0;
		virtual inline uint32_t getCount() const = 0;
		virtual void edit(DrawElementsIndirectCommand* commands, uint32_t count, uint32_t offset) = 0;

		static IndirectBuffer* create(DrawElementsIndirectCommand* commands, uint32_t count);
	};
}
