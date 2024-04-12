#ifndef RENDERER_H
#define RENDERER_H

#pragma once

#include <iostream>

#include "Shader.h"
#include "VertexArray.h"
#include "IndexBuffer.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

//Used to debug openGL calls
#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) ClearGLErrors();\
    x;\
    ASSERT(LogGLErrors(#x, __FILE__, __LINE__))

void ClearGLErrors();

bool LogGLErrors(const char* function, const char* file, int line);

class Renderer
{
private:
public:
    //clear currently bound buffer
    void Clear() const;

    //draw to currently bound buffer
    void Draw(unsigned int count) const;
};

#endif