#pragma once
#include "headers/Globals.h"

/**
*Terrain reference
* https://github.com/damdoy/opengl_examples/tree/master/terrain_camera
* 
*/

enum {
	TR_ATTRIBUTE_POSITION = 0,
	TR_ATTRIBUTE_TEXCOORD,
	TR_ATTRIBUTE_COLOR,
	TR_ATTRIBUTE_NORMAL,
};

//shader objects
GLint TR_gVertexShaderObject;
GLint TR_gFragmentShaderObject;
GLint TR_gShaderProgramObject;

//shader binding objects
GLuint TR_vao;
GLuint TR_vboPos;
GLuint TR_mvpMatrixUniform;
GLuint keyPressUniform;

GLuint TR_vboTexture;
GLuint TR_vboColor;

//texture 
GLuint TR_texture;
GLuint TR_textureSamplerUnifirm;
GLuint TR_keyPressUniform;

void InitializeTerrain() {
	bool LoadGLTexture(GLuint * texture, TCHAR resourceId[]);

	//Vertex shader
	//create shader
	TR_gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//provide source object to shader
	const GLchar* TR_vertexShaderSourceCode =
		"#version 440 core "\
		"\n"\
		"in vec4 vPosition;"\
		"in vec2 vTexcord;"\
		"in vec4 vColor;"\
		"uniform mat4 u_mvpMatrix;"\
		"out vec2 outTexcord;"\
		"out vec4 outColor;"\
		"void main(void)"\
		"{"\
		"gl_Position = u_mvpMatrix * vPosition;"\
		"outTexcord = vTexcord;"\
		"outColor = vColor;"\
		"}";


	TR_gVertexShaderObject = CompileShader("***Terrain vertex shader****", GL_VERTEX_SHADER, TR_vertexShaderSourceCode);


	//fragment shader
	//create shader
	TR_gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source object to shader
	const GLchar* TR_fragmentShaderSourceCode =
		"#version 440 core "\
		"\n"\
		"in vec2 outTexcord;"\
		"in vec4 outColor;"\
		"uniform sampler2D u_texture_sampler;"\
		"out vec4 FragColor;"\
		"void main(void)"\
		"{"\
		"	{FragColor = texture(u_texture_sampler, outTexcord); }"\
		"}";

	TR_gFragmentShaderObject = CompileShader("***Terrain Fragment shader****", GL_FRAGMENT_SHADER, TR_fragmentShaderSourceCode);

	
	//Shader program
	//create
	TR_gShaderProgramObject = glCreateProgram();

	//attach vertext shader to shader program
	glAttachShader(TR_gShaderProgramObject, TR_gVertexShaderObject);
	//attach fragment shader to shader program 
	glAttachShader(TR_gShaderProgramObject, TR_gFragmentShaderObject);

	//bind attribute with the one which we have specified with in in vertex shader
	glBindAttribLocation(TR_gShaderProgramObject, TR_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(TR_gShaderProgramObject, TR_ATTRIBUTE_TEXCOORD, "vTexcord");
	glBindAttribLocation(TR_gShaderProgramObject, TR_ATTRIBUTE_COLOR, "vColor");


	//link shader
	LinkShader(TR_gShaderProgramObject);

	//get MVP uniform location 
	TR_mvpMatrixUniform = glGetUniformLocation(TR_gShaderProgramObject, "u_mvpMatrix");
	TR_textureSamplerUnifirm = glGetUniformLocation(TR_gShaderProgramObject, "u_texture_sampler"); ///verify once 

	const GLfloat quadVertices[] = {
		//front
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f
	};
	

	glGenVertexArrays(1, &TR_vao); //?
	glBindVertexArray(TR_vao); //?

	//push this vector array to shader
	glGenBuffers(1, &TR_vboPos);
	glBindBuffer(GL_ARRAY_BUFFER, TR_vboPos);//bind vbo with buffer array
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW); //add vertex data

	glVertexAttribPointer(TR_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL); //map data 
	glEnableVertexAttribArray(TR_ATTRIBUTE_POSITION); //enable the mapped buffer

	glBindBuffer(GL_ARRAY_BUFFER, 0);//unbind

	//white color array
	const GLfloat quadColor[] = {
		1.0f,0.0f,1.0f,
		1.0f,0.0f,0.0f,
		1.0f,0.0f,1.0f,
		1.0f,0.0f,0.0f
	};
	//push this color array to shader
	glGenBuffers(1, &TR_vboColor);
	glBindBuffer(GL_ARRAY_BUFFER, TR_vboColor);//bind vbo with buffer array
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadColor), quadColor, GL_DYNAMIC_DRAW);//add color data

	glVertexAttribPointer(TR_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(TR_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind

	glBindVertexArray(0);

	//push this color array to shader
	glGenBuffers(1, &TR_vboTexture);
	glBindBuffer(GL_ARRAY_BUFFER, TR_vboTexture);//bind vbo with buffer array
	//TODO add color data foe showing white colored square
	//glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * (4 * 2), NULL, GL_DYNAMIC_DRAW); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadColor), quadColor, GL_DYNAMIC_DRAW);//add color data

	glVertexAttribPointer(TR_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL); //map data 
	glEnableVertexAttribArray(TR_ATTRIBUTE_TEXCOORD); //enable the mapped buffer

	glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind

	glBindVertexArray(0);

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glEnable(GL_CULL_FACE);

	//texture
	LoadGLTexture(&TR_texture, MAKEINTRESOURCE(TERRAIN_BITMAP));
}


/*bool LoadGLTexture(GLuint* texture, TCHAR resourceId[]) {
	bool bResult = false;
	HBITMAP hBitmap = NULL;
	BITMAP bmp;

	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), resourceId, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	if (hBitmap) {
		fprintf(gpFile,"Image bitmap found");
		bResult = true;
		GetObject(hBitmap, sizeof(BITMAP), &bmp);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, bmp.bmBits);
		glGenerateMipmap(GL_TEXTURE_2D);
		DeleteObject(hBitmap);
	}
	else {
		fprintf(gpFile,"Failed to load texture");
	}
	return bResult;
}*/

void RenderTerrain() {
	//start using opengl program object
	glClearColor(0.0f,0.0f,1.0f,1.0f);
	glUseProgram(TR_gShaderProgramObject);

	//OpenGL drawing
	//set modelview and projection matrix to dentity 
	mat4 modelViewMatrix = mat4::identity();
	mat4 modelViewprojectionMatrix = mat4::identity();
	mat4 translationMatrix = mat4::identity();

	translationMatrix = vmath::translate(0.0f, 0.0f, -5.0f);

	modelViewMatrix = translationMatrix;
	modelViewprojectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(TR_mvpMatrixUniform, 1, GL_FALSE, modelViewprojectionMatrix);
	//push to shader ^

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TR_texture); //
	glUniform1i(TR_textureSamplerUnifirm, 0);

	glBindVertexArray(TR_vao); //bind vao

	glEnable(GL_TEXTURE_2D);
	//push this texture array to shader
	glGenBuffers(1, &TR_vboTexture);
	glBindBuffer(GL_ARRAY_BUFFER, TR_vboTexture);//bind vbo with buffer array
	//TODO add color data foe showing white colored square
	//complete texture
	GLfloat texCoord[8];
	texCoord[0] = 0.0f;
	texCoord[1] = 2.0f;
	texCoord[2] = 0.0f;
	texCoord[3] = 0.0f;
	texCoord[4] = 2.0f;
	texCoord[5] = 0.0f;
	texCoord[6] = 2.0f;
	texCoord[7] = 2.0f;


	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoord), texCoord, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(TR_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL); //map data 
	glEnableVertexAttribArray(TR_ATTRIBUTE_TEXCOORD); //enable the mapped buffer

	glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind
	
	
	glBindVertexArray(TR_vao); //bind vao

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindVertexArray(0); //unbind vao

	//stop using program
	glUseProgram(0);
}


void UnInitializeTerrain() {
	if (TR_vao) {
		glDeleteVertexArrays(1, &TR_vao);
		TR_vao = 0;
	}
	if (TR_vboPos) {
		glDeleteBuffers(1, &TR_vboPos);
		TR_vboPos = 0;
	}
	if (TR_vboColor) {
		glDeleteBuffers(1, &TR_vboColor);
		TR_vboColor = 0;
	}
	if (TR_vboTexture) {
		glDeleteBuffers(1, &TR_vboTexture);
		TR_vboTexture = 0;
	}
	//safe release changes
	if (TR_gShaderProgramObject) {
		glUseProgram(TR_gShaderProgramObject);
		//shader cound to shaders attached to shader prog object
		GLsizei shaderCount;
		glGetProgramiv(TR_gShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);
		GLuint* pShaders;
		pShaders = (GLuint*)malloc(sizeof(GLuint) * shaderCount);
		if (pShaders == NULL) {
			fprintf(gpFile, "Failed to allocate memory for pShaders");
			return;
		}
		//1st shader count is expected value we are passing and 2nd variable we are passing address in which
		//we are getting actual shader count currently attached to shader prog 
		glGetAttachedShaders(TR_gShaderProgramObject, shaderCount, &shaderCount, pShaders);
		for (GLsizei i = 0; i < shaderCount; i++) {
			glDetachShader(TR_gShaderProgramObject, pShaders[i]);
			glDeleteShader(pShaders[i]);
			pShaders[i] = 0;
		}
		free(pShaders);
		glDeleteProgram(TR_gShaderProgramObject);
		TR_gShaderProgramObject = 0;
		glUseProgram(0);
	}
}
