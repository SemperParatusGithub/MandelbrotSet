#include "Shader.h"

#include <string>
#include <sstream>
#include <fstream>
#include <utility>
#include <vector>

#include <glad/glad.h>


Shader::Shader(const std::string &filepath)
{
	Load(filepath);
}
Shader::~Shader()
{
	if (m_ShaderHandle != 0)
		glDeleteProgram(m_ShaderHandle);
}

Shader::Shader(Shader &&other) noexcept
	: m_ShaderHandle(other.m_ShaderHandle)
{
	other.m_ShaderHandle = 0;
}
Shader &Shader::operator=(Shader &&other) noexcept
{
	if (this != &other)
	{
		if (m_ShaderHandle != 0)
			glDeleteProgram(m_ShaderHandle);
		m_ShaderHandle = other.m_ShaderHandle;
		other.m_ShaderHandle = 0;
	}
	return *this;
}

void Shader::Load(const std::string &filepath)
{
	const auto [vertexSource, fragmentSource] = LoadShaderFromFile(filepath);
	if (vertexSource.empty() || fragmentSource.empty())
	{
		LOG_ERROR("Refusing to compile shader '%s' (missing vertex or fragment stage)", filepath.c_str());
		return;
	}

	if (m_ShaderHandle != 0)
		glDeleteProgram(m_ShaderHandle);
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
	if (!stream.is_open())
	{
		LOG_ERROR("Could not open shader file '%s'", filepath.c_str());
		return { {}, {} };
	}

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
		else if (type != ShaderType::NONE)
		{
			ss[(int) type] << line << "\n";
		}
		// Lines that appear before any `#shader` marker are silently ignored
		// (so things like a leading BOM or comment can't UB into ss[-1]).
	}
	return { ss[(int) ShaderType::VERTEX].str(), ss[(int) ShaderType::FRAGMENT].str() };
}

u32 Shader::CreateShader(const std::string &vertexShader, const std::string &fragmentShader)
{
	std::string errorLog;

	u32 vertexProgram = TryCompileShader(GL_VERTEX_SHADER, vertexShader, errorLog);
	if (!vertexProgram)
	{
		LOG_ERROR("%s", errorLog.c_str());
		return 0;
	}

	errorLog.clear();
	u32 fragmentProgram = TryCompileShader(GL_FRAGMENT_SHADER, fragmentShader, errorLog);
	if (!fragmentProgram)
	{
		LOG_ERROR("%s", errorLog.c_str());
		glDeleteShader(vertexProgram);
		return 0;
	}

	u32 shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexProgram);
	glAttachShader(shaderProgram, fragmentProgram);
	glLinkProgram(shaderProgram);

	int linkStatus = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
	if (!linkStatus)
	{
		int logLength = 0;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<char> message(logLength > 0 ? (size_t) logLength : 1, '\0');
		glGetProgramInfoLog(shaderProgram, logLength, &logLength, message.data());
		LOG_ERROR("Failed to link shader program:\n%s", message.data());

		glDetachShader(shaderProgram, vertexProgram);
		glDetachShader(shaderProgram, fragmentProgram);
		glDeleteShader(vertexProgram);
		glDeleteShader(fragmentProgram);
		glDeleteProgram(shaderProgram);
		return 0;
	}

	glDetachShader(shaderProgram, vertexProgram);
	glDetachShader(shaderProgram, fragmentProgram);
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

	int result = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);

	if (!result)
	{
		int errorLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &errorLength);

		std::vector<char> message(errorLength > 0 ? (size_t) errorLength : 1, '\0');
		glGetShaderInfoLog(shaderID, errorLength, &errorLength, message.data());

		errorLog = std::string("Failed to compile ") +
			((shaderType == GL_VERTEX_SHADER) ? "vertex" : "fragment") +
			" shader:\n" + std::string(message.data());

		glDeleteShader(shaderID);
		return 0;
	}
	return shaderID;
}
