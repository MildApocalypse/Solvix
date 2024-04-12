#include "FrameBuffer.h"

//Constructor
FrameBuffer::FrameBuffer(Texture* texture, const char *n)
	:m_RendererID(0), name(n)
{
	GLCall(glGenFramebuffers(1, &m_RendererID));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));

	GLCall(glViewport(0, 0, texture->GetWidth(), texture->GetHeight()))
	AttachTexture(texture->GetID());
}

FrameBuffer::FrameBuffer()
	:m_RendererID(0)
{
	GLCall(glGenFramebuffers(1, &m_RendererID));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
}

FrameBuffer::~FrameBuffer()
{
}

//Attach given texture to to color attachment 0
void FrameBuffer::AttachTexture(unsigned int TexID)
{
	Bind();

	GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, TexID, 0));

	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	GLCall(glDrawBuffers(1, DrawBuffers));


	if (!checkOK())
	{
		std::cout << name << " not complete" << std::endl;
	}
}

//check framebuffer complete
bool FrameBuffer::checkOK()
{
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Frame buffer status: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
		return false;
	}
	return true;
}

//bind
void FrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
}

//unbind
void FrameBuffer::UnBind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Delete() {
	GLCall(glDeleteBuffers(1, &m_RendererID));
}
