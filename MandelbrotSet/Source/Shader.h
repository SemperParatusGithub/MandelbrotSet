#pragma once

#include "Core.h"


class Shader
{
public:
	Shader() = default;
	Shader(const std::string &filepath);
	~Shader();

	void Load(const std::string &filepath);

	void Bind() const;
	void UnBind() const;

	void SetFloat(const char *name, float value);
	void SetFloat2(const char *name, const vec2 &values);
	void SetFloat3(const char *name, const vec3 &values);
	void SetFloat4(const char *name, const vec4 &values);

	void SetDouble(const char *name, double value);
	void SetDouble2(const char *name, dvec2 values);
	void SetDouble3(const char *name, dvec3 values);
	void SetDouble4(const char *name, dvec4 values);

	virtual void SetInt(const char *name, int value);

	void SetIntArray(const char *name, int *values, u32 count);
	void SetFloatArray(const char *name, float *values, u32 count);
	void SetDoubleArray(const char *name, double *values, u32 count);


private:
	std::pair<std::string, std::string> LoadShaderFromFile(const std::string &filepath);

	u32 CreateShader(const std::string &vertexShader, const std::string &fragmentShader);
	u32 TryCompileShader(u32 shaderType, const std::string &shaderSource, std::string &errorLog);

private:
	u32 m_ShaderHandle = 0;
};