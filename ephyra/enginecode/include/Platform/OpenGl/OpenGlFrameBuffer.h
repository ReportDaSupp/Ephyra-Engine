/** \file OpenGLFrameBuffer.h */
#pragma once

#include "Core/Rendering/API/Buffers/FrameBuffer.h"

namespace Engine
{
    class OpenGLFrameBuffer : public FrameBuffer {
    public:
        OpenGLFrameBuffer();
        virtual ~OpenGLFrameBuffer();

        virtual void bind() override;
        virtual void unbind() override;
        virtual void clear() override;
        virtual uint32_t getRenderID() const override { return m_FrameBufferID; }

        virtual void attachColorTexture(uint32_t textureID, int attachmentIndex) override;
        virtual void attachDepthTexture(uint32_t textureID) override;

    private:
        uint32_t m_FrameBufferID;
    };
}