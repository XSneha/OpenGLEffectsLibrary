#include <fstream>
#include <string>
#include "../headers/Globals.h"



GLuint CompileShader(const char* shader_file_name, GLuint shader_type , const char* shaderSourceParam) {

	GLuint shader_object = 0;
	if (!shader_file_name || shader_file_name[0] == '\0')
	{
		fprintf(gpFile, "Invalid file name: %s\n", shader_file_name);
			return NULL;
	}

	std::ifstream file;
	file.open(shader_file_name);
	std::string fileContent = "";

	if (!file.good())
	{
		//TODO : fix file loading 
		fileContent = shaderSourceParam;
	}
	char* line = (char *)malloc(sizeof(char)*1000);


	const char* shaderSource = fileContent.c_str();
	shader_object = glCreateShader(shader_type);
	glShaderSource(shader_object, 1, (const GLchar**)&shaderSource, NULL);

	glCompileShader(shader_object);
	GLint infoLength = 0;
	GLint shaderCompiledStatus = 0;
	char* infoLog = NULL;

	glGetShaderiv(shader_object, GL_COMPILE_STATUS, &shaderCompiledStatus);

	if (shaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(shader_object, GL_INFO_LOG_LENGTH, &infoLength);
		if (infoLength > 0)
		{
			infoLog = (char*)malloc(infoLength);
			if (infoLog)
			{
				GLsizei written;
				glGetShaderInfoLog(shader_object, infoLength, &written, infoLog);

				fprintf(gpFile, "ShaderName: %s\n" , shader_file_name);
				fprintf(gpFile, "infoLog: %s\n" , infoLog);
				free(infoLog);
			}
		}
		shader_object = 0;
	}
	fprintf(gpFile, "Compilation complted ShaderName: %s\n", shader_file_name);

	return shader_object;
}

void LinkShader(GLuint spheresUpdaterShaderProgramObject) {
	//link shader
	glLinkProgram(spheresUpdaterShaderProgramObject);
	//linking error cheking code
	GLint infoLogLength = 0;
	GLint shaderCompilationStatus = 0;
	char* szBuffer = NULL;
	GLint shaderProgramLinkStatus = 0;
	szBuffer = NULL;
	glGetProgramiv(spheresUpdaterShaderProgramObject, GL_LINK_STATUS, &shaderProgramLinkStatus);
	if (shaderProgramLinkStatus == GL_FALSE) {
		glGetProgramiv(spheresUpdaterShaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0) {
			szBuffer = (char*)malloc(infoLogLength);
			if (szBuffer != NULL) {
				GLint written;
				glGetProgramInfoLog(spheresUpdaterShaderProgramObject, infoLogLength, &written, szBuffer);
				//print log to file
				fprintf(gpFile, "Sphere Updater shader logs : %s \n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);
			}
		}
	}
}
