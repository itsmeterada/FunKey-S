/*
 *
 */

#ifndef DEMOCOMMON_H_552F7375_1BA4_4f12_91F4_51950EF31347
#define DEMOCOMMON_H_552F7375_1BA4_4f12_91F4_51950EF31347

#define SCREEN_WIDTH	240
#define SCREEN_HEIGHT	240

#include "vector_math.h"
#include "fixed_class.h"

using namespace vmath;
using namespace fixedpoint;

typedef fixed_point<16> fixed16_t;

typedef vec2<fixed16_t> vec2x;
typedef vec3<fixed16_t> vec3x;
typedef vec4<fixed16_t> vec4x;
typedef mat4<fixed16_t> mat4x;

#define X(a) fixed16_t(a)

#include <algorithm>


inline void fade_effect(SDL_Surface *s, bool ltr, float t)
{
	for (int y = 0; y < SCREEN_HEIGHT; y += 16) {
		for (int x = 0; x < SCREEN_WIDTH; x += 16) {

			float sz;
			if (ltr)
				sz = ((1.0f - (x/304.0f)) -1.0f + t * 2.0f);
			else
				sz = ((x/304.0f) -1.0f + t * 2.0f);

			int size = std::max(0, std::min(16, (int)(sz * 16 + 0.5f)));

			SDL_Rect r = {x + 8 - size / 2, y + 8 - size / 2, size, size};
			SDL_FillRect(s, &r, 0);
		}
	}
}

#endif
