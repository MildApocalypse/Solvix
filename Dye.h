#ifndef DYE_H
#define DYE_H

#pragma once
#include "Influencer.h"

class Dye: public Influencer
{
private:
	float density;
public:
	Dye(type t, shape s, vec2 p, vec2 si, Shader* shade, std::string n, float dens);
	~Dye();

	void Draw(float width, float height);
	void Inject(float width, float height, float d, int textureType);
	void ShowVars(float uiWidth);

	inline float GetDensity() { return density; }
	inline void SetDensity(float d) { density = d; }
};

#endif

