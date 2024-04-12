#include "VertexArray.h"
#include "VertexBufferLayout.h"

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray()
{
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); ++i)
	{
		const auto& element = elements[i]; 
		GLCall(glEnableVertexAttribArray(layout_index));
		GLCall(glVertexAttribPointer(layout_index, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset));

		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
		++layout_index;
	}
}

void VertexArray::AddBuffer(const VertexBuffer& vb, int elem, GLenum type, GLenum norm)
{
	Bind();
	vb.Bind();
	GLCall(glEnableVertexAttribArray(layout_index));
	GLCall(glVertexAttribPointer( layout_index, elem, type, norm, 0, (void*)0 ));
	++layout_index;
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}

void VertexArray::Delete()
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID));
}