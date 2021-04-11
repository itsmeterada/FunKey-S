/*
 *
 */

#include <stdio.h>
#include <cstdarg>

#include "demo01.h"

#include <limits.h>
#include <math.h>

using namespace swr;

int main(int ac, char *av[])
{
	Uint32 fs = SDL_FULLSCREEN | SDL_DOUBLEBUF | SDL_HWSURFACE;
	//Uint32 fs = 0;

	//SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Surface *hw_surface = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, fs);
	//SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, fs);

	SDL_ShowCursor(SDL_DISABLE);
  	demo01();

	return 0;
}

void demo01()
{
	screen = SDL_DisplayFormat(SDL_GetVideoSurface());
	zbuffer = SDL_CreateRGBSurface(SDL_HWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 16, 0, 0, 0, 0);

	for (size_t i = 0; i < sizeof(cube_vertices)/sizeof(float); ++i) {
		int *v = reinterpret_cast<int*>(&cube_vertices[i]);
		*v = float2fix<16>(cube_vertices[i]);
	}

	for (size_t i = 0; i < sizeof(star_vertices)/sizeof(float); ++i) {
		int *v = reinterpret_cast<int*>(&star_vertices[i]);
		*v = float2fix<16>(star_vertices[i]);
	}

	RasterizerSubdivAffine r;
	GeometryProcessor g(&r);
	r.clip_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	g.viewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	g.vertex_shader<VertexShader>();
	r.fragment_shader<FragmentShader>();
	/*r.interlace(0, 1);*/

	Texture t;
	tex = &t;
	mat4x projection_matrix = perspective_matrix(X(50.0f), X(3.0f/3.0f), X(0.1f), X(10.0f));

	Uint32 demolength = 15000;
	Uint32 demostart = SDL_GetTicks();
	Uint32 framecount = 0;
	while( true ) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT:
				case SDL_JOYBUTTONDOWN:
				case SDL_MOUSEBUTTONDOWN:
				case SDL_KEYDOWN:
					exit(1);
			}
		}
		Uint32 t = SDL_GetTicks();
		Uint32 demotime = t - demostart;

		SDL_FillRect(zbuffer, 0, INT_MAX);

		float x = demotime/1000.0f;
		pos = vec3x(sin(X(x)/2), cos(X(x)/2), sin(X(x)/2) / 4);
		modelviewprojection_matrix = projection_matrix *
			lookat_matrix(pos, vec3x(X(0.0f)), vec3x(X(0.0f), X(0.0f), X(1.0f)));

		SDL_LockSurface(screen);
		g.vertex_shader<VertexShader>();
		r.fragment_shader<FragmentShader>();
		g.vertex_attrib_pointer(0, 5 * sizeof(float), cube_vertices);
		g.draw_triangles(36, cube_indices);

		g.vertex_shader<StarVertexShader>();
		r.fragment_shader<StarFragmentShader>();
		g.vertex_attrib_pointer(0, 6 * sizeof(float), star_vertices);
		g.draw_triangles(72, star_indices);
		SDL_UnlockSurface(screen);

		if (demotime < 1000) {
			fade_effect(screen, false, 1.0f - (t-demostart)/1000.0f);
		}

		if (demotime > demolength) {
			float x = (demotime - demolength)/1000.0f;
			fade_effect(screen, true, x);
		}

		SDL_BlitSurface(screen, 0, SDL_GetVideoSurface(), 0);
		SDL_Flip(SDL_GetVideoSurface());

		if (t-demostart > demolength + 1000)
			goto end;

		static int fpscounter = 0;
		static Uint32 fpst = SDL_GetTicks();
		fpscounter++;
		if (SDL_GetTicks() >= fpst + 500) {
			fpst += 500;
			printf("%4i fps\n", fpscounter*2);
			fpscounter = 0;
		}

		framecount++;
	}
end:;

	printf("avg. fps: %f\n", (float)framecount * 1000.0f/(SDL_GetTicks() - demostart));

	SDL_FreeSurface(screen);
	SDL_FreeSurface(zbuffer);
}

namespace fixedpoint {

static const int32_t FIX16_2PI	= float2fix<16>(6.28318530717958647692f);
static const int32_t FIX16_R2PI = float2fix<16>(1.0f/6.28318530717958647692f);

static const uint16_t sin_tab[] = {
#include "fixsintab.h"
};

int32_t fixcos16(int32_t a)
{
    int32_t v;
    /* reduce to [0,1) */
    while (a < 0) a += FIX16_2PI;
    a = fixmul<16>(a, FIX16_R2PI);
    a += 0x4000;

    /* now in the range [0, 0xffff], reduce to [0, 0xfff] */
    a >>= 4;

    v = (a & 0x400) ? sin_tab[0x3ff - (a & 0x3ff)] : sin_tab[a & 0x3ff];
	v = fixmul<16>(v, 1 << 16);
    return (a & 0x800) ? -v : v;
}

int32_t fixsin16(int32_t a)
{
    int32_t v;

    /* reduce to [0,1) */
    while (a < 0) a += FIX16_2PI;
    a = fixmul<16>(a, FIX16_R2PI);

    /* now in the range [0, 0xffff], reduce to [0, 0xfff] */
    a >>= 4;

    v = (a & 0x400) ? sin_tab[0x3ff - (a & 0x3ff)] : sin_tab[a & 0x3ff];
    v = fixmul<16>(v, 1 << 16);
    return (a & 0x800) ? -v : v;
}

int32_t fixrsqrt16(int32_t a)
{
    int32_t x;

    static const uint16_t rsq_tab[] = { /* domain 0.5 .. 1.0-1/16 */
		0xb504, 0xaaaa, 0xa1e8, 0x9a5f, 0x93cd, 0x8e00, 0x88d6, 0x8432,
    };

    int32_t i, exp;
    if (a == 0) return 0x7fffffff;
    if (a == (1<<16)) return a;

	exp = detail::CountLeadingZeros(a);
    x = rsq_tab[(a>>(28-exp))&0x7]<<1;

	exp -= 16;
    if (exp <= 0)
		x >>= -exp>>1;
    else
		x <<= (exp>>1)+(exp&1);
    if (exp&1) x = fixmul<16>(x, rsq_tab[0]);


    /* newton-raphson */
    /* x = x/2*(3-(a*x)*x) */
    i = 0;
    do {

		x = fixmul<16>((x>>1),((1<<16)*3 - fixmul<16>(fixmul<16>(a,x),x)));
    } while(++i < 3);

    return x;
}

static inline int32_t fast_div16(int32_t a, int32_t b)
{
	if ((b >> 24) && (b >> 24) + 1) {
		return fixmul<16>(a >> 8, fixinv<16>(b >> 8));
	} else {
		return fixmul<16>(a, fixinv<16>(b));
	}
}

int32_t fixsqrt16(int32_t a)
{
    int32_t s;
    int32_t i;
    s = (a + (1<<16)) >> 1;
    /* 6 iterations to converge */
    for (i = 0; i < 6; i++)
		s = (s + fast_div16(a, s)) >> 1;
    return s;
}

} // end namespace fixedpoint


namespace swr {
	namespace detail {
		static const unsigned SKIP_FLAG = static_cast<unsigned>(-1);
	}

#define DIFFERENT_SIGNS(x,y) ((x <= 0 && y > 0) || (x > 0 && y <= 0))

#define CLIP_DOTPROD(I, A, B, C, D) \
	(vertices_[I].x * A + vertices_[I].y * B + vertices_[I].z * C + vertices_[I].w * D)

#define POLY_CLIP( PLANE_BIT, A, B, C, D ) \
{  \
	if (mask & PLANE_BIT) {  \
		int idxPrev = inlist[0];  \
		int dpPrev = CLIP_DOTPROD(idxPrev, A, B, C, D );  \
		int outcount = 0;  \
		int i;  \
  \
		inlist[n] = inlist[0];  \
		for (i = 1; i <= n; i++) { 		  \
			int idx = inlist[i]; \
			int dp = CLIP_DOTPROD(idx, A, B, C, D ); \
			if (dpPrev >= 0) {  \
				outlist[outcount++] = idxPrev;  \
			}  \
   \
			if (DIFFERENT_SIGNS(dp, dpPrev)) {				   \
				if (dp < 0) {					   \
					/*int t = fixdiv<24>(dp,(dp - dpPrev));				   */\
					int t = fixmul<8>(dp, invert(dp - dpPrev)); \
					add_interp_vertex(t, idx, idxPrev);   \
				} else {							   \
					/*int t = fixdiv<24>(dpPrev,(dpPrev - dp));			   */\
					int t = fixmul<8>(dpPrev, invert(dpPrev - dp)); \
					add_interp_vertex(t, idxPrev, idx);   \
				}								   \
				outlist[outcount++] = (int)(vertices_.size() - 1); \
			}								   \
   \
			idxPrev = idx;							   \
			dpPrev = dp;							   \
		}									   \
   \
		if (outcount < 3)							   \
			continue;							   \
   \
	 	{									   \
			int *tmp = inlist;				  	 \
			inlist = outlist;				  	 \
			outlist = tmp;					  	 \
			n = outcount;					  	 \
		}									   \
	}									   \
}

#define LINE_CLIP(PLANE_BIT, A, B, C, D ) \
{ \
	if (mask & PLANE_BIT) { \
		const int dp0 = CLIP_DOTPROD( v0, A, B, C, D ); \
		const int dp1 = CLIP_DOTPROD( v1, A, B, C, D ); \
		const bool neg_dp0 = dp0 < 0; \
		const bool neg_dp1 = dp1 < 0; \
\
		if (neg_dp0 && neg_dp1) \
			continue; \
\
		if (neg_dp1) { \
			/*int t = fixdiv<24>(dp1, (dp1 - dp0));*/ \
			int t = fixmul<8>(dp1, invert(dp1 - dp0)); \
			if (t > t1) t1 = t; \
		} else if (neg_dp0) { \
			/*int t = fixdiv<24>(dp0, (dp0 - dp1));*/ \
			int t = fixmul<8>(dp0, invert(dp0 - dp1)); \
			if (t > t0) t0 = t; \
		} \
		if (t0 + t1 >= (1 << 24)) \
			continue; \
	} \
}

////////////////////////////////////////////////////////////////////////////////
// implement the interface inherited from the vertex processor

void GeometryProcessor::process_begin() {}

GeometryProcessor::VertexOutput* GeometryProcessor::acquire_output_location() {
	vertices_.resize(vertices_.size() + 1);
	return &vertices_.back();
}

bool GeometryProcessor::push_vertex_index(unsigned i)
{
	indices_.push_back(i);

	switch (draw_mode_) {
	case DM_TRIANGLES:
		if (indices_.size() >= MAX_TRIANGLES * 3) {
			process_triangles();
			return true;
		}
		break;
	case DM_LINES:
		if (indices_.size() >= MAX_LINES * 2) {
			process_lines();
			return true;
		}
		break;
	case DM_POINTS:
		if (indices_.size() >= MAX_POINTS) {
			process_points();
			return true;
		}
		break;
	}

	return false;
}

void GeometryProcessor::process_end() {
	if (draw_mode_ == DM_TRIANGLES)
		process_triangles();
	else if (draw_mode_ == DM_LINES)
		process_lines();
	else if (draw_mode_ == DM_POINTS)
		process_points();
}

////////////////////////////////////////////////////////////////////////////////

enum {
	CLIP_POS_X_BIT = 0x01,
	CLIP_NEG_X_BIT = 0x02,
	CLIP_POS_Y_BIT = 0x04,
	CLIP_NEG_Y_BIT = 0x08,
	CLIP_POS_Z_BIT = 0x10,
	CLIP_NEG_Z_BIT = 0x20
};

static inline int calc_clip_mask(const GeometryProcessor::VertexOutput& v)
{
	int cmask = 0;
	if (v.w - v.x < 0) cmask |= CLIP_POS_X_BIT;
	if (v.x + v.w < 0) cmask |= CLIP_NEG_X_BIT;
	if (v.w - v.y < 0) cmask |= CLIP_POS_Y_BIT;
	if (v.y + v.w < 0) cmask |= CLIP_NEG_Y_BIT;
	if (v.w - v.z < 0) cmask |= CLIP_POS_Z_BIT;
	if (v.z + v.w < 0) cmask |= CLIP_NEG_Z_BIT;
	return cmask;
}

GeometryProcessor::GeometryProcessor(IRasterizer *r) : rasterizer_(r)
{
	// don't use the full positive range (0x7fffffff) because it
	// could be possible to get overflows at the far plane.
	depth_range(0, 0x3fffffff);
	cull_mode_ = CULL_CW;
	viewport_.ox = viewport_.oy = viewport_.px = viewport_.py = 0;
}

void GeometryProcessor::add_interp_vertex(int t, int out, int in)
{
	using namespace detail;

	#define LINTERP(T, OUT, IN) (OUT) + fixmul<24>((IN) - (OUT), (T))

	vertices_.resize(vertices_.size() + 1);
	VertexOutput& v = vertices_.back();
	VertexOutput& a = vertices_[out];
	VertexOutput& b = vertices_[in];

	v.x = LINTERP(t, a.x, b.x);
	v.y = LINTERP(t, a.y, b.y);
	v.z = LINTERP(t, a.z, b.z);
	v.w = LINTERP(t, a.w, b.w);

	for (unsigned i = 0, n = varying_count_(); i < n; ++i)
		v.varyings[i] = LINTERP(t, a.varyings[i], b.varyings[i]);

	#undef LINTERP
}

void GeometryProcessor::clip_triangles()
{
	using namespace detail;

	int mask = 0;
	for (size_t i = 0, n = vertices_.size(); i < n; ++i)
		mask |= calc_clip_mask(vertices_[i]);

	if (mask != 0) {
		for (size_t idx = 0, count = indices_.size(); idx + 3 <= count; idx += 3) {
			int vlist[2][2*6+1];
			int *inlist = vlist[0], *outlist = vlist[1];
			int n = 3;

			inlist[0] = indices_[idx];
			inlist[1] = indices_[idx + 1];
			inlist[2] = indices_[idx + 2];

			// mark this triangle as unused in case it should be completely
			// clipped
			indices_[idx] = SKIP_FLAG;
			indices_[idx + 1] = SKIP_FLAG;
			indices_[idx + 2] = SKIP_FLAG;

			POLY_CLIP(CLIP_POS_X_BIT, -1,  0,  0, 1);
			POLY_CLIP(CLIP_NEG_X_BIT,  1,  0,  0, 1);
			POLY_CLIP(CLIP_POS_Y_BIT,  0, -1,  0, 1);
			POLY_CLIP(CLIP_NEG_Y_BIT,  0,  1,  0, 1);
			POLY_CLIP(CLIP_POS_Z_BIT,  0,  0, -1, 1);
			POLY_CLIP(CLIP_NEG_Z_BIT,  0,  0,  1, 1);

			// transform the poly in inlist into triangles
			indices_[idx] = inlist[0];
			indices_[idx + 1] = inlist[1];
			indices_[idx + 2] = inlist[2];
			for (int i = 3; i < n; ++i) {
				indices_.push_back(inlist[0]);
				indices_.push_back(inlist[i - 1]);
				indices_.push_back(inlist[i]);
			}
		}
	}
}


// perspective divide and viewport transform of vertices
void GeometryProcessor::pdiv_and_vt()
{
	using namespace detail;

	static_vector<bool, MAX_VERTICES_INDICES> already_processed;
	already_processed.resize(vertices_.size(), false);

	for (size_t i = 0; i < indices_.size(); ++i) {
		// don't process triangles which are marked as unused by clipping
		if (indices_[i] == SKIP_FLAG) continue;

		if (!already_processed[indices_[i]]) {
			// perspective divide
			VertexOutput &v = vertices_[indices_[i]];
#if 0
			v.x = fixdiv<16>(v.x, v.w);
			v.y = fixdiv<16>(v.y, v.w);
			v.z = fixdiv<16>(v.z, v.w);
#else
			// this is a a bit faster but may loose precision especially if w is
			// large
			int oow = invert(v.w);
			v.x = fixmul<16>(v.x, oow);
			v.y = fixmul<16>(v.y, oow);
			v.z = fixmul<16>(v.z, oow);
#endif

			// triangle setup (x and y are converted from 16.16 to 28.4)
			v.x = (viewport_.px * v.x + viewport_.ox) >> 12;

			// y needs to be flipped since the viewport has (0,0) in the
			// upper left but vs output is like in OpenGL
			v.y = (viewport_.py * -v.y + viewport_.oy) >> 12;
			v.z = fixmul<16>(depth_range_.fmndiv2,v.z) + depth_range_.npfdiv2;

			already_processed[indices_[i]] = true;
		}
	}
}

void GeometryProcessor::process_triangles()
{
	clip_triangles();
	pdiv_and_vt();

	// compute facing and possibly cull and then draw the triangles
	for (size_t i = 0; i + 3 <= indices_.size(); i += 3) {
		// don't process triangles which are marked as unused by clipping
		if (indices_[i] == detail::SKIP_FLAG) continue;

		VertexOutput &v0 = vertices_[indices_[i]];
		VertexOutput &v1 = vertices_[indices_[i + 1]];
		VertexOutput &v2 = vertices_[indices_[i + 2]];

		// here x and y are in 28.4 fixed point. I don't use the fixmul<4>
		// here since these coordinates are clipped to the viewport and
		// therefore are sufficiently small to not overflow.
		int facing = (v0.x-v1.x)*(v2.y-v1.y)-(v2.x-v1.x)*(v0.y-v1.y);
		if (facing > 0) {
			if (cull_mode_ != CULL_CCW)
				rasterizer_->draw_triangle(v0, v1, v2);
		}
		else {
			if (cull_mode_ != CULL_CW)
				rasterizer_->draw_triangle(v2, v1, v0);
		}
	}

	vertices_.clear();
	indices_.clear();
}

////////////////////////////////////////////////////////////////////////////////

void GeometryProcessor::clip_lines()
{
	using namespace detail;

	int mask = 0;
	detail::static_vector<int, MAX_LINES * 2> clip_mask(vertices_.size(), 0);
	for (size_t i = 0, n = vertices_.size(); i < n; ++i)
	{
		clip_mask[i] = calc_clip_mask(vertices_[i]);
		mask |= clip_mask[i];
	}

	if (mask == 0)
		return;

	for (size_t i = 0, n = indices_.size(); i < n; i += 2) {
		const int v0 = indices_[i];
		const int v1 = indices_[i + 1];
		int t0 = 0;
		int t1 = 0;

		// Mark unused in case of early termination
		// of the macros below. (When fully clipped)
		indices_[i] = SKIP_FLAG;
		indices_[i + 1] = SKIP_FLAG;

		LINE_CLIP(CLIP_POS_X_BIT, -1,  0,  0, 1);
		LINE_CLIP(CLIP_NEG_X_BIT,  1,  0,  0, 1);
		LINE_CLIP(CLIP_POS_Y_BIT,  0, -1,  0, 1);
		LINE_CLIP(CLIP_NEG_Y_BIT,  0,  1,  0, 1);
		LINE_CLIP(CLIP_POS_Z_BIT,  0,  0, -1, 1);
		LINE_CLIP(CLIP_NEG_Z_BIT,  0,  0,  1, 1);

		// Restore the old values as this line
		// was not fully clipped.
		indices_[i] = v0;
		indices_[i + 1] = v1;

		if (clip_mask[v0]) {
			add_interp_vertex(t0, v0, v1);
			indices_[i] = (int)(vertices_.size() - 1);
		}

		if (clip_mask[v1]) {
			add_interp_vertex(t1, v1, v0);
			indices_[i+1] = (int)(vertices_.size() - 1);
		}
	}
}

void GeometryProcessor::process_lines()
{
	clip_lines();
	pdiv_and_vt();

	for (size_t i = 0; i + 2 <= indices_.size(); i += 2) {
		if (indices_[i] == detail::SKIP_FLAG) continue;

		VertexOutput &v0 = vertices_[indices_[i]];
		VertexOutput &v1 = vertices_[indices_[i + 1]];

		rasterizer_->draw_line(v0, v1);
	}

	vertices_.clear();
	indices_.clear();
}

////////////////////////////////////////////////////////////////////////////////


void GeometryProcessor::clip_points()
{
	int mask = 0;
	detail::static_vector<int, MAX_POINTS> clip_mask(vertices_.size(), 0);
	for (size_t i = 0, n = vertices_.size(); i < n; ++i)
	{
		clip_mask[i] = calc_clip_mask(vertices_[i]);
		mask |= clip_mask[i];
	}

	if (mask == 0)
		return;

	for (size_t i = 0, n = indices_.size(); i < n; ++i)
		if (clip_mask[indices_[i]])
			indices_[i] = detail::SKIP_FLAG;
}

void GeometryProcessor::process_points()
{
	clip_points();
	pdiv_and_vt();

	for (size_t i = 0; i < indices_.size(); ++i) {
		if (indices_[i] == detail::SKIP_FLAG) continue;

		VertexOutput &v0 = vertices_[indices_[i]];

		rasterizer_->draw_point(v0);
	}

	vertices_.clear();
	indices_.clear();
}

////////////////////////////////////////////////////////////////////////////////
// public interface

void GeometryProcessor::viewport(int x, int y, int w, int h)
{
	viewport_.px = w / 2;
	viewport_.py = h / 2;

	// the origin is stored in fixed point so it does not need to be
	// converted later.
	viewport_.ox = (x + viewport_.px) << 16;
	viewport_.oy = (y + viewport_.py) << 16;
}

void GeometryProcessor::depth_range(int n, int f)
{
	depth_range_.fmndiv2 = (f - n)/2;
	depth_range_.npfdiv2 = (n + f)/2;
}

void GeometryProcessor::vertex_attrib_pointer(int n, int stride, const void* buffer)
{
	Base::vertex_attrib_pointer(n, stride, buffer);
}

void GeometryProcessor::draw_triangles(unsigned count, unsigned *indices)
{
	draw_mode_ = DM_TRIANGLES;
	Base::process(count, indices);
}

void GeometryProcessor::draw_lines(unsigned count, unsigned *indices)
{
	draw_mode_ = DM_LINES;
	Base::process(count, indices);
}

void GeometryProcessor::draw_points(unsigned count, unsigned *indices)
{
	draw_mode_ = DM_POINTS;
	Base::process(count, indices);
}

void GeometryProcessor::cull_mode(CullMode m)
{
	cull_mode_ = m;
}

} // end namespace swr
