#include "shader.hpp"

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
    Shader::Shader(const Shader& other_) :
	_name(other_._name),
	_type(other_._type)
    {
        if(instanceCount)
            ++*instanceCount;
    }
    Shader::Shader(GLenum type_, const char* const code_):
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
    Shader& Shader::operator=(const Shader& other_)
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
    Shader::~Shader()
    {
        if (!instanceCount)
            return;
        if(--*instanceCount == 0)
        {
            glDeleteShader(_name);
            delete instanceCount;
        }
        
    }

    Shader Shader::FromFile(GLenum type_, const char* const path_)
    {
        std::string code;
        if(!ReadTxtFile(path_, code))
        {
            std::fprintf(stderr, "Error: Reading Shader from: %s failed!\n", path_);
            return Shader();
        }
        return Shader(type_, code.c_str());
    }

    ShaderProgram::ShaderProgram(std::initializer_list<Shader> shaders_)
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

    ShaderProgram::ShaderProgram(const ShaderProgram& other_) : 
        _name(other_._name)
    {
        if(instanceCount)
            ++*instanceCount;
    }

    ShaderProgram& ShaderProgram::operator=(const ShaderProgram& other_)
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
    ShaderProgram::~ShaderProgram()
    {
        if (!instanceCount)
            return;
        if(--*instanceCount == 0)
        {
            glDeleteProgram(_name);
            delete instanceCount;
        }
        
    }

    void ShaderProgram::Use() const
    {
        glUseProgram(_name);
    }
}
