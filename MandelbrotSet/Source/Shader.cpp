#include "Shader.h"

#include <string>
#include <sstream>
#include <fstream>
#include <assert.h>

#include <glad/glad.h>


Shader::Shader(const std::string &filepath)
{
	Load(filepath);
}
Shader::~Shader()
{
	glDeleteProgram(m_ShaderHandle);
}

void Shader::Load(const std::string &filepath)
{
	const auto [vertexSource, fragmentSource] = LoadShaderFromFile(filepath);
	m_ShaderHandle = CreateShader(vertexSource, fragmentSource);
}

void Shader::Bind() const
{
	glUseProgram(m_ShaderHandle);
}
void Shader::UnBind() const
{
	glUseProgram(0);
}

void Shader::SetFloat(const char *name, float value)
{
	glUniform1f(glGetUniformLocation(m_ShaderHandle, name), value);
}
void Shader::SetFloat2(const char *name, const vec2 &values)
{
	glUniform2f(glGetUniformLocation(m_ShaderHandle, name), values.x, values.y);
}
void Shader::SetFloat3(const char *name, const vec3 &values)
{
	glUniform3f(glGetUniformLocation(m_ShaderHandle, name), values.x, values.y, values.z);
}
void Shader::SetFloat4(const char *name, const vec4 &values)
{
	glUniform4f(glGetUniformLocation(m_ShaderHandle, name), values.x, values.y, values.z, values.w);
}

void Shader::SetDouble(const char *name, double value)
{
	glUniform1d(glGetUniformLocation(m_ShaderHandle, name), value);
}
void Shader::SetDouble2(const char *name, dvec2 values)
{
	glUniform2d(glGetUniformLocation(m_ShaderHandle, name), values.x, values.y);
}
void Shader::SetDouble3(const char *name, dvec3 values)
{
	glUniform3d(glGetUniformLocation(m_ShaderHandle, name), values.x, values.y, values.z);
}
void Shader::SetDouble4(const char *name, dvec4 values)
{
	glUniform4d(glGetUniformLocation(m_ShaderHandle, name), values.x, values.y, values.z, values.w);
}

void Shader::SetInt(const char *name, int value)
{
	glUniform1i(glGetUniformLocation(m_ShaderHandle, name), value);
}

void Shader::SetIntArray(const char *name, int *values, u32 count)
{
	glUniform1iv(glGetUniformLocation(m_ShaderHandle, name), count, values);
}
void Shader::SetFloatArray(const char *name, float *values, u32 count)
{
	glUniform1fv(glGetUniformLocation(m_ShaderHandle, name), count, values);
}
void Shader::SetDoubleArray(const char *name, double *values, u32 count)
{
	glUniform1dv(glGetUniformLocation(m_ShaderHandle, name), count, values);
}

std::pair<std::string, std::string> Shader::LoadShaderFromFile(const std::string &filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
		{
			ss[(int) type] << line << "\n";
		}
	}
	return { ss[(int) ShaderType::VERTEX].str(), ss[(int) ShaderType::FRAGMENT].str() };
}

u32 Shader::CreateShader(const std::string &vertexShader, const std::string &fragmentShader)
{
	u32 shaderProgram = glCreateProgram();
	std::string errorLog;

	u32 vertexProgram = TryCompileShader(GL_VERTEX_SHADER, vertexShader, errorLog);
	
	if (!vertexProgram)
	{
		LOG_ERROR("%s", errorLog.c_str());
		assert(false);
	}

	errorLog.clear();

	u32 fragmentProgram = TryCompileShader(GL_FRAGMENT_SHADER, fragmentShader, errorLog);
	if (!fragmentProgram)
	{
		LOG_ERROR("%s", errorLog.c_str());
		assert(false);
	}

	glAttachShader(shaderProgram, vertexProgram);
	glAttachShader(shaderProgram, fragmentProgram);

	glLinkProgram(shaderProgram);
	glValidateProgram(shaderProgram);

	glDeleteShader(vertexProgram);
	glDeleteShader(fragmentProgram);

	return shaderProgram;
}
u32 Shader::TryCompileShader(u32 shaderType, const std::string &shaderSource, std::string &errorLog)
{
	u32 shaderID = glCreateShader(shaderType);
	const char *src = shaderSource.c_str();

	glShaderSource(shaderID, 1, &src, nullptr);
	glCompileShader(shaderID);

	int result;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);

	if (!result)
	{
		int errorLength;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &errorLength);

		char *message = new char[errorLength * sizeof(char)];
		glGetShaderInfoLog(shaderID, errorLength, &errorLength, message);

		errorLog = "Failed to compile" + std::string((shaderType == GL_VERTEX_SHADER) ? "vertex" : "fragment") +
			" shader: \n" + std::string(message);

		glDeleteShader(shaderType);
		delete[] message;
		return 0;
	}
	return shaderID;
}