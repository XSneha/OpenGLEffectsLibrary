#pragma once
#ifndef MASTER_HEADER_H
#define MASTER_HEADER_H
#include<windows.h>
#include<stdio.h>
#include"vmath.h"
#include "../stb_image.h"
#include<gl/glew.h>
#include<gl/gl.h>

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"OpenGL32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define MYICON 101
#define TERRAIN_BITMAP 102

using namespace vmath;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//common globals
extern DWORD dwStyle;
extern HWND ghwnd;
extern FILE* gpFile;
extern mat4 perspectiveProjectionMatrix;

//shader helper functions
GLuint CompileShader(const char* shader_file_name, GLuint shader_type, const char* shaderSourceParam);
void LinkShader(GLuint);

//Perspective triangle functions
void DisplayMulticoloredTriangle();
void InitializeMulticoloredTriangle();
void UnInitializeMulticoloredTriangle();

//metaball vertex updater functions
void InitializeMetaBallVertexUpdater();
void RenderMetaballVertexUpdater();
void UninitializeMetaballVertexUpdater();

//Starfield
void InitializeStarField();
void RenderStarField();
void UnInitializeStarField();

//terrain 
void InitializeTerrain();
void RenderTerrain();
void UnInitializeTerrain();

//water
void InitializeWater();
void RenderWater();
void UnInitializeWater();

//noise
void InitializeNoise();
void RenderNoise();
void UnInitializeNoise();

bool LoadGLTexture(GLuint* texture, TCHAR resourceId[]);

#endif

