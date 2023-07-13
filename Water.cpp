#pragma once
#include "headers/Globals.h"

//Thin matrix

enum {
	WTR_ATTRIBUTE_POSITION = 0,
	WTR_ATTRIBUTE_COLOR,
	WTR_ATTRIBUTE_NORMAL,
	WTR_ATTRIBUTE_TEXTCOORD
};

//shader objects
GLint WTR_gVertexShaderObject;
GLint WTR_gFragmentShaderObject;
GLint WTR_gShaderProgramObject;

//shader binding objects
GLuint WTR_vao;
GLuint WTR_vboPos;
GLuint WTR_mvpMatrixUniform;


//color variables
GLuint WTR_vboColor;

void InitializeWater() {
	//Vertex shader
	//create shader
	WTR_gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//provide source object to shader
	const GLchar* WTR_vertexShaderSourceCode =
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


	WTR_gVertexShaderObject = CompileShader("***vertex shader****", GL_VERTEX_SHADER, WTR_vertexShaderSourceCode);


	//fragment shader
	//create shader
	WTR_gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source object to shader
	const GLchar* WTR_fragmentShaderSourceCode =
		"#version 440 core "\
		"\n"\
		"out vec4 FragColor;"\
		"in vec4 outColor;"\
		"void main(void)"\
		"{"\
		"	FragColor = outColor;"\
		"}";

	WTR_gFragmentShaderObject = CompileShader("***Fragment shader****", GL_FRAGMENT_SHADER, WTR_fragmentShaderSourceCode);


	//Shader program
	//create
	WTR_gShaderProgramObject = glCreateProgram();

	//attach vertext shader to shader program
	glAttachShader(WTR_gShaderProgramObject, WTR_gVertexShaderObject);
	//attach fragment shader to shader program 
	glAttachShader(WTR_gShaderProgramObject, WTR_gFragmentShaderObject);

	//bind attribute with the one which we have specified with in in vertex shader
	glBindAttribLocation(WTR_gShaderProgramObject, WTR_ATTRIBUTE_POSITION, "vPosition");
	// 
	glBindAttribLocation(WTR_gShaderProgramObject, WTR_ATTRIBUTE_COLOR, "vColor");

	//link shader
	LinkShader(WTR_gShaderProgramObject);

	//get MVP uniform location 
	WTR_mvpMatrixUniform = glGetUniformLocation(WTR_gShaderProgramObject, "u_mvpMatrix");

	const GLfloat triangleVertices[] = {
		0.0f,1.0f,0.0f, //apex
		-1.0f,-1.0f,0.0f, //left bottom
		1.0f,-1.0f,0.0f //right bottom
	};

	const GLfloat triangleColor[] = {
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f
	};

	glGenVertexArrays(1, &WTR_vao); //?
	glBindVertexArray(WTR_vao); //?

	//push this vector array to shader
	glGenBuffers(1, &WTR_vboPos);
	glBindBuffer(GL_ARRAY_BUFFER, WTR_vboPos);//bind vbo with buffer array
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW); //add vertex data

	glVertexAttribPointer(WTR_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL); //map data 
	glEnableVertexAttribArray(WTR_ATTRIBUTE_POSITION); //enable the mapped buffer

	glBindBuffer(GL_ARRAY_BUFFER, 0);//unbind

	//push this color array to shader
	glGenBuffers(1, &WTR_vboColor);
	glBindBuffer(GL_ARRAY_BUFFER, WTR_vboColor);//bind vbo with buffer array
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColor), triangleColor, GL_STATIC_DRAW);//add color data

	glVertexAttribPointer(WTR_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(WTR_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind

	glBindVertexArray(0);
}

void RenderWater() {
	//start using opengl program object
	glUseProgram(WTR_gShaderProgramObject);

	//OpenGL drawing
	//set modelview and projection matrix to dentity 
	mat4 modelViewMatrix = mat4::identity();
	mat4 modelViewprojectionMatrix = mat4::identity();
	mat4 translationMatrix = mat4::identity();

	translationMatrix = vmath::translate(-0.5f, 0.0f, -5.0f);

	modelViewMatrix = translationMatrix;
	modelViewprojectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(WTR_mvpMatrixUniform, 1, GL_FALSE, modelViewprojectionMatrix);
	//push to shader ^

	glBindVertexArray(WTR_vao); //bind vao

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0); //unbind vao

	//stop using program
	glUseProgram(0);
}


void UnInitializeWater() {
	if (WTR_vao) {
		glDeleteVertexArrays(1, &WTR_vao);
		WTR_vao = 0;
	}
	if (WTR_vboPos) {
		glDeleteBuffers(1, &WTR_vboPos);
		WTR_vboPos = 0;
	}
	if (WTR_vboColor) {
		glDeleteBuffers(1, &WTR_vboColor);
		WTR_vboColor = 0;
	}
	//safe release changes
	if (WTR_gShaderProgramObject) {
		glUseProgram(WTR_gShaderProgramObject);
		//shader cound to shaders attached to shader prog object
		GLsizei shaderCount;
		glGetProgramiv(WTR_gShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);
		GLuint* pShaders;
		pShaders = (GLuint*)malloc(sizeof(GLuint) * shaderCount);
		if (pShaders == NULL) {
			fprintf(gpFile, "Failed to allocate memory for pShaders");
			return;
		}
		//1st shader count is expected value we are passing and 2nd variable we are passing address in which
		//we are getting actual shader count currently attached to shader prog 
		glGetAttachedShaders(WTR_gShaderProgramObject, shaderCount, &shaderCount, pShaders);
		for (GLsizei i = 0; i < shaderCount; i++) {
			glDetachShader(WTR_gShaderProgramObject, pShaders[i]);
			glDeleteShader(pShaders[i]);
			pShaders[i] = 0;
		}
		free(pShaders);
		glDeleteProgram(WTR_gShaderProgramObject);
		WTR_gShaderProgramObject = 0;
		glUseProgram(0);
	}
}
