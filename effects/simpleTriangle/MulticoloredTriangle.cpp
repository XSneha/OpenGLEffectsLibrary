#pragma once
#include "../../headers/Globals.h"

enum {
	SCS_ATTRIBUTE_POSITION = 0,
	SCS_ATTRIBUTE_COLOR,
	SCS_ATTRIBUTE_NORMAL,
	SCS_ATTRIBUTE_TEXTCOORD
};

//shader objects
GLint gVertexShaderObject;
GLint gFragmentShaderObject;
GLint gShaderProgramObject;

//shader binding objects
GLuint vao;
GLuint vboPos;
GLuint mvpMatrixUniform;

//color variables
GLuint vboColor;

void InitializeMulticoloredTriangle() {
	//Vertex shader
	//create shader
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//provide source object to shader
	const GLchar* vertexShaderSourceCode =
		"#version 440 core "\
		"\n"\
		"in vec4 vPosition;"\
		"in vec4 vColor;"\
		"uniform mat4 u_mvpMatrix;"\
		"out vec4 outColor;"\
		"void main(void)"\
		"{"\
		"gl_Position = u_mvpMatrix * vPosition;"\
		"outColor = vColor;"\
		"}";


	glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	//compile shader
	glCompileShader(gVertexShaderObject);

	//shader compilation error checking here
	GLint infoLogLength = 0;
	GLint shaderCompilationStatus = 0;
	char* szBuffer = NULL;
	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &shaderCompilationStatus);
	if (shaderCompilationStatus == GL_FALSE) {
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0) {
			szBuffer = (char*)malloc(infoLogLength);
			if (szBuffer != NULL) {
				GLint written;
				glGetShaderInfoLog(gVertexShaderObject, infoLogLength, &written, szBuffer);
				//print log to file
				fprintf(gpFile, "Vertex shader logs : %s \n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);
			}
		}
	}

	//fragment shader
	//create shader
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source object to shader
	const GLchar* fragmentShaderSourceCode =
		"#version 440 core "\
		"\n"\
		"out vec4 FragColor;"\
		"in vec4 outColor;"\
		"void main(void)"\
		"{"\
		"	FragColor = outColor;"\
		"}";

	glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	//compile shader
	glCompileShader(gFragmentShaderObject);
	//shader compilation error checking here
	infoLogLength = 0;
	shaderCompilationStatus = 0;
	szBuffer = NULL;
	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &shaderCompilationStatus);
	if (shaderCompilationStatus == GL_FALSE) {
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0) {
			szBuffer = (char*)malloc(infoLogLength);
			if (szBuffer != NULL) {
				GLint written;
				glGetShaderInfoLog(gFragmentShaderObject, infoLogLength, &written, szBuffer);
				//print log to file
				fprintf(gpFile, "Fragment shader logs : %s \n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);
			}
		}
	}

	//Shader program
	//create
	gShaderProgramObject = glCreateProgram();

	//attach vertext shader to shader program
	glAttachShader(gShaderProgramObject, gVertexShaderObject);
	//attach fragment shader to shader program 
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	//bind attribute with the one which we have specified with in in vertex shader
	glBindAttribLocation(gShaderProgramObject, SCS_ATTRIBUTE_POSITION, "vPosition");
	// 
	glBindAttribLocation(gShaderProgramObject, SCS_ATTRIBUTE_COLOR, "vColor");

	//link shader
	glLinkProgram(gShaderProgramObject);
	//linking error cheking code
	GLint shaderProgramLinkStatus = 0;
	szBuffer = NULL;
	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &shaderProgramLinkStatus);
	if (shaderProgramLinkStatus == GL_FALSE) {
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0) {
			szBuffer = (char*)malloc(infoLogLength);
			if (szBuffer != NULL) {
				GLint written;
				glGetProgramInfoLog(gShaderProgramObject, infoLogLength, &written, szBuffer);
				//print log to file
				fprintf(gpFile, "Fragment shader logs : %s \n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);
			}
		}
	}

	//get MVP uniform location 
	mvpMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_mvpMatrix");

	const GLfloat triangleVertices[] = {
		0.0f,1.0f,0.0f, //apex
		-1.0f,-1.0f,0.0f, //left bottom
		1.0f,-1.0f,0.0f //right bottom
	};

	const GLfloat triangleColor[] = {
		1.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,0.0f,1.0f
	};

	glGenVertexArrays(1, &vao); //?
	glBindVertexArray(vao); //?

	//push this vector array to shader
	glGenBuffers(1, &vboPos);
	glBindBuffer(GL_ARRAY_BUFFER, vboPos);//bind vbo with buffer array
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW); //add vertex data

	glVertexAttribPointer(SCS_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL); //map data 
	glEnableVertexAttribArray(SCS_ATTRIBUTE_POSITION); //enable the mapped buffer

	glBindBuffer(GL_ARRAY_BUFFER, 0);//unbind

	//push this color array to shader
	glGenBuffers(1, &vboColor);
	glBindBuffer(GL_ARRAY_BUFFER, vboColor);//bind vbo with buffer array
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColor), triangleColor, GL_STATIC_DRAW);//add color data

	glVertexAttribPointer(SCS_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SCS_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind

	glBindVertexArray(0);
}



void DisplayMulticoloredTriangle() {
	//start using opengl program object
	glUseProgram(gShaderProgramObject);

	//OpenGL drawing
	//set modelview and projection matrix to dentity 
	mat4 modelViewMatrix = mat4::identity();
	mat4 modelViewprojectionMatrix = mat4::identity();
	mat4 translationMatrix = mat4::identity();

	translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);

	modelViewMatrix = translationMatrix;
	modelViewprojectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewprojectionMatrix);
	//push to shader ^

	glBindVertexArray(vao); //bind vao

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0); //unbind vao

	//stop using program
	glUseProgram(0);
}

void UnInitializeMulticoloredTriangle() {
	if (vao) {
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}
	if (vboPos) {
		glDeleteBuffers(1, &vboPos);
		vboPos = 0;
	}
	if (vboColor) {
		glDeleteBuffers(1, &vboColor);
		vboColor = 0;
	}
	//safe release changes
	if (gShaderProgramObject) {
		glUseProgram(gShaderProgramObject);
		//shader cound to shaders attached to shader prog object
		GLsizei shaderCount;
		glGetProgramiv(gShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);
		GLuint* pShaders;
		pShaders = (GLuint*)malloc(sizeof(GLuint) * shaderCount);
		if (pShaders == NULL) {
			fprintf(gpFile, "Failed to allocate memory for pShaders");
			return;
		}
		//1st shader count is expected value we are passing and 2nd variable we are passing address in which
		//we are getting actual shader count currently attached to shader prog 
		glGetAttachedShaders(gShaderProgramObject, shaderCount, &shaderCount, pShaders);
		for (GLsizei i = 0; i < shaderCount; i++) {
			glDetachShader(gShaderProgramObject, pShaders[i]);
			glDeleteShader(pShaders[i]);
			pShaders[i] = 0;
		}
		free(pShaders);
		glDeleteProgram(gShaderProgramObject);
		gShaderProgramObject = 0;
		glUseProgram(0);
	}

	/*glDetachShader(gShaderProgramObject , gVertexShaderObject);
	glDetachShader(gShaderProgramObject, gFragmentShaderObject);
	glDeleteShader(gVertexShaderObject);
	gVertexShaderObject = 0;
	glDeleteShader(gFragmentShaderObject);
	gFragmentShaderObject = 0;
	glDeleteShader(gShaderProgramObject);
	gShaderProgramObject = 0;
	glUseProgram(0);
	*/
}

