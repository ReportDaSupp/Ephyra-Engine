/** \file OpenGLFrameBuffer.cpp */

#include "Ephyra_pch.h"
#include <glad/glad.h>
#include "Platform/OpenGl/OpenGlFrameBuffer.h"


namespace Engine
{

    OpenGLFrameBuffer::OpenGLFrameBuffer()
    {
        glGenFramebuffers(1, &m_FrameBufferID);
    }

    OpenGLFrameBuffer::~OpenGLFrameBuffer()
    {
        glDeleteFramebuffers(1, &m_FrameBufferID);
    }

    void OpenGLFrameBuffer::bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
    }

    void OpenGLFrameBuffer::unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLFrameBuffer::clear()
    {
        this->bind();
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        this->unbind();
    }

    void OpenGLFrameBuffer::attachColorTexture(uint32_t textureID, int attachmentIndex)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentIndex, GL_TEXTURE_2D, textureID, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLFrameBuffer::attachDepthTexture(uint32_t textureID)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }



}