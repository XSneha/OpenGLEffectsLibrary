#pragma once
#include "headers/Globals.h"

//Windowing variables
DWORD dwStyle;
HWND ghwnd;
bool gbFullscree = false;
bool gbActiveWindow = false;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

FILE* gpFile;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

//common perspective projection matrix
mat4 perspectiveProjectionMatrix;

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int iCmdShow) {
	void Initialize(void);
	void Display(void);

	WNDCLASSEX wndclass;
	MSG msg;
	HWND hwnd;
	TCHAR szAppName[] = TEXT("OpenGl Effects Template");
	bool bDone = false;

	if (fopen_s(&gpFile, "logs/Log.txt", "w") != 0) {
		MessageBox(NULL, TEXT("Failed to Open file Mylog.txt"), TEXT("ERROR"), MB_OK);
		return (0);
	}

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.lpszClassName = szAppName;
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpszMenuName = NULL;

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("OpenGL Effects Demo."),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		0,
		0,
		hInstance,
		NULL);

	if (hwnd == NULL) {
		MessageBox(NULL, TEXT("Failed to Create Window."), TEXT("ERROR!"), MB_OK);
		exit(0);
	}
	ghwnd = hwnd;

	Initialize();
	ShowWindow(hwnd, iCmdShow);

	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	while (bDone == false) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				bDone = true;
			}
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			if (gbActiveWindow == true) {
				Display();
			}
		}
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {

	void Resize(int, int);
	void UnInitialize(void);
	void ToggleFullscreen(void);

	MONITORINFO mi = { sizeof(MONITORINFO) };

	switch (iMsg) {
	case WM_SETFOCUS:
		gbActiveWindow = true;
		break;
	case WM_KILLFOCUS:
		gbActiveWindow = false;
		break;
	case WM_SIZE:
		Resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_ERASEBKGND:
		return(0);
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:DestroyWindow(ghwnd);
			break;
		case 0x46:
		case 0x66:
			ToggleFullscreen();
			break;
		default:
			break;
		}
		break;
	case WM_DESTROY:
		UnInitialize();
		PostQuitMessage(0);
		break;
	default:break;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

void ToggleFullscreen(void) {
	MONITORINFO mi = { sizeof(MONITORINFO) };
	if (gbFullscree == false) {
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW) {
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi)) {
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,
					HWND_TOP,
					mi.rcMonitor.left,
					mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		//ShowCursor(FALSE);
		gbFullscree = true;
	}
	else {
		//ShowCursor(TRUE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
		gbFullscree = false;
	}
}

void Initialize(void) {
	void Resize(int, int);
	void InitializeMulticoloredTriangle();

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;

	ghdc = GetDC(ghwnd);

	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0) {
		fprintf(gpFile, "ChoosePixelFormat() Failed\n");
		DestroyWindow(ghwnd);
	}
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE) {
		fprintf(gpFile, "SetPixelFormat() Failed\n");
		DestroyWindow(ghwnd);
	}
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL) {
		fprintf(gpFile, "wglCreateContext() Failed\n");
		DestroyWindow(ghwnd);
	}
	if (wglMakeCurrent(ghdc, ghrc) == FALSE) {
		fprintf(gpFile, "wglMakeCurrent() Failed\n");
		DestroyWindow(ghwnd);
	}
	//Glew initilalization code
	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK) {
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	//OpenGL realted logs
	fprintf(gpFile, "\n\n OpenGL vendor : %s \n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL renderer : %s \n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL renderer : %s \n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL version : %s \n", glGetString(GL_VERSION));
	fprintf(gpFile, "GLSL version : %s \n\n ", glGetString(GL_SHADING_LANGUAGE_VERSION));

	//OpenGL enabled extensions
	GLint numExt;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);

	//loop
	for (int i = 0; i < numExt; i++) {
		fprintf(gpFile, "%s \n", glGetStringi(GL_EXTENSIONS, i));
	}
	
	//InitializeMulticoloredTriangle();
	InitializeMetaBallVertexUpdater();
	
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_CULL_FACE);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	perspectiveProjectionMatrix = mat4::identity();
	//warmup resize
	Resize(WIN_WIDTH, WIN_HEIGHT);

}

void Resize(int width, int height) {
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	//gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void Display(void) {
	void DisplayMulticoloredTriangle();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//DisplayMulticoloredTriangle();
	RenderMetaballVertexUpdater();
	SwapBuffers(ghdc);
}

void UnInitialize(void) {
	dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
	ShowCursor(TRUE);
	SetWindowLong(ghwnd, GWL_STYLE, dwStyle);
	SetWindowPlacement(ghwnd, &wpPrev);
	SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOOWNERZORDER);

//	UnInitializeMulticoloredTriangle();
	UninitializeMetaballVertexUpdater();
	if (wglGetCurrentContext() == ghrc) {
		wglMakeCurrent(NULL, NULL);
	}
	if (ghrc) {
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}
	if (ghdc) {
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	if (gpFile) {
		fclose(gpFile);
		gpFile = NULL;
	}
}
