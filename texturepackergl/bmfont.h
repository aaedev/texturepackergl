/*
BMFont example implementation with Kerning, for C++ and OpenGL 2.0+

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

#ifndef __BMFONT__
#define __BMFONT__

#include <vector>
#include <map>
#include "colordefs.h"
#include "gl_basics.h"


//This can and should be substituted by your own Vec2f implementation
class fpoint
{
public:
	float x;
	float y;
	fpoint(float x, float y) : x(x), y(y) {}
	fpoint() : x(0), y(0) {}
	~fpoint() {};
};


class txdata
{
public:

	float x, y;
	float tx, ty;
	rgb_t colors;

	txdata() : x(0), y(0), tx(0), ty(0), colors(0) { }
	txdata(float x, float y, float tx, float ty, rgb_t colors) : x(x), y(y), tx(tx), ty(ty), colors(colors) {}
};


class KearningInfo
{
public:

	short First;
	short Second;
	short Amount;

	KearningInfo() : First(0), Second(0), Amount(0) { }
};


class CharDescriptor
{

public:
	short x, y;
	short Width;
	short Height;
	short XOffset;
	short YOffset;
	short XAdvance;
	short Page;

	CharDescriptor() : x(0), y(0), Width(0), Height(0), XOffset(0), YOffset(0),
		XAdvance(0), Page(0) { }
};


class BMFont
{

public:

	enum fontalign { NONE, AlignNear, AlignCenter, AlignFar };
	enum fontorigin { Top, Bottom };
	//Font load function
	bool  BMFont::LoadFont(std::string fontfile);
	//Adjusters
	void  BMFont::SetColor(int r, int g, int b, int a) { fcolor = MAKE_RGBA(r, g, b, a); fcolor2 = 0; }
	void  BMFont::SetColor(rgb_t startcolor, rgb_t endcolor, int dir);
	void  BMFont::SetColor(rgb_t color) { fcolor = color; }
	void  BMFont::SetBlend(int b) { fblend = b; }
	void  BMFont::SetScale(float);
	void  BMFont::SetCaching(bool en);
	void  BMFont::ClearCache() { txlist.clear(); };
	void  BMFont::SetAngle(int);
	void  BMFont::SetAlign(fontalign align);
	void  BMFont::SetOrigin(fontorigin origin);
	float BMFont::GetHeight() { return LineHeight * fscale; }
	void  BMFont::UseKerning(bool b) { UseKern = b; }
	//Pathing functions.
	void BMFont::SetPath(std::string &path);
	void BMFont::SetPath(const char *path);
	std::string BMFont::GetPath();
	//Printing
	void  BMFont::Print(float x, float y, const char *fmt, ...);
	void  BMFont::Print(float scale, float x, float y, const char *fmt, ...);
	void  BMFont::Print(float x, float y, rgb_t color, float scale, const char *fmt, ...);
	void  BMFont::Print(float y, rgb_t color, float scale, int angle, const char *fmt, ...);
    //Screen related functions
	float BMFont::GetStringWidth(const char *);
	float BMFont::GetStringWidth(std::string &string);
	//Renderer Function
	void  BMFont::Render();

	BMFont(int width, int height)
	{
		ftexid = nullptr;
		surface_width = width;
		surface_height = height;
		MathTableInit();
		SetColor(RGB_WHITE);
		fcolor2 = 0;
		KernCount = 0;
		fblend = 0;
		fscale = 1.0;
		fangle = 0;
		fcache = false;
		falign = fontalign::NONE;
		forigin = fontorigin::Bottom;
		UseKern = true;
	};
	~BMFont();


private:
	//Character and page variables
	short LineHeight;
	short Base;
	short Width;
	short Height;
	short Pages;
	short Outline;
	short KernCount;
	std::map<int, CharDescriptor> Chars;     //Character Descriptor Map
	std::vector<KearningInfo> Kearn;        //Kerning info 
	std::vector<txdata> txlist;             //Vertex Array Data
	std::string Pngname;                    //Png File name storage for loading

	//Modifiers
	bool UseKern;							//Whether or not to process kerning information 
	rgb_t fcolor;							//Font current color
	rgb_t fcolor2;							//Second font color for gradient effects
	TEX *ftexid;							//Font Texture ID
	float fscale;							//Current Font scaling factor
	int fblend;								//Current Rendering Blending Value"Do I need this?"
	int falign;								//Font Alignment Variable
	bool forigin;							//Set Font origin Top Or Bottom
	int fangle;								//Set font angle
	bool fcache;                            //Enable Text Caching. WARNING! Text must be pre-staged before entering main program loop!
	int surface_width;                      //Width of the orthographic 2D surface the text is going on
	int surface_height;                     //Height of the orthographic 2D surface the text is going on
	std::string datapath;                   //Path for data files, if empty use the root directory    

	//Internal Functions
	bool BMFont::ParseFont(const std::string &);
	int  BMFont::GetKerningPair(int, int);
	void BMFont::PrintKerningPairs();
	void BMFont::SortKerningPairs();
	void BMFont::PrintString(float x, float y, char *textstr);
	//void AddKerningPair(int first, int second, int amount);

	//Math Functions
	void   BMFont::MathTableInit();
	fpoint BMFont::MakePoint(float x, float y) { fpoint p; p.x = x; p.y = y; return p; }
	fpoint BMFont::RotateAroundPoint(float, float, float, float, float, float);

	//Math Table data for rotation
	float _sin[360];
	float _cos[360];
};


#endif
