/*
BMFont example implementation with Kerning, for C++ and OpenGL 2.0

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <gl/gl.h>
#include <gl/glu.h>
#include "log.h"
#include "globals.h"
#include "gl_basics.h"
#define  GL_GLEXT_PROTOTYPES 1
#include "glext.h"
#define STB_IMAGE_IMPLEMENTATION   
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

//OpenGL Globals for context
HDC hDC;
HGLRC hRC;

#pragma warning (disable : 4996)


//Enable OpenGL 2.0 Context
void CreateGLContext()
{
	PIXELFORMATDESCRIPTOR pfd;
	int iFormat;
	// Get the device context (DC)
	hDC = GetDC(win_get_window());
	// Set the pixel format for the DC
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	iFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, iFormat, &pfd);

	// Create and enable the render context (RC)
	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);
}


// Disable OpenGL
void DeleteGLContext()
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	ReleaseDC(win_get_window(), hDC);
}


void GlSwap()
{
	SwapBuffers(hDC);
}


void CheckGLVersionSupport()
{
	const char *version;
	int major, minor;

	version = (char *)glGetString(GL_VERSION);
	sscanf(version, "%d.%d", &major, &minor);
	LOG_DEBUG("OpenGl Version supported %d.%d", major, minor);

	if (major < 2)
	{
		MessageBox(NULL,L"This program may not work, your supported opengl version is less then 2.0",L"OpenGL version warning", MB_ICONERROR | MB_OK);
	}
}


void FreeTexture(GLuint texture)
{
	glDeleteTextures(1, &texture);
}

void FreeTex(TEX *fonttex)
{
	if (fonttex)
	{
		glDeleteTextures(1, &fonttex->texid);
		delete fonttex;
	}
}

//----------------------------------TEXTURE Handling----------------------------------
//This function has a lot more options in my main library
//This is just a minimum build.
TEX *LoadPNG(std::string filename) 
{
	GLuint tex = 0;
	int x = 0;
	int y = 0;
	int comp = 0;

	TEX *temptex = new (TEX);

	unsigned char *image_data = stbi_load(filename.c_str(), &x, &y, &comp, STBI_rgb_alpha);
	if (!image_data) {
		wrlog("ERROR: could not load %s\n", filename);
		return 0;
	}
	wrlog("Texture x is %d, y is %d, components %d", x, y, comp);
	// NPOT check
	if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
		wrlog("WARNING: texture %s is not power-of-2 dimensions\n", filename);
	}
	//Flip Texture
	int width_in_bytes = x * STBI_rgb_alpha;
	unsigned char *top = NULL;
	unsigned char *bottom = NULL;
	unsigned char temp = 0;
	int half_height = y / 2;

	for (int row = 0; row < half_height; row++) {
		top = image_data + row * width_in_bytes;
		bottom = image_data + (y - row - 1) * width_in_bytes;
		for (int col = 0; col < width_in_bytes; col++) {
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			top++;
			bottom++;
		}
	}
	//Create Texture
	glGenTextures(1, &temptex->texid);
	glBindTexture(GL_TEXTURE_2D, temptex->texid);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA8,
		x,
		y,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		image_data
	);

	//
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
	//The below gluBuild2DMipmaps is ONLY for maximum compatibility using OpenGL 1.4.
	//In a real program, you should be using GLEW or some other library to allow glGenerateMipmap to work. 
	// it's much better,and you should be using an OpenGL 3+ context anyways. This is just for my example program.
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, x, y, GL_RGBA, GL_UNSIGNED_BYTE, image_data); //Never use this in anything but an opengl 2 demo.
	//glGenerateMipmap(GL_TEXTURE_2D); //Only if using opengl 3+, next example...
	GLfloat max_aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
	// set the maximum!
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);

	stbi_image_free(image_data); //Now that we're done making a texture, free the image data
	
								 //set image data properties
	temptex->bpp = comp;
	temptex->height = x;
	temptex->width = y;
	
	return temptex;
}


GLvoid ReSizeGLScene(GLsizei width, GLsizei height)             // Resize And Initialize The GL Window
{
	if (height == 0)                              // Prevent A Divide By Zero By
	{
		height = 1;                           // Making Height Equal One
	}
	glViewport(0, 0, width, height);                    // Reset The Current Viewport
}


void ViewOrtho(int width, int height)
{
	glViewport(0, 0, width, height);             // Set Up An Ortho View	 
	glMatrixMode(GL_PROJECTION);			  // Select Projection
	glLoadIdentity();						  // Reset The Matrix
	glOrtho(0, width, 0, height, -1, 1);	  // Select Ortho 2D Mode (640x480)
	glMatrixMode(GL_MODELVIEW);				  // Select Modelview Matrix
	glLoadIdentity();						  // Reset The Matrix
}


void GLPoint(float x, float y)
{
	glDisable(GL_TEXTURE_2D);
	glColor3f(.5f, 1.0f, .5f);
	glPointSize(4.0f);
	glBegin(GL_POINTS);
	glVertex2f(x, y);
	glEnd();
}

void GLLine(float sx, float sy, float ex, float ey)
{
	glBegin(GL_LINES);
	glVertex2f(sx, sy);
	glVertex2f(ex, ey);
	glEnd();
}

void GLRect(int xmin, int xmax, int ymin, int ymax)
{
	glBegin(GL_QUADS);
	glTexCoord2i(0, 1); glVertex2i(xmin, ymin);
	glTexCoord2i(0, 0); glVertex2i(xmin, ymax);
	glTexCoord2i(1, 0); glVertex2i(xmax, ymax);
	glTexCoord2i(1, 1); glVertex2i(xmax, ymin);
	glEnd();
}


void UseTexture(GLuint *texture, GLboolean linear, GLboolean mipmapping)
{
	GLenum filter = linear ? GL_LINEAR : GL_NEAREST;
	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (mipmapping)
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	else
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)filter);
}

void SetBlendMode(int mode)
{
	if (mode) {
		glEnable(GL_ALPHA_TEST);
		glDisable(GL_BLEND);
		glAlphaFunc(GL_GREATER, 0.5f);
	}
	else {
		glDisable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}


void SnapShot()
{
	unsigned char *bmpBuffer;
	time_t tim;
	struct tm tm;
	char buf[15];
	char temppath[80];

	int Width = 0;
	int Height = 0;
	int i = 0;

	RECT rect;

	GetClientRect(win_get_window(), &rect);
	Width = rect.right - rect.left;
	Height = rect.bottom - rect.top;

	memset(&temppath[0], 0, sizeof(temppath));

	//Get time/date stamp
	tim = time(0);
	tm = *localtime(&tim);
	strftime(buf, sizeof buf, "%Y%m%d%H%M%S", &tm);
	puts(buf);
	//////
	bmpBuffer = (unsigned char*)malloc(Width*Height * 4);

	if (!bmpBuffer) wrlog("Error creating buffer for snapshot"); return;

	glReadPixels((GLint)0, (GLint)0, (GLint)Width, (GLint)Height, GL_RGBA, GL_UNSIGNED_BYTE, bmpBuffer);

	//Flip Texture
	int width_in_bytes = Width * STBI_rgb_alpha;
	unsigned char *top = NULL;
	unsigned char *bottom = NULL;
	unsigned char temp = 0;
	int half_height = Height / 2;

	for (int row = 0; row < half_height; row++)
	{
		top = bmpBuffer + row * width_in_bytes;
		bottom = bmpBuffer + (Height - row - 1) * width_in_bytes;
		for (int col = 0; col < width_in_bytes; col++)
		{
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			top++;
			bottom++;
		}
	}

	strcat(temppath, "SS");
	strcat(temppath, buf);
	strcat(temppath, ".png");
	strcat(temppath, "\0");
	LOG_DEBUG("Saving Snapshot: %s", temppath);

	stbi_write_png(temppath, Width, Height, 4, bmpBuffer, Width * 4);

	free(bmpBuffer);
}