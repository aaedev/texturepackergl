/*
Sprite Sheet example implementation  for C++ and OpenGL 2.0+

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

#ifndef __Sprite__
#define __Sprite__

#include "gl_basics.h"
#include <vector>
#include <map>
#include "colordefs.h"


class Sprite
{

 public:

	  //This can and should be substituted by your own Vec2f implementation
	 class spoint
	 {
		 public:
		 float x;
		 float y;
		 spoint(float x, float y): x(x), y(y) {} 
		 spoint() :  x(0), y(0) { }
	 };
	
	bool Sprite::LoadSheet(std::string fontfile);
	void Sprite::SetColor(int r, int g, int b, int a) {fcolor = MAKE_RGBA(r,g,b,a);}
	void Sprite::SetColor(rgb_t color) {fcolor=color;}
	void Sprite::SetBlend(int b) {fblend = b;}
	void Sprite::SetScale(int, float);
	void Sprite::SetCaching(bool en);
	void Sprite::ClearCache(){txlist.clear();};
	void Sprite::Rotate(int);
	void Sprite::setOriginOffset(float x, float y, int spritenum);
	void Sprite::setOriginOffset(spoint a, int spritenum);
	void Sprite::setRotationAngle(int spritenum, int sangle);
	int Sprite::nameToNum(std::string name);
	std::string Sprite::numToName(int spritenum);
	void Sprite::setPath(std::string &path);
	void Sprite::setPath(const char *path);
	std::string Sprite::getPath();
	
	void Sprite::Add(float x, float y, int spritenum);
	void Sprite::Add(float scale, float x, float y, int spritenum);
	void Sprite::Add(float x, float y, rgb_t color, float scale, int spritenum);
	void Sprite::Add(float x, float y, rgb_t color, float scale, int angle, int spritenum);
		
	void  Sprite::Render();
		
	Sprite() 
	  {
		  MathTableInit();
		  SetColor(RGB_WHITE);
		  fblend = 0;
          fscale = 1.0;
		  fangle = 0;
		  fcache = false;
		  count = 0;
	  };
	 ~Sprite ();

 //protected:
	
	
		 
	 class SpriteDescriptor
	 {
	 public:

		short x, y;
		short Width;
		short Height;
		std::string name;
		spoint spOffset;
		float scale;
		int angle;

	//	friend bool operator== (const SpriteDescriptor &n1, const SpriteDescriptor &n2)
	//	{return (n1.name == n2.name);}

	//	friend bool operator!= (const SpriteDescriptor &n1, const SpriteDescriptor &n2);
	//	{return !(n1 == n2);}
   			
		SpriteDescriptor() : x( 0 ), y( 0 ), Width( 0 ), Height( 0 ), name ("na"), spOffset(0,0) , scale(1.0), angle(0) { }
	 };

	
	class stxdata {
	public:

	  float x, y;
	  float tx,ty;
	  rgb_t colors;

	  stxdata() :  x(0), y(0), tx (0), ty(0),colors(0) { }
	  stxdata(float x, float y, float tx, float ty, rgb_t colors): x(x), y(y), tx(tx), ty(ty), colors(colors) {} 

	};
	
 private:
	 //Character and page variables

	 short sheetWidth;
	 short sheetHeight;
	 short count;

	 std::vector<SpriteDescriptor> Sprites;     //Character Descriptor Map
	 std::vector<stxdata> txlist;               //Vertex Array Data
	 std::string Pngname;                       //Png File name storage for loading

												//Modifiers
	 rgb_t fcolor;							   //Sprite current color
	 TEX *ftexid;							   //Sprite Texture ID
	 float fscale;							   //Current Font scaling factor
	 int fblend;								   //Current Rendering Blending Value"Do I need this?"
	 int fangle;								   //Set font angle
	 bool fcache;                               //Enable Text Caching. WARNING! Text must be pre-staged before entering main program loop!
	 std::string datapath;                      //Path for data files, if empty use the root directory       

												//Internal Functions
	 bool Sprite::ParseDefinition(const std::string &);
	 void Sprite::InternalAdd(float x, float y, int spriteid);

	 //Math Functions
	 void Sprite::MathTableInit();
	 Sprite::spoint Sprite::MakePoint(float x, float y) { spoint p; p.x = x; p.y = y; return p; }
	 Sprite::spoint Sprite::RotateAroundPoint(float, float, float, float, float, float);

	 //Math Table data for rotation - This could be a separate implementation, or just use sin/cos/rad.
	 float _sin[360];
	 float _cos[360];
};



#endif
