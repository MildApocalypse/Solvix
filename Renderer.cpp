#include "Renderer.h"

void ClearGLErrors()
{
    while (glGetError() != GL_NO_ERROR);
}

//writes errors from openGL calls to console
bool LogGLErrors(const char* function, const char* file, int line)
{
    if (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "): " << function << " " <<
            file << ":" << line << std::endl;
        return false;
    }

    return true;
}

//clear currently bound buffer
void Renderer::Clear() const
{
    glClear(GL_COLOR_BUFFER_BIT);
}

//draw to currently bound buffer
void Renderer::Draw(unsigned int count) const
{
    GLCall(glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr));
}