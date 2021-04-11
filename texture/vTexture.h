//
//
//

#include <stdio.h>
#include <stdlib.h>

#include "vVec.h"

#ifndef uchar
typedef unsigned char uchar;
#endif

class vVertex {
  public:
    vVec3	vtx;
    vVec2	txc;
    vVec3	svtx;	// Screen coordinate 
    vVec2	stxc;

    vVertex() {
    }

    ~vVertex() {
    }
};

class vTriangle {
  public:
    vVertex	v[3];

    vTriangle() {
    }

    ~vTriangle() {
    }
};

class vTexture {
  public:
    int	tw;
    int	th;
    uchar	*tPtr;		// Pointer to the texture image data.
    uchar 	*screen;	// Pointer to the screen.
	int screenWidth;
	int screenHeight;
    vTriangle	*tri;

    vTexture() {
	tri = new vTriangle;
    }

    ~vTexture() {
	delete tri;
    }

    int drawScanline(float, float, float, float, float, float, int);

    void render(void);

};
