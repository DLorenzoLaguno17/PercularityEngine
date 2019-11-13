#pragma once

// Warning that exceptions are disabled
#pragma warning( disable : 4577 ) 
#pragma warning( disable : 4530 )

#include <windows.h>
#include <stdio.h>
#include <list>
#include <vector>

#include "Json Parser/nlohmann/json.hpp"

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define HAVE_M_PI

#ifdef NULL
#undef NULL
#endif
#define NULL  0

// Deletes a buffer
#define RELEASE( x ) \
    {                        \
    if( x != NULL )        \
	    {                      \
      delete x;            \
	  x = NULL;              \
	    }                      \
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x ) \
    {                              \
    if( x != NULL )              \
	    {                            \
      delete[] x;                \
	  x = NULL;                    \
	    }                            \
                              \
    }

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned long ulong;

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define SCREEN_SIZE 1
#define WIN_FULLSCREEN false
#define WIN_RESIZABLE false
#define WIN_BORDERLESS false
#define WIN_FULLSCREEN_DESKTOP true
#define VSYNC true
#define TITLE "Percularity v0.1"
#define ORG "UPC - Tech Talent Center"

#define ASSETS_FOLDER "/Assets/"
#define ASSETS_MESH_FOLDER "/Assets/FBX/"
#define ASSETS_TEXTURE_FOLDER "/Assets/Textures/"
#define LIBRARY_FOLDER "/Library/"
#define LIBRARY_TEXTURE_FOLDER "/Library/Textures/"
#define LIBRARY_MESH_FOLDER "/Library/Meshes/"
#define LIBRARY_MODEL_FOLDER "/Library/Models/"
#define LIBRARY_MATERIAL_FOLDER "/Library/Materials/"