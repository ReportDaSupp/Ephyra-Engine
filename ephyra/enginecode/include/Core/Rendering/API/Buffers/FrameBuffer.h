#pragma once

#include <cstdint>

namespace Engine
{
    class FrameBuffer {
    public:
        virtual ~FrameBuffer() = default;

        virtual void bind() = 0;
        virtual void unbind() = 0;
        virtual void clear() = 0;
        virtual uint32_t getRenderID() const = 0;

        // may need to add texture format, depth, stencil
        virtual void attachColorTexture(uint32_t textureID, int attachmentIndex) = 0;
        virtual void attachDepthTexture(uint32_t textureID) = 0;

        static FrameBuffer* create(/* still need to add params here */);
    };
}