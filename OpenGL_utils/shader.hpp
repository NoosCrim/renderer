#pragma once
#include <cstdio>
#include <unordered_map>
#include <GL/glew.h>
#include <string>

namespace renderer
{
	bool ReadTxtFile(const char* const path, std::string &out)
    {
        FILE* f = fopen(path, "rb");
        if(!f)
            return false;

        fseek(f, 0, SEEK_END);
        size_t size = ftell(f);
        rewind(f);

        std::string text(size, '\0');
        fread(text.data(), 1, size, f);
        fclose(f);

        out = std::move(text);

        return true;
    }
	static constexpr const char* ShaderTypeToStr(GLenum type)
	{
		switch(type)
		{
			case GL_VERTEX_SHADER: return "vertex";
			case GL_FRAGMENT_SHADER: return "fragment";
			case GL_GEOMETRY_SHADER: return "geometry";
			case  GL_TESS_CONTROL_SHADER: return "tesselation control";
			case GL_TESS_EVALUATION_SHADER: return "tesselation evaluation";
			case GL_COMPUTE_SHADER: return "compute";
			default: return "unknown";
		}
	}
	class Shader
	{
		unsigned int *instanceCount = nullptr;
		GLuint ID = 0;
		GLenum type;

	public:
		Shader(){}
		Shader(GLenum type, const char* const code);
		Shader(const Shader& other);
		Shader& operator=(const Shader& other);
		~Shader();
		
		static Shader FromFile(GLenum type, const char* const path);
		inline GLenum GetType() const
		{
			return type;
		}
		inline GLuint GetID() const
		{
			return ID;
		}
		inline operator GLuint() const
		{
			return ID;
		}
		inline operator bool() const
		{
			return ID;
		}
	};

	class ShaderProgram
	{
	private:
		unsigned int *instanceCount = nullptr;
		GLuint ID = 0;
	public:
		ShaderProgram(){}
		ShaderProgram(std::initializer_list<Shader> shaders);
		ShaderProgram(const ShaderProgram& other);
		ShaderProgram& operator=(const ShaderProgram& other);
		~ShaderProgram();

		void Use() const;

		inline GLuint GetID() const
		{
			return ID;
		}
		inline operator GLuint() const
		{
			return ID;
		}
		inline operator bool() const
		{
			return ID;
		}
	};
}
renderer::Shader::Shader(const Shader& other_) :
	ID(other_.ID),
	type(other_.type)
{
	if(instanceCount)
		++*instanceCount;
}
renderer::Shader::Shader(GLenum type_, const char* const code_):
	type(type_)
{
	GLint compileStatus, logLen;
	ID = glCreateShader(type);
	glShaderSource(ID, 1, &code_, NULL);
	glCompileShader(ID);
	glGetShaderiv(ID, GL_COMPILE_STATUS, &compileStatus);
	glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &logLen);
	if (!compileStatus)
	{
		char log[logLen + 1];
		glGetShaderInfoLog(ID, logLen + 1, 0, log);

		std::fprintf(stderr, "%s shader compilation error: %s\n", ShaderTypeToStr(type), log);
		return;
	}
}
renderer::Shader& renderer::Shader::operator=(const Shader& other_)
{
	if(instanceCount)
	{
		if(!--*instanceCount)
		{
			glDeleteShader(ID);
			delete instanceCount;
		}
	}

	instanceCount = other_.instanceCount;
	ID = other_.ID;
	type = other_.type;

	if(instanceCount)
		++*instanceCount;
	
	return *this;
}
renderer::Shader::~Shader()
{
	if (!instanceCount)
		return;
	if(--*instanceCount == 0)
	{
		glDeleteShader(ID);
		delete instanceCount;
	}
	
}

renderer::Shader renderer::Shader::FromFile(GLenum type_, const char* const path_)
{
	std::string code;
	if(!ReadTxtFile(path_, code))
	{
		std::fprintf(stderr, "Error: Reading Shader from: %s failed!\n", path_);
		return Shader();
	}
	return Shader(type_, code.c_str());
}

renderer::ShaderProgram::ShaderProgram(std::initializer_list<Shader> shaders_)
{
	ID = glCreateProgram();
	for(const Shader &shader : shaders_)
	{
		glAttachShader(ID, shader);
	}
	glLinkProgram(ID);

	GLint linkStatus, logLen;
	glGetProgramiv(ID, GL_LINK_STATUS, &linkStatus);
	glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &logLen);
	if (!linkStatus)
	{
		char log[logLen + 1];
		glGetProgramInfoLog(ID, logLen + 1, 0, log);
		std::fprintf(stderr, "Shader Linking Error: %s\n", log);
		glDeleteProgram(ID);
		ID = 0;
	}

	if(ID)
	{
		instanceCount = new GLuint(1);
	}
}

renderer::ShaderProgram::ShaderProgram(const ShaderProgram& other_) : 
	ID(other_.ID)
{
	if(instanceCount)
		++*instanceCount;
}

renderer::ShaderProgram& renderer::ShaderProgram::operator=(const ShaderProgram& other_)
{
	if(instanceCount)
	{
		if(!--*instanceCount)
		{
			glDeleteProgram(ID);
			delete instanceCount;
		}
	}

	instanceCount = other_.instanceCount;
	ID = other_.ID;

	if(instanceCount)
		++*instanceCount;
	
	return *this;
}
renderer::ShaderProgram::~ShaderProgram()
{
	if (!instanceCount)
		return;
	if(--*instanceCount == 0)
	{
		glDeleteProgram(ID);
		delete instanceCount;
	}
	
}

void renderer::ShaderProgram::Use() const
{
	glUseProgram(ID);
}
