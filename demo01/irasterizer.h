/*
 *
 */

#ifndef IRASTERIZER_8B18E9A9_B88A_4f69_9E6B_99A9EFCDE545
#define IRASTERIZER_8B18E9A9_B88A_4f69_9E6B_99A9EFCDE545

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

namespace swr {

	class IRasterizer {
	public:
		virtual ~IRasterizer() {};

		static const int MAX_VARYING = 8;

		// Type definitions
		struct Vertex {
			int x, y; // in 28.4 fixed point
			int z; // range from 0 to 0x7fffffff
			int w; // in 16.16 fixed point
			int varyings[MAX_VARYING];
		};

		// This is the data necessary for each fragment. It is defined here
		// as probably all rasterizers will need this.
		struct FragmentData {
			int z;
			int varyings[MAX_VARYING];
		};

		// Use for perspective spans. Defined here for convenience
		struct FragmentDataPerspective {
			int oow;
			FragmentData fd;
		};

		virtual void clip_rect(int x, int y, int w, int h) = 0;
		virtual void draw_triangle(const Vertex &v1, const Vertex &v2, const Vertex &v3) = 0;
		virtual void draw_line(const Vertex &v1, const Vertex &v2) = 0;
		virtual void draw_point(const Vertex &v1) = 0;
	};
}

#endif
