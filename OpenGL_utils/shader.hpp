#pragma once
#include <cstdio>
#include <unordered_map>
#include <GL/glew.h>
#include <string>

namespace render
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
	static constexpr const char* ShaderTypeToStr(GLenum _type)
	{
		switch(_type)
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
		GLuint _name = 0;
		GLenum _type;

	public:
		Shader(){}
		Shader(GLenum _type, const char* const code);
		Shader(const Shader& other);
		Shader& operator=(const Shader& other);
		~Shader();
		
		static Shader FromFile(GLenum _type, const char* const path);
		inline GLenum type() const
		{
			return _type;
		}
		inline GLuint name() const
		{
			return _name;
		}
		inline operator GLuint() const
		{
			return _name;
		}
		inline operator bool() const
		{
			return _name;
		}
	};

	class ShaderProgram
	{
	private:
		unsigned int *instanceCount = nullptr;
		GLuint _name = 0;
	public:
		ShaderProgram(){}
		ShaderProgram(std::initializer_list<Shader> shaders);
		ShaderProgram(const ShaderProgram& other);
		ShaderProgram& operator=(const ShaderProgram& other);
		~ShaderProgram();

		void Use() const;

		inline GLuint name() const
		{
			return _name;
		}
		inline operator GLuint() const
		{
			return _name;
		}
		inline operator bool() const
		{
			return _name;
		}
	};
}
render::Shader::Shader(const Shader& other_) :
	_name(other_._name),
	_type(other_._type)
{
	if(instanceCount)
		++*instanceCount;
}
render::Shader::Shader(GLenum type_, const char* const code_):
	_type(type_)
{
	GLint compileStatus, logLen;
	_name = glCreateShader(_type);
	glShaderSource(_name, 1, &code_, NULL);
	glCompileShader(_name);
	glGetShaderiv(_name, GL_COMPILE_STATUS, &compileStatus);
	glGetShaderiv(_name, GL_INFO_LOG_LENGTH, &logLen);
	if (!compileStatus)
	{
		char log[logLen + 1];
		glGetShaderInfoLog(_name, logLen + 1, 0, log);

		std::fprintf(stderr, "%s shader compilation error: %s\n", ShaderTypeToStr(_type), log);
		return;
	}
}
render::Shader& render::Shader::operator=(const Shader& other_)
{
	if(instanceCount)
	{
		if(!--*instanceCount)
		{
			glDeleteShader(_name);
			delete instanceCount;
		}
	}

	instanceCount = other_.instanceCount;
	_name = other_._name;
	_type = other_._type;

	if(instanceCount)
		++*instanceCount;
	
	return *this;
}
render::Shader::~Shader()
{
	if (!instanceCount)
		return;
	if(--*instanceCount == 0)
	{
		glDeleteShader(_name);
		delete instanceCount;
	}
	
}

render::Shader render::Shader::FromFile(GLenum type_, const char* const path_)
{
	std::string code;
	if(!ReadTxtFile(path_, code))
	{
		std::fprintf(stderr, "Error: Reading Shader from: %s failed!\n", path_);
		return Shader();
	}
	return Shader(type_, code.c_str());
}

render::ShaderProgram::ShaderProgram(std::initializer_list<Shader> shaders_)
{
	_name = glCreateProgram();
	for(const Shader &shader : shaders_)
	{
		glAttachShader(_name, shader);
	}
	glLinkProgram(_name);

	GLint linkStatus, logLen;
	glGetProgramiv(_name, GL_LINK_STATUS, &linkStatus);
	glGetProgramiv(_name, GL_INFO_LOG_LENGTH, &logLen);
	if (!linkStatus)
	{
		char log[logLen + 1];
		glGetProgramInfoLog(_name, logLen + 1, 0, log);
		std::fprintf(stderr, "Shader Linking Error: %s\n", log);
		glDeleteProgram(_name);
		_name = 0;
	}

	if(_name)
	{
		instanceCount = new GLuint(1);
	}
}

render::ShaderProgram::ShaderProgram(const ShaderProgram& other_) : 
	_name(other_._name)
{
	if(instanceCount)
		++*instanceCount;
}

render::ShaderProgram& render::ShaderProgram::operator=(const ShaderProgram& other_)
{
	if(instanceCount)
	{
		if(!--*instanceCount)
		{
			glDeleteProgram(_name);
			delete instanceCount;
		}
	}

	instanceCount = other_.instanceCount;
	_name = other_._name;

	if(instanceCount)
		++*instanceCount;
	
	return *this;
}
render::ShaderProgram::~ShaderProgram()
{
	if (!instanceCount)
		return;
	if(--*instanceCount == 0)
	{
		glDeleteProgram(_name);
		delete instanceCount;
	}
	
}

void render::ShaderProgram::Use() const
{
	glUseProgram(_name);
}
