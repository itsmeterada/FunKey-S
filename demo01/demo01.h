/*
 *
 */

#ifndef __DEMO01_H__
#define __DEMO01_H__

#include "SDL.h"
#include "SDL_image.h"
#include "geometry_processor.h"
#include "rasterizer_subdivaffine.h"
#include "span.h"
#include "vector_math.h"
#include "fixed_func.h"
#include "democommon.h"

#include "myfixed_func.h"
#include "util.h"

using namespace swr;


void demo01();


namespace {
	SDL_Surface *screen;
	SDL_Surface *zbuffer;



	class Texture {
		SDL_Surface *tex;
	public:
		Texture()
		{
			SDL_Surface *tmp = IMG_Load("lava.png");
			tex = SDL_DisplayFormat(tmp);
			SDL_FreeSurface(tmp);
		}

		~Texture()
		{
			SDL_FreeSurface(tex);
		}

		unsigned short sample(int x, int y)
		{
			int tx = x >> (24 - 8);
			int ty = y >> (24 - 8);

			tx &= 256 - 1;
			ty &= 256 - 1;

			return *((unsigned short*)tex->pixels + tx + (ty << 8));
		}
	};

	Texture *tex;

	mat4x modelviewprojection_matrix;
	vec3x pos;


	class VertexShader {
		struct InputVertex {
			vec3x vertex;
			int tx, ty;
		};

	public:
		static const unsigned attribute_count = 1;
		static const unsigned varying_count = 2;

	public:
		static void shade(const GeometryProcessor::VertexInput in, GeometryProcessor::VertexOutput &out)
		{
			const InputVertex &i = *static_cast<const InputVertex*>(in[0]);

			vec4x tvertex = modelviewprojection_matrix * vec4x(i.vertex, X(1.0f));

			out.x = tvertex.x.intValue;
			out.y = tvertex.y.intValue;
			out.z = tvertex.z.intValue;
			out.w = tvertex.w.intValue;
			out.varyings[0] = i.tx << 8;
			out.varyings[1] = i.ty << 8;
		}
	};


	class FragmentShader : public GenericSpanDrawer<FragmentShader> {
	public:

		static const unsigned varying_count = 2;
		static const bool interpolate_z = false;

		// per triangle callback
		static void begin_triangle(
			const IRasterizer::Vertex& v1,
			const IRasterizer::Vertex& v2,
			const IRasterizer::Vertex& v3,
			int area2)
		{}

		static void single_fragment(int x, int y, const IRasterizer::FragmentData &fd)
		{
			unsigned short* color_buffer = (unsigned short*)screen->pixels +
				(x) + (y)*screen->w;

			*color_buffer = tex->sample(fd.varyings[0], fd.varyings[1]);
		}
	};

	class StarVertexShader {
		struct InputVertex {
			vec3x vertex;
			vec3x normal;
		};

	public:
		static const unsigned attribute_count = 1;
		static const unsigned varying_count = 3;

	public:
		static void shade(const GeometryProcessor::VertexInput in, GeometryProcessor::VertexOutput &out)
		{
			const InputVertex &i = *static_cast<const InputVertex*>(in[0]);

			vec4x tvertex = modelviewprojection_matrix * vec4x(i.vertex, X(1.0f));
			vec3x r = normalize(reflect(i.vertex - pos, i.normal));

			out.x = tvertex.x.intValue;
			out.y = tvertex.y.intValue;
			out.z = tvertex.z.intValue;
			out.w = tvertex.w.intValue;
			out.varyings[0] = r.x.intValue << 8;
			out.varyings[1] = r.y.intValue << 8;
			out.varyings[2] = r.z.intValue << 8;
		}
	};


	class StarFragmentShader : public GenericSpanDrawer<StarFragmentShader> {
	public:

		static const unsigned varying_count = 3;
		static const bool interpolate_z = true;

		// per triangle callback
		static void begin_triangle(
			const IRasterizer::Vertex& v1,
			const IRasterizer::Vertex& v2,
			const IRasterizer::Vertex& v3,
			int area2)
		{}

		static void single_fragment(int x, int y, const IRasterizer::FragmentData &fd)
		{
			unsigned short* color_buffer = (unsigned short*)screen->pixels +
				(x) + (y)*screen->w;

			unsigned short* zb = (unsigned short*)zbuffer->pixels +
				(x) + (y)*zbuffer->w;

			unsigned short z = fd.z >> 16;
			if ( z < *zb) {
				int tx = 0, ty = 0;
				int coord[3] = { fd.varyings[0], fd.varyings[1],
					fd.varyings[2]};

				int ma = 0;
				//if (std::abs(coord[1]) > std::abs(coord[ma])) ma = 1;
				//if (std::abs(coord[2]) > std::abs(coord[ma])) ma = 2;
				if (abs(coord[1]) > abs(coord[ma])) ma = 1;
				if (abs(coord[2]) > abs(coord[ma])) ma = 2;

				switch (ma) {
				case 0:
					if (coord[0] > 0) tx = -coord[2];
					else tx = coord[2];
					ty = -coord[1];
					break;
				case 1:
					if (coord[1] > 0) ty = coord[1];
					else ty = -coord[1];
					tx = coord[0];
					break;
				case 2:
					if (coord[2] > 0) tx = coord[0];
					else tx = -coord[0];
					ty = -coord[1];
					break;
				}

				tx += (1 << 23);
				ty += (1 << 23);

				unsigned short color = tex->sample(tx, ty);
				color &= ~(1 << 5| 1 << 11);
				color >>= 1;
				*color_buffer = color;
				*zb = z;
			}
		}
	};

  float cube_vertices[] = {
  -1.000000f, 1.000000f, -1.000000f, 0.000000f, 1.000000f,
  1.000000f, 1.000000f, -1.000000f, 0.000000f, 0.000000f,
  1.000000f, 0.999999f, 1.000000f, 1.000000f, 0.000000f,
  -1.000000f, 1.000000f, -1.000000f, 0.000000f, 1.000000f,
  1.000000f, 0.999999f, 1.000000f, 1.000000f, 0.000000f,
  -1.000000f, 1.000000f, 1.000000f, 1.000000f, 1.000000f,
  -1.000000f, -1.000000f, -1.000000f, 0.000000f, 1.000000f,
  -1.000000f, 1.000000f, -1.000000f, 0.000000f, 0.000000f,
  -1.000000f, 1.000000f, 1.000000f, 1.000000f, 0.000000f,
  -1.000000f, -1.000000f, -1.000000f, 0.000000f, 1.000000f,
  -1.000000f, 1.000000f, 1.000000f, 1.000000f, 0.000000f,
  -1.000000f, -1.000000f, 1.000000f, 1.000000f, 1.000000f,
  1.000000f, -1.000000f, -1.000000f, 0.000000f, 1.000000f,
  -1.000000f, -1.000000f, -1.000000f, 0.000000f, 0.000000f,
  0.999999f, -1.000001f, 1.000000f, 1.000000f, 1.000000f,
  -1.000000f, -1.000000f, -1.000000f, 0.000000f, 0.000000f,
  -1.000000f, -1.000000f, 1.000000f, 1.000000f, 0.000000f,
  0.999999f, -1.000001f, 1.000000f, 1.000000f, 1.000000f,
  1.000000f, 1.000000f, -1.000000f, 0.000000f, 1.000000f,
1.000000f, -1.000000f, -1.000000f, 0.000000f, 0.000000f,
  1.000000f, 0.999999f, 1.000000f, 1.000000f, 1.000000f,
  1.000000f, -1.000000f, -1.000000f, 0.000000f, 0.000000f,
  0.999999f, -1.000001f, 1.000000f, 1.000000f, 0.000000f,
  1.000000f, 0.999999f, 1.000000f, 1.000000f, 1.000000f,
  1.000000f, 0.999999f, 1.000000f, 0.000000f, 1.000000f,
  0.999999f, -1.000001f, 1.000000f, 0.000000f, 0.000000f,
  -1.000000f, 1.000000f, 1.000000f, 1.000000f, 1.000000f,
  0.999999f, -1.000001f, 1.000000f, 0.000000f, 0.000000f,
  -1.000000f, -1.000000f, 1.000000f, 1.000000f, 0.000000f,
  -1.000000f, 1.000000f, 1.000000f, 1.000000f, 1.000000f,
  1.000000f, 1.000000f, -1.000000f, 0.000000f, 1.000000f,
  -1.000000f, 1.000000f, -1.000000f, 0.000000f, 0.000000f,
  -1.000000f, -1.000000f, -1.000000f, 1.000000f, 0.000000f,
  1.000000f, 1.000000f, -1.000000f, 0.000000f, 1.000000f,
  -1.000000f, -1.000000f, -1.000000f, 1.000000f, 0.000000f,
  1.000000f, -1.000000f, -1.000000f, 1.000000f, 1.000000f,
};

unsigned cube_indices[] = {
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
  32,
  33,
  34,
  35,

};

float star_vertices[] = {
  0.000000f, 0.000000f, 0.200000f, 0.894427f, 0.000000f, 0.447214f,
  0.100000f, -0.100000f, 0.000000f, 0.894427f, 0.000000f, 0.447214f,
  0.100000f, 0.100000f, 0.000000f, 0.894427f, 0.000000f, 0.447214f,
  0.100000f, -0.100000f, 0.000000f, 0.000000f, -0.894427f, 0.447214f,
  0.000000f, 0.000000f, 0.200000f, 0.000000f, -0.894427f, 0.447214f,
  -0.100000f, -0.100000f, 0.000000f, 0.000000f, -0.894427f, 0.447214f,
  -0.100000f, -0.100000f, 0.000000f, -0.894427f, 0.000000f, 0.447214f,
  0.000000f, 0.000000f, 0.200000f, -0.894427f, 0.000000f, 0.447214f,
  -0.100000f, 0.100000f, 0.000000f, -0.894427f, 0.000000f, 0.447214f,
  0.000000f, 0.000000f, 0.200000f, 0.000000f, 0.894427f, 0.447214f,
  0.100000f, 0.100000f, 0.000000f, 0.000000f, 0.894427f, 0.447214f,
  -0.100000f, 0.100000f, 0.000000f, 0.000000f, 0.894427f, 0.447214f,
  0.300000f, 0.000000f, -0.100000f, 0.447214f, 0.894427f, 0.000000f,
  0.100000f, 0.100000f, -0.200000f, 0.447214f, 0.894427f, 0.000000f,
  0.100000f, 0.100000f, 0.000000f, 0.447214f, 0.894427f, 0.000000f,
  0.100000f, -0.100000f, 0.000000f, 0.447213f, 0.000000f, 0.894427f,
  0.300000f, 0.000000f, -0.100000f, 0.447213f, 0.000000f, 0.894427f,
  0.100000f, 0.100000f, 0.000000f, 0.447213f, 0.000000f, 0.894427f,
  0.300000f, 0.000000f, -0.100000f, 0.447213f, -0.894427f, 0.000000f,
  0.100000f, -0.100000f, 0.000000f, 0.447213f, -0.894427f, 0.000000f,
  0.100000f, -0.100000f, -0.200000f, 0.447213f, -0.894427f, 0.000000f,
  0.100000f, 0.100000f, -0.200000f, 0.447213f, 0.000000f, -0.894427f,
  0.300000f, 0.000000f, -0.100000f, 0.447213f, 0.000000f, -0.894427f,
  0.100000f, -0.100000f, -0.200000f, 0.447213f, 0.000000f, -0.894427f,
  -0.100000f, 0.100000f, 0.000000f, -0.447213f, 0.000000f, 0.894427f,
  -0.300000f, 0.000000f, -0.100000f, -0.447213f, 0.000000f, 0.894427f,
  -0.100000f, -0.100000f, 0.000000f, -0.447213f, 0.000000f, 0.894427f,
  -0.100000f, -0.100000f, 0.000000f, -0.447214f, -0.894427f, 0.000000f,
  -0.300000f, 0.000000f, -0.100000f, -0.447214f, -0.894427f, 0.000000f,
  -0.100000f, -0.100000f, -0.200000f, -0.447214f, -0.894427f, 0.000000f,
  -0.100000f, -0.100000f, -0.200000f, -0.447214f, 0.000000f, -0.894427f,
  -0.300000f, 0.000000f, -0.100000f, -0.447214f, 0.000000f, -0.894427f,
  -0.100000f, 0.100000f, -0.200000f, -0.447214f, 0.000000f, -0.894427f,
  -0.300000f, 0.000000f, -0.100000f, -0.447214f, 0.894427f, 0.000000f,
  -0.100000f, 0.100000f, 0.000000f, -0.447214f, 0.894427f, 0.000000f,
  -0.100000f, 0.100000f, -0.200000f, -0.447214f, 0.894427f, 0.000000f,
  0.000000f, 0.000000f, -0.400000f, 0.000000f, 0.894427f, -0.447214f,
  -0.100000f, 0.100000f, -0.200000f, 0.000000f, 0.894427f, -0.447214f,
  0.100000f, 0.100000f, -0.200000f, 0.000000f, 0.894427f, -0.447214f,
  0.100000f, -0.100000f, -0.200000f, 0.894427f, 0.000000f, -0.447213f,
  0.000000f, 0.000000f, -0.400000f, 0.894427f, 0.000000f, -0.447213f,
  0.100000f, 0.100000f, -0.200000f, 0.894427f, 0.000000f, -0.447213f,
  -0.100000f, -0.100000f, -0.200000f, 0.000000f, -0.894427f, -0.447214f,
  0.000000f, 0.000000f, -0.400000f, 0.000000f, -0.894427f, -0.447214f,
  0.100000f, -0.100000f, -0.200000f, 0.000000f, -0.894427f, -0.447214f,
  -0.100000f, 0.100000f, -0.200000f, -0.894427f, 0.000000f, -0.447214f,
  0.000000f, 0.000000f, -0.400000f, -0.894427f, 0.000000f, -0.447214f,
  -0.100000f, -0.100000f, -0.200000f, -0.894427f, 0.000000f, -0.447214f,
  -0.100000f, -0.100000f, -0.200000f, -0.894427f, -0.447214f, 0.000000f,
  0.000000f, -0.300000f, -0.100000f, -0.894427f, -0.447214f, 0.000000f,
  -0.100000f, -0.100000f, 0.000000f, -0.894427f, -0.447214f, 0.000000f,
  0.000000f, -0.300000f, -0.100000f, 0.000000f, -0.447213f, 0.894427f,
  0.100000f, -0.100000f, 0.000000f, 0.000000f, -0.447213f, 0.894427f,
  -0.100000f, -0.100000f, 0.000000f, 0.000000f, -0.447213f, 0.894427f,
  0.100000f, -0.100000f, 0.000000f, 0.894427f, -0.447214f, 0.000000f,
  0.000000f, -0.300000f, -0.100000f, 0.894427f, -0.447214f, 0.000000f,
  0.100000f, -0.100000f, -0.200000f, 0.894427f, -0.447214f, 0.000000f,
  0.000000f, -0.300000f, -0.100000f, 0.000000f, -0.447214f, -0.894427f,
  -0.100000f, -0.100000f, -0.200000f, 0.000000f, -0.447214f, -0.894427f,
  0.100000f, -0.100000f, -0.200000f, 0.000000f, -0.447214f, -0.894427f,
  0.000000f, 0.300000f, -0.100000f, 0.000000f, 0.447214f, 0.894427f,
  -0.100000f, 0.100000f, 0.000000f, 0.000000f, 0.447214f, 0.894427f,
  0.100000f, 0.100000f, 0.000000f, 0.000000f, 0.447214f, 0.894427f,
  0.100000f, 0.100000f, -0.200000f, 0.894427f, 0.447214f, 0.000000f,
  0.000000f, 0.300000f, -0.100000f, 0.894427f, 0.447214f, 0.000000f,
  0.100000f, 0.100000f, 0.000000f, 0.894427f, 0.447214f, 0.000000f,
  -0.100000f, 0.100000f, -0.200000f, 0.000000f, 0.447213f, -0.894427f,
  0.000000f, 0.300000f, -0.100000f, 0.000000f, 0.447213f, -0.894427f,
  0.100000f, 0.100000f, -0.200000f, 0.000000f, 0.447213f, -0.894427f,
  -0.100000f, 0.100000f, 0.000000f, -0.894427f, 0.447214f, 0.000000f,
  0.000000f, 0.300000f, -0.100000f, -0.894427f, 0.447214f, 0.000000f,
  -0.100000f, 0.100000f, -0.200000f, -0.894427f, 0.447214f, 0.000000f,
};

unsigned star_indices[] = {
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
  32,
  33,
  34,
  35,
  36,
  37,
  38,
  39,
  40,
  41,
  42,
  43,
  44,
  45,
  46,
  47,
  48,
  49,
  50,
  51,
  52,
  53,
  54,
  55,
  56,
  57,
  58,
  59,
  60,
  61,
  62,
  63,
  64,
  65,
  66,
  67,
  68,
  69,
  70,
  71,
};


} // end namespace

#endif // __DEMO01_H__
