#ifndef SHADER_H
#define SHADER_H

#pragma once

#include <string>
#include <unordered_map>

#include <GL/glew.h>
#include "glm/glm.hpp"

//hold strings for vertex and fragment code
struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
	
};

//abstraction for creating and maintaining shader programs
class Shader
{
private:
	std::string m_Filepath; //path for .shader file
	unsigned int m_RendererID; //program ID
	std::unordered_map<std::string, int> m_UniformLocationCache; //

	ShaderProgramSource ParseShader(const std::string& filepath); //read shader
	unsigned int GetUniformLocation(const std::string& name); //get uniform
	unsigned int CompileShader(unsigned int type, const std::string& source); //compile shader
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader); //create shader
	
public:
	//constructor
	Shader(const std::string& filepath);
	~Shader();

	//bind/unbind
	void Bind() const;
	void UnBind() const;

	//set shader uniforms
	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform1fv(const std::string& name, int count, float* value);
	void SetUniform2f(const std::string& name, float v0, float v1);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

	void Delete();

};

#endif