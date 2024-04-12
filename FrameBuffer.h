#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#pragma once
#include "Texture.h"

//abstraction for framebuffer functions
class FrameBuffer {
private:
	unsigned int m_RendererID;
	const char *name = " ";
public:
	FrameBuffer(Texture* texture, const char* n);
	FrameBuffer();
	~FrameBuffer();

	//attach texture to framebuffer
	void AttachTexture(unsigned int TexID);

	//check framebuffer is complete
	bool checkOK();

	//bind and unbind
	void Bind();
	void UnBind();

	void Delete();
};

#endif