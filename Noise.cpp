#pragma once
#include "headers/Globals.h"

/*
*
* Reference
* https://thebookofshaders.com/13/
* 
*/

enum {
	NOISE_ATTRIBUTE_POSITION = 0,
	NOISE_ATTRIBUTE_COLOR,
	NOISE_ATTRIBUTE_NORMAL,
	NOISE_ATTRIBUTE_TEXTCOORD
};

//shader objects
GLint NOISE_gVertexShaderObject;
GLint NOISE_gFragmentShaderObject;
GLint NOISE_gShaderProgramObject;

//shader binding objects
GLuint NOISE_vao;
GLuint NOISE_vboPos;
GLuint NOISE_mvpMatrixUniform;

//color variables
GLuint NOISE_vboColor;

/* Uniforms */
static GLuint u_noise_sampler;
static GLuint u_time;
static GLuint u_resolution;
//static GLuint u_orange_color;
//static GLuint u_green_color;
//static GLuint u_alpha;

/* Transformation matrices uniform */
static GLuint u_model_matrix;
static GLuint u_view_matrix;
static GLuint u_projection_matrix;

GLuint NOISE_texture;


void InitializeNoise() {
	//Vertex shader
	//create shader
	NOISE_gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	bool LoadGLTexture(GLuint * texture, TCHAR resourceId[]);

	//provide source object to shader
	const GLchar* NOISE_vertexShaderSourceCode =
		"#version 460 core"\
		"\n"\
		"in vec4 v_position;"\
		"uniform mat4 u_model_matrix; "\
		"uniform mat4 u_view_matrix;"\
		"uniform mat4 u_projection_matrix;"\
		"void main(void)"\
		"{"\
		"	gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * v_position;"\
		"}";


	NOISE_gVertexShaderObject = CompileShader("***vertex shader for noise****", GL_VERTEX_SHADER, NOISE_vertexShaderSourceCode);


	//fragment shader
	//create shader
	NOISE_gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source object to shader
	const GLchar* NOISE_fragmentShaderSourceCode = 
		"#version 460 core"\
		"\n"\
		"#define OCTAVES 6											  "\
		"\n"\
		"uniform vec2 u_resolution;									  "\
		"uniform float u_time;										  "\
		"out vec4 FragColor;										  "\
		"															  "\
		"float random (in vec2 st) {								  "\
		"    return fract(sin(dot(st.xy,							  "\
		"                         vec2(12.9898,78.233)))*			  "\
		"        43758.5453123);									  "\
		"}															  "\
		"															  "\
		"float noise (in vec2 st) {									  "\
		"    vec2 i = floor(st);									  "\
		"    vec2 f = fract(st);									  "\
		"															  "\
		"    float a = random(i);									  "\
		"    float b = random(i + vec2(1.0, 0.0));					  "\
		"    float c = random(i + vec2(0.0, 1.0));					  "\
		"    float d = random(i + vec2(1.0, 1.0));					  "\
		"															  "\
		"    vec2 u = f * f * (3.0 - 2.0 * f);						  "\
		"															  "\
		"    return mix(a, b, u.x) +								  "\
		"            (c - a)* u.y * (1.0 - u.x) +					  "\
		"            (d - b) * u.x * u.y;							  "\
		"}															  "\
		"															  "\
		"float fbm (in vec2 st) {									  "\
		"    float value = 0.0;										  "\
		"    float amplitude = .5;									  "\
		"    float frequency = 0.;									  "\
		"    for (int i = 0; i < OCTAVES; i++) {					  "\
		"        value += amplitude * noise(st);					  "\
		"        st *= 2.;											  "\
		"        amplitude *= .5;									  "\
		"    }														  "\
		"    return value;											  "\
		"}															  "\
		"															  "\
		"void main() {												  "\
		"vec2 st = gl_FragCoord.xy/u_resolution.xy*3.;			"\
		"  vec3 color = vec3(0.0);								"\
		"															"\
		"  vec2 q = vec2(0.);										"\
		"  q.x = fbm( st + 0.00*u_time);							"\
		"  q.y = fbm( st + vec2(1.0));							"\
		"															"\
		"  vec2 r = vec2(0.);										"\
		"  r.x = fbm( st + 1.0*q + vec2(1.7,9.2)+ 0.15*u_time );	"\
		"  r.y = fbm( st + 1.0*q + vec2(8.3,2.8)+ 0.126*u_time);	"\
		"															"\
		"  float f = fbm(st+r);									"\
		"															"\
		"  color = mix(vec3(0.101961,0.619608,0.666667),			"\
		"              vec3(0.666667,0.166667,0.498039),			"\
		"              clamp((f*f)*4.0,0.0,1.0));					"\
		"															"\
		"  color = mix(color,										"\
		"              vec3(0,0,0.164706),						"\
		"              clamp(length(q),0.0,1.0));					"\
		"															"\
		"  color = mix(color,										"\
		"              vec3(0.666667,1,1),						"\
		"              clamp(length(r.x),0.0,1.0));				"\
		"															  "\
		"    FragColor = vec4((f*f*f+.6*f*f+.5*f)*color,0.5);					  "\
		"}";

	/*const GLchar* NOISE_fragmentShaderSourceCode =
		"#version 460 core"\
		"in vec2 out_texcoord;"\
		"out vec4 FragColor;"\

		"uniform float u_time;"\
		"uniform sampler2D u_noise_sampler;"\

		"uniform vec3 u_orange_color;"\
		"uniform vec3 u_green_color;"\

		"uniform int u_apply_texture;"\
		"uniform float u_alpha;"\

		"#define NUM_OCTAVES 5"\

		"float random(in vec2 uv)"\
		"{"\
		"return fract(sin(dot(uv.xy, vec2(12.9898, 78.233))) * 43758.5453123);"\
		"}"\

		"float noise(in vec2 uv)"\
		"{"\
		"vec2 i = floor(uv);"\
		"vec2 f = fract(uv);"\

		"// Four corners in 2D of a tile"\
		"float a = random(i);"\
		"float b = random(i + vec2(1.0, 0.0));"\
		"float c = random(i + vec2(0.0, 1.0));"\
		"float d = random(i + vec2(1.0, 1.0));"\

		"vec2 u = f * f * (3.0 - 2.0 * f);"\

		"return mix(a, b, u.x) +"\
		"	(c - a) * u.y * (1.0 - u.x) +"\
		"	(d - b) * u.x * u.y;"\
		"}"\

		"float fbm(in vec2 uv)"\
		"{"\
		"float v = 0.0;"\
		"float a = 0.5;"\
		"vec2 shift = vec2(100.0);"\

		"// Rotate to reduce axial bias"\
		"mat2 rot = mat2(cos(0.5), sin(0.5),-sin(0.5), cos(0.50));"\
		"for (int i = 0; i < NUM_OCTAVES; ++i)"\
		"{"\
		"	v += a * noise(uv);"\
		"	uv = rot * uv * 2.0 + shift;"\
		"	a *= 0.5;"\
		"}"\
		"return v;"\
		"}"\
		"void main()"\
		"{"\
		"vec3 color = vec3(1.0);"\
		"vec3 whiteColor = vec3(1.0, 1.0, 1.0);"\
		"vec2 q = vec2(0.0);"\
		"q.x = fbm(out_texcoord + 0.0 * u_time);"\
		"q.y = fbm(out_texcoord + vec2(1.0));"\
		"vec2 r = vec2(0.0);"\
		"r.x = fbm(out_texcoord + 1.0 * q + vec2(1.7, 9.2) + 0.15 * u_time);"\
		"r.y = fbm(out_texcoord + 1.0 * q + vec2(8.3, 2.8) + 0.126 * u_time);"\

		"float f = fbm(out_texcoord + r);"\

		"//vec2 newTexcoord = out_texcoord * 0.5 + 0.5;"\
		"vec3 flagColor = texture(u_noise_sampler, newTexcoord).xyz;"\

		"color = mix(u_orange_color, whiteColor, smoothstep(1.0, 0.0, out_texcoord.y));"\
		"color = mix(color, flagColor, smoothstep(0.5, -0.5, out_texcoord.y));"\
		"color = mix(color, u_green_color, smoothstep(0.0, -1.0, out_texcoord.y));"\

		"//color = color * (f * f * f + 0.6 * f * f + 0.5 * f);"\
		"color = color * (0.5 * f * f + 0.5 * f + 0.4);"\

		"FragColor = vec4(1.0,0.0,0.0,0.5);"\
		"}";*/

	NOISE_gFragmentShaderObject = CompileShader("***Fragment shader for noise****", GL_FRAGMENT_SHADER, NOISE_fragmentShaderSourceCode);

	//Shader program
	//create
	NOISE_gShaderProgramObject = glCreateProgram();

	//attach vertext shader to shader program
	glAttachShader(NOISE_gShaderProgramObject, NOISE_gVertexShaderObject);
	//attach fragment shader to shader program 
	glAttachShader(NOISE_gShaderProgramObject, NOISE_gFragmentShaderObject);

	//bind attribute with the one which we have specified with in in vertex shader
	glBindAttribLocation(NOISE_gShaderProgramObject, NOISE_ATTRIBUTE_POSITION, "v_position");

	//link shader
	LinkShader(NOISE_gShaderProgramObject);

	/* Get Uniform Locations */
	u_model_matrix = glGetUniformLocation(NOISE_gShaderProgramObject, "u_model_matrix");
	u_view_matrix = glGetUniformLocation(NOISE_gShaderProgramObject, "u_view_matrix");
	u_projection_matrix = glGetUniformLocation(NOISE_gShaderProgramObject, "u_projection_matrix");
	u_time = glGetUniformLocation(NOISE_gShaderProgramObject, "u_time");
	u_resolution = glGetUniformLocation(NOISE_gShaderProgramObject, "u_resolution");

	LoadGLTexture(&NOISE_texture, MAKEINTRESOURCE(TERRAIN_BITMAP));
	const GLfloat triangleVertices[] = {
		0.0f,1.0f,0.0f, //apex
		-1.0f,-1.0f,0.0f, //left bottom
		1.0f,-1.0f,0.0f //right bottom
	};

	glGenVertexArrays(1, &NOISE_vao); //?
	glBindVertexArray(NOISE_vao); //?

	//push this vector array to shader
	glGenBuffers(1, &NOISE_vboPos);
	glBindBuffer(GL_ARRAY_BUFFER, NOISE_vboPos);//bind vbo with buffer array
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW); //add vertex data

	glVertexAttribPointer(NOISE_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL); //map data 
	glEnableVertexAttribArray(NOISE_ATTRIBUTE_POSITION); //enable the mapped buffer

	glBindBuffer(GL_ARRAY_BUFFER, 0);//unbind

	glBindVertexArray(0);
}


void RenderNoise() {
	//start using opengl program object
	//OpenGL drawing
	//set modelview and projection matrix to dentity 
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	//mat4 projectionMatrix = mat4::identity();
	mat4 translationMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f,0.0f,-2.0f) * vmath::scale(80.0f,80.0f,10.0f);
	static float time = 0.05f;
	time += 0.005f;
	GLfloat alpha = 0.9f;

	glUseProgram(NOISE_gShaderProgramObject);

	glUniformMatrix4fv(u_model_matrix, 1, GL_FALSE, translationMatrix);
	glUniformMatrix4fv(u_view_matrix, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(u_projection_matrix, 1, GL_FALSE, perspectiveProjectionMatrix);

	glUniform1f(u_time, time);
	vec2 resolution = vec2(1500.0f, 1500.0f);
	glUniform2fv(u_resolution, 1,resolution);

	glBindVertexArray(NOISE_vao); //bind vao
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0); //unbind vao
	glUseProgram(0);
}


void UnInitializeNoise() {
	if (NOISE_vao) {
		glDeleteVertexArrays(1, &NOISE_vao);
		NOISE_vao = 0;
	}
	if (NOISE_vboPos) {
		glDeleteBuffers(1, &NOISE_vboPos);
		NOISE_vboPos = 0;
	}
	if (NOISE_vboColor) {
		glDeleteBuffers(1, &NOISE_vboColor);
		NOISE_vboColor = 0;
	}
	//safe release changes
	if (NOISE_gShaderProgramObject) {
		glUseProgram(NOISE_gShaderProgramObject);
		//shader cound to shaders attached to shader prog object
		GLsizei shaderCount;
		glGetProgramiv(NOISE_gShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);
		GLuint* pShaders;
		pShaders = (GLuint*)malloc(sizeof(GLuint) * shaderCount);
		if (pShaders == NULL) {
			fprintf(gpFile, "Failed to allocate memory for pShaders");
			return;
		}
		//1st shader count is expected value we are passing and 2nd variable we are passing address in which
		//we are getting actual shader count currently attached to shader prog 
		glGetAttachedShaders(NOISE_gShaderProgramObject, shaderCount, &shaderCount, pShaders);
		for (GLsizei i = 0; i < shaderCount; i++) {
			glDetachShader(NOISE_gShaderProgramObject, pShaders[i]);
			glDeleteShader(pShaders[i]);
			pShaders[i] = 0;
		}
		free(pShaders);
		glDeleteProgram(NOISE_gShaderProgramObject);
		NOISE_gShaderProgramObject = 0;
		glUseProgram(0);
	}
}
