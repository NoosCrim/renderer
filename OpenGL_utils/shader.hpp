#pragma once
#include <cstdio>
#include <unordered_map>
#include <GL/glew.h>
#include <string>

namespace render
{
	bool ReadTxtFile(const char* const path, std::string &out);
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