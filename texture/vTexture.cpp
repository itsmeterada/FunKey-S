//
//
//

#include "vTexture.h"

int
vTexture::drawScanline(float x1, float x2, float u1, float u2,
	float v1, float v2, int y)
{
    int i;

    // To draw always from left to right.
    if (x1 > x2) {
		float tmpx;
		tmpx = x1; x1 = x2; x2 = tmpx;
		tmpx = u1; u1 = u2; u2 = tmpx;
		tmpx = v1; v1 = v2; v2 = tmpx;
    }

    float width = x2 - x1;
    float du, dv;
    int u, v;

    if (width) {
		du = (u2 - u1) / width;
		dv = (v2 - v1) / width;
    }

	int idx;

    // Draw a scanline
	//if (x1 > 0.0f && x2 < screenWidth) {
   	for (i = (int)x1; i < (int)x2; i++) {
		u = (int)u1;
		v = (int)v1;
		screen[(y * screenWidth + i) * 4 + 0] = tPtr[(v * tw + u) * 4 + 0];
		screen[(y * screenWidth + i) * 4 + 1] = tPtr[(v * tw + u) * 4 + 1];
		screen[(y * screenWidth + i) * 4 + 2] = tPtr[(v * tw + u) * 4 + 2];
		screen[(y * screenWidth + i) * 4 + 3] = tPtr[(v * tw + u) * 4 + 3];
		u1 += du;
		v1 += dv;
	}
    //}

    return 0;
}

void
vTexture::render(void)
{
    int top, a, b, i, y, height1, height2;
    float dx1, dx2, du1, du2, dv1, dv2;
    float x1, x2, u1, u2, v1, v2;

    top = 0;
    for (i = 1; i < 3; i++) {
		if (tri->v[i].svtx[1] < tri->v[top].svtx[1]) top = i;
    }

    a = top + 1;
    if (a > 2) a = 0;
    b = top - 1;
    if (b < 0) b = 2;

    y = (int)(tri->v[top].svtx[1]);
    x1 = x2 = tri->v[top].svtx[0];
    u1 = u2 = tri->v[top].txc[0];
    v1 = v2 = tri->v[top].txc[1];

    height1 = (int)(tri->v[a].svtx[1] - tri->v[top].svtx[1]);
    if (height1) {
		dx1 = (tri->v[a].svtx[0] - tri->v[top].svtx[0]) / height1;
		du1 = (tri->v[a].txc[0] - tri->v[top].txc[0]) / height1;
		dv1 = (tri->v[a].txc[1] - tri->v[top].txc[1]) / height1;
    }

    height2 = (int)(tri->v[b].svtx[1] - tri->v[top].svtx[1]);
    if (height2) {
		dx2 = (tri->v[b].svtx[0] - tri->v[top].svtx[0]) / height2;
		du2 = (tri->v[b].txc[0] - tri->v[top].txc[0]) / height2;
		dv2 = (tri->v[b].txc[1] - tri->v[top].txc[1]) / height2;
    }

    for (i = 2; i > 0; ) {
		while(height1 && height2) {
	    	drawScanline(x1, x2, u1, u2, v1, v2, y);
	    	y++;
	    	height1--;
	    	height2--;
	    	x1 += dx1;
	    	x2 += dx2;
	    	u1 += du1;
	    	u2 += du2;
	    	v1 += dv1;
	    	v2 += dv2;
		}

		if (!height1) {
	    	int na;
	    	na = a + 1;
	    	if (na > 2) na = 0;
	    	height1 = (int)(tri->v[na].svtx[1] - tri->v[a].svtx[1]);
    	    if (height1) {
				dx1 = (tri->v[na].svtx[0] - tri->v[a].svtx[0]) / height1;
				du1 = (tri->v[na].txc[0] - tri->v[a].txc[0]) / height1;
				dv1 = (tri->v[na].txc[1] - tri->v[a].txc[1]) / height1;
    	    }
	    	x1 = tri->v[a].svtx[0];
	    	u1 = tri->v[a].txc[0];
	    	v1 = tri->v[a].txc[1];
	    	i--;
	    	a = na;
		}

		if (!height2) {
	    	int nb;
	    	nb = b - 1;
	    	if (nb < 0) nb = 2;
	    	height2 = (int)(tri->v[nb].svtx[1] - tri->v[b].svtx[1]);
    	    if (height2) {
				dx2 = (tri->v[nb].svtx[0] - tri->v[b].svtx[0]) / height2;
				du2 = (tri->v[nb].txc[0] - tri->v[b].txc[0]) / height2;
				dv2 = (tri->v[nb].txc[1] - tri->v[b].txc[1]) / height2;
    	    }
	    	x2 = tri->v[b].svtx[0];
	    	u2 = tri->v[b].txc[0];
	    	v2 = tri->v[b].txc[1];
	    	i--;
	    	b = nb;
		}
    }
}

