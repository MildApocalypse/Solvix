#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#pragma once

#include <GL/glew.h>

#include "VertexBuffer.h"

class VertexBufferLayout;

class VertexArray
{
private:
	unsigned int m_RendererID;
	unsigned int layout_index = 0;

public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
	void AddBuffer(const VertexBuffer& vb, int elem, GLenum type = GL_FLOAT, GLenum norm = false);

	void Bind() const;
	void Unbind() const;
	void Delete();
};

#endif