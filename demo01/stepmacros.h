/*
 *
 */

#include "duffsdevice.h"

// LHS and RHS are supposed to be FragmentData structures. OP can be any
// operation.
#define FRAGMENTDATA_APPLY(SHADER, LHS, OP, RHS) \
{ \
	if (SHADER::interpolate_z) \
		LHS.z OP RHS.z; \
	DUFFS_DEVICE8( \
		int i = 0, \
		LHS.varyings[i] OP RHS.varyings[i]; i++,  \
		SHADER::varying_count, \
		/**/) \
}

#define FRAGMENTDATA_PERSPECTIVE_APPLY(SHADER, LHS, OP, RHS) \
{ \
	if (SHADER::varying_count) \
		LHS.oow OP RHS.oow; \
	FRAGMENTDATA_APPLY(SHADER, LHS.fd, OP, RHS.fd) \
}
