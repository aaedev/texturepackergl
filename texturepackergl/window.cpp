
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "gl_basics.h"
#include "log.h"
#include "globals.h"
#include "bmfont.h"
#include "spritesheet.h"
#include "simpletimer.h"

#pragma warning (disable : 4996)

HWND hWnd;
RECT MyWindow;

//Default Window Size
int WinWidth = 1024;
int WinHeight = 768;

#pragma comment (lib, "opengl32.lib")  
#pragma comment (lib, "glu32.lib")  

BMFont *Snap;
Sprite *sp;

static int but = 0;
static int butlp = 0;
static int rt = 0;


HWND win_get_window(void)
{
	return hWnd;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Simple, generic window init //
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int iCmdShow)
{
	WNDCLASS wc;

	MSG msg;
	bool Quit = FALSE;
	DWORD       dwExStyle;                      // Window Extended Style
	DWORD       dwStyle;                        // Window Style

	// register window class
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"BMFontGL";
	RegisterClass(&wc);

	dwExStyle = WS_EX_APPWINDOW;   // Window Extended Style    
	dwStyle = WS_OVERLAPPEDWINDOW | WS_THICKFRAME;                    // Windows Style
	RECT WindowRect;                                                 // Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left = (long)0;                                         // Set Left Value To 0
	WindowRect.right = (long)WinWidth;                                 // Set Right Value To Requested Width
	WindowRect.top = (long)0;                                          // Set Top Value To 0
	WindowRect.bottom = (long)WinHeight;                               // Set Bottom Value To Requested Height
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);      // Adjust Window To True Requested Size


	// Create The Window
	if (!(hWnd = CreateWindowEx(dwExStyle,							// Extended Style For The Window
		L"BMFontGL",							// Class Name
		L"OpenGL BMFont Sample Implementation",						// Window Title
		dwStyle |							// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		CW_USEDEFAULT, 0,   				// Window Position
		WindowRect.right - WindowRect.left,	// Calculate Window Width
		WindowRect.bottom - WindowRect.top,	// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		hInstance,							// Instance
		NULL)))								// Dont Pass Anything To WM_CREATE
	{
		// Reset The Display
		MessageBox(NULL, L"Window Creation Error.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}


	//********** Program Initializations *************
	//Enable Logging
	LogOpen("glfonttest.log");
	// enable OpenGL for the window
	CreateGLContext();
	//Basic Window Init
	WRLOG("Starting Program");
	ShowWindow(hWnd, SW_SHOW);                                         // Show The Window
	SetForegroundWindow(hWnd);									    // Slightly Higher Priority
	SetFocus(hWnd);													// Sets Keyboard Focus To The Window
	ReSizeGLScene(WinWidth, WinHeight);										    // Set Up Our Perspective GL Screen
	//Get the Supported OpenGl Version
	CheckGLVersionSupport();
	//Fill in the Window Rect;
	GetClientRect(hWnd, &MyWindow);
	//Set the OpenGl View
	ViewOrtho(WinWidth, WinHeight);

	//Install the animation timer
	timers_init();

	//Load and Initialize the Fonts
	Snap = new BMFont(WinWidth, WinHeight);
	if (!Snap->LoadFont("snap.fnt"))
	{
		MessageBox(NULL, L"Error, font file not found, exiting", L"File Not Found", MB_ICONERROR | MB_OK);
		Quit = TRUE;
		//PostQuitMessage(-1);
	}
	LOG_DEBUG("Font Loaded Sucessfully");


	sp = new Sprite();
	sp->setPath("data\\");
	sp->LoadSheet("yamfegraphics.xml");

	// ********** Program Main Loop **********

	while (!Quit) {

		// check for messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {

			// handle or dispatch messages
			if (msg.message == WM_QUIT) {
				Quit = TRUE;
			}
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

		}
		else {

			// ********** OpenGL drawing code, very simple, just to show off the font. **********

			glClearColor(0.2f, 0.2f, 0.0f, 0.2f);
			glClear(GL_COLOR_BUFFER_BIT);

			// setup texture mapping
			glEnable(GL_TEXTURE_2D);
			glLoadIdentity();

			Snap->SetScale(1.0f);
			Snap->SetAngle(0);
			Snap->Print(130.0, 300, "This is just a test of what you can do");
			Snap->Render();
			sp->Add(200, 200, sp->nameToNum("shipani1.png"));

			if (ani_counter)
			{
				//reset for next frame
				ani_counter = 0;
				rt++; if (rt > 360) rt = 0;

				butlp++;
				if (butlp > 1) {
					butlp = 0;
					but++;
					if (but > 1) { but = 0; }
				}
			}
			
			sp->Add(60, 60, RGB_WHITE, 2.5f, rt, sp->nameToNum("shipani3.png"));
			sp->Add(300, 200, but);
			sp->Add(200, 200, sp->nameToNum("shipani1.png"));
			sp->Render();

			GlSwap();
		}

	}

	// ********** Cleanup and exit gracefully. **********

	timers_shutdown();
	delete Snap;
	delete sp;
	DeleteGLContext();
	LogClose();
	// destroy the window 
	DestroyWindow(hWnd);

	return (int)msg.wParam;
}


//Simple Message Pump Proc ///////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message) 
	{

	case WM_CREATE:
		return 0;

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_DESTROY:
		return 0;

	case WM_KEYDOWN:
		switch (wParam) {

		case VK_ESCAPE:
			PostQuitMessage(0);  return 0;

		case VK_F1: {SnapShot(); return 0; break; }
						
		}
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);

	}

}

