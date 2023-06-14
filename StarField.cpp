#pragma once
#include "headers/Globals.h"

/***
*Starfield : OpenGL_SuperBible_7th_Edition(Blue_Book)
***/
//shader objects
GLint sf_vertextShaderObj;
GLint sf_fragmentShaderObj;
GLint sf_shaderProgramObj;
//shader binding objects
GLuint sf_vao;
GLuint sf_vboPos;
GLuint sf_mvpMatrixUniform;
GLuint sf_timeUniform;
//color variables
GLuint sf_star_buffer;
GLuint sf_star_vao;
enum
{
	NUM_STARS = 2000
};
static inline float random_float();
void InitializeStarField() {
	//Vertex shader
	//create shader
	sf_vertextShaderObj = glCreateShader(GL_VERTEX_SHADER);

	//provide source object to shader
	const GLchar* vertexShaderSourceCode =
		"#version 440 core "\
		"\n"\
		"layout (location = 0) in vec4 position;"\
		"layout (location = 1) in vec4 color;"\
		"uniform float time;"\
		"uniform mat4 proj_matrix;"\
		"flat out vec4 starColor;"\
		"void main(void)"\
		"{"\
			"vec4 newVertex = position;"\
			"newVertex.z += time;"\
			"newVertex.z = fract(newVertex.z);"\
			"float size = (6.0 * newVertex.z * newVertex.z);"\
			"starColor = smoothstep(1.0, 7.0, size) * color;"\
			"newVertex.z = (999.9 * newVertex.z) - 1000.0;"\
			"gl_Position = proj_matrix * newVertex;"\
			"gl_PointSize = size;"\
		"}";

	sf_vertextShaderObj = CompileShader("***vertex shader****", GL_VERTEX_SHADER, vertexShaderSourceCode);

	//provide source object to shader
	const GLchar* fragmentShaderSourceCode =
		"#version 440 core"\
		"\n"\
		"layout (location = 0) out vec4 color; "\
		"flat in vec4 starColor;"\
		"void main(void)"\
		"{"\
			"color = starColor;"\
		"}";

	sf_fragmentShaderObj = CompileShader("***Fragment shader****", GL_FRAGMENT_SHADER, fragmentShaderSourceCode);

	sf_shaderProgramObj = glCreateProgram();
	//attach vertext shader to shader program
	glAttachShader(sf_shaderProgramObj, sf_vertextShaderObj);
	//attach fragment shader to shader program 
	glAttachShader(sf_shaderProgramObj, sf_fragmentShaderObj);
	LinkShader(sf_shaderProgramObj);

	sf_timeUniform = glGetUniformLocation(sf_shaderProgramObj, "time");
	sf_mvpMatrixUniform = glGetUniformLocation(sf_shaderProgramObj, "proj_matrix");

	glGenVertexArrays(1, &sf_star_vao);
	glBindVertexArray(sf_star_vao);
	struct star_t
	{
		vmath::vec3     position;
		vmath::vec3     color;
	};

	glGenBuffers(1, &sf_star_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, sf_star_buffer);
	glBufferData(GL_ARRAY_BUFFER, NUM_STARS * sizeof(star_t), NULL, GL_STATIC_DRAW);

	star_t* star = (star_t*)glMapBufferRange(GL_ARRAY_BUFFER, 0, NUM_STARS * sizeof(star_t), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	int i;

	for (i = 0; i < 1000; i++)
	{
		star[i].position[0] = (random_float() * 2.0f - 1.0f) * 100.0f;
		star[i].position[1] = (random_float() * 2.0f - 1.0f) * 100.0f;
		star[i].position[2] = random_float();
		star[i].color[0] = 0.8+random_float()* random_float() * 0.2f;
		star[i].color[1] = 0.8+random_float()* random_float() * 0.2f;
		star[i].color[2] = 0.8+random_float()* random_float() * 0.2f;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(star_t), NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(star_t), (void*)sizeof(vmath::vec3));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}
// Random number generator
static unsigned int seed = 0x13371337;
static inline float random_float()
{
	float res;
	unsigned int tmp;
	seed *= 16807;
	tmp = seed ^ (seed >> 4) ^ (seed << 15);
	*((unsigned int*)&res) = (tmp >> 9) | 0x3F800000;
	return (res - 1.0f);
}

static float currentTime = 100.0f;
void RenderStarField() {
	static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const GLfloat one[] = { 1.0f };
	float t = (float)currentTime;
	currentTime+=5.0f;
	float aspect = (float)WIN_WIDTH /(float)WIN_HEIGHT;
	mat4 mat4_perspective = vmath::perspective(45.0f, aspect, 0.1f, 100.0f);

	vmath::mat4 proj_matrix = vmath::perspective(50.0f,
		aspect,
		0.1f,
		1000.0f); 
	t *= 0.1f;
	t -= floor(t);

	vmath::mat4 translateZ = vmath::translate(0.0f,0.0f,-2.0f);
	vmath::mat4 modelView = translateZ ;
	vmath::mat4 mvp = proj_matrix * modelView ;

	//glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
	glClearBufferfv(GL_COLOR, 0, black);
	glClearBufferfv(GL_DEPTH, 0, one);

	glUseProgram(sf_shaderProgramObj);

	glUniform1f(sf_timeUniform, t);
	glUniformMatrix4fv(sf_mvpMatrixUniform, 1, GL_FALSE, mvp);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glBindVertexArray(sf_star_vao);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glDrawArrays(GL_POINTS, 0, NUM_STARS);
}


void UnInitializeStarField() {
	if (sf_star_vao) {
		glDeleteVertexArrays(1, &sf_star_vao);
		sf_star_vao = 0;
	}
	if (sf_star_buffer) {
		glDeleteBuffers(1, &sf_star_buffer);
		sf_star_buffer = 0;
	}

	//safe release changes
	if (sf_shaderProgramObj) {
		glUseProgram(sf_shaderProgramObj);
		//shader cound to shaders attached to shader prog object
		GLsizei shaderCount;
		glGetProgramiv(sf_shaderProgramObj, GL_ATTACHED_SHADERS, &shaderCount);
		GLuint* pShaders;
		pShaders = (GLuint*)malloc(sizeof(GLuint) * shaderCount);
		if (pShaders == NULL) {
			fprintf(gpFile, "Failed to allocate memory for pShaders");
			return;
		}
		//1st shader count is expected value we are passing and 2nd variable we are passing address in which
		//we are getting actual shader count currently attached to shader prog 
		glGetAttachedShaders(sf_shaderProgramObj, shaderCount, &shaderCount, pShaders);
		for (GLsizei i = 0; i < shaderCount; i++) {
			glDetachShader(sf_shaderProgramObj, pShaders[i]);
			glDeleteShader(pShaders[i]);
			pShaders[i] = 0;
		}
		free(pShaders);
		glDeleteProgram(sf_shaderProgramObj);
		sf_shaderProgramObj = 0;
		glUseProgram(0);
	}

}