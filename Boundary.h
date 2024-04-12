#ifndef BOUNDARY_H
#define BOUNDARY_H

#pragma once

#include "Influencer.h"
class Boundary : public Influencer
{
private:

public:
	Boundary(type t, shape s, vec2 p, vec2 si, Shader* shade, std::string n);
	~Boundary();

	void Draw(float width, float height);
	void Inject(float width, float height, float d, int textureType);
	void ShowVars(float uiWidth);
};

#endif // !BOUNDARY_H
