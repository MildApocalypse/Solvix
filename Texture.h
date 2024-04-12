#pragma once
#include "Renderer.h"

//Abstraction for texture handling
class Texture
{
private:
	unsigned int m_RendererID; //Texture ID
	std::string m_FilePath; //path for texture
	unsigned char* m_LocalBuffer; //CPU texture data
	int m_Width, m_Height, m_BPP; //width, height, bytes per pixel
public:
	Texture(const std::string& path); //constructor for images from file
	Texture(unsigned int w, unsigned int h); //constructor for empty textures 
	~Texture();

	//bind/unbind
	void Bind(unsigned int slot = 0) const;
	void Unbind() const;
	void Delete() const;

	//read private variables
	inline int GetWidth() { return m_Width; }
	inline int GetHeight() { return m_Height; }
	inline unsigned int GetID() { return m_RendererID; }

};