/*
 *
 */

#ifndef DUFFS_DEVICE_346D4123_8A0B_41e8_A0D0_86B9318204FA
#define DUFFS_DEVICE_346D4123_8A0B_41e8_A0D0_86B9318204FA

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#define DUFFS_DEVICE2(BEFORE_LOOP, LOOP_INSTRUCTION, LOOP_COUNT, AFTER_LOOP) \
{ \
	BEFORE_LOOP; \
	int __n = (LOOP_COUNT); \
	if (__n > 0) { \
		switch (__n & 1) { \
			case 0: do	{	LOOP_INSTRUCTION; \
			case 1: 		LOOP_INSTRUCTION; \
					} while ((__n -= 2) > 0); \
		} \
	} \
	AFTER_LOOP; \
}

#define DUFFS_DEVICE4(BEFORE_LOOP, LOOP_INSTRUCTION, LOOP_COUNT, AFTER_LOOP) \
{ \
	BEFORE_LOOP; \
	int __n = (LOOP_COUNT); \
	if (__n > 0) { \
		switch (__n & 3) { \
			case 0: do	{	LOOP_INSTRUCTION; \
			case 3: 		LOOP_INSTRUCTION; \
			case 2: 		LOOP_INSTRUCTION; \
			case 1: 		LOOP_INSTRUCTION; \
					} while ((__n -= 4) > 0); \
		} \
	} \
	AFTER_LOOP; \
}

#define DUFFS_DEVICE8(BEFORE_LOOP, LOOP_INSTRUCTION, LOOP_COUNT, AFTER_LOOP) \
{ \
	BEFORE_LOOP; \
	int __n = (LOOP_COUNT); \
	if (__n > 0) { \
		switch (__n & 7) { \
			case 0: do	{	LOOP_INSTRUCTION; \
			case 7: 		LOOP_INSTRUCTION; \
			case 6: 		LOOP_INSTRUCTION; \
			case 5: 		LOOP_INSTRUCTION; \
			case 4: 		LOOP_INSTRUCTION; \
			case 3: 		LOOP_INSTRUCTION; \
			case 2: 		LOOP_INSTRUCTION; \
			case 1: 		LOOP_INSTRUCTION; \
					} while ((__n -= 8) > 0); \
		} \
	} \
	AFTER_LOOP; \
}

#define DUFFS_DEVICE16(BEFORE_LOOP, LOOP_INSTRUCTION, LOOP_COUNT, AFTER_LOOP) \
{ \
	BEFORE_LOOP; \
	int __n = (LOOP_COUNT); \
	if (__n > 0) { \
		switch (__n & 15) { \
			case 0: do	{	LOOP_INSTRUCTION; \
			case 15: 		LOOP_INSTRUCTION; \
			case 14: 		LOOP_INSTRUCTION; \
			case 13: 		LOOP_INSTRUCTION; \
			case 12: 		LOOP_INSTRUCTION; \
			case 11: 		LOOP_INSTRUCTION; \
			case 10: 		LOOP_INSTRUCTION; \
			case 9: 		LOOP_INSTRUCTION; \
			case 8: 		LOOP_INSTRUCTION; \
			case 7: 		LOOP_INSTRUCTION; \
			case 6: 		LOOP_INSTRUCTION; \
			case 5: 		LOOP_INSTRUCTION; \
			case 4: 		LOOP_INSTRUCTION; \
			case 3: 		LOOP_INSTRUCTION; \
			case 2: 		LOOP_INSTRUCTION; \
			case 1: 		LOOP_INSTRUCTION; \
					} while ((__n -= 16) > 0); \
		} \
	} \
	AFTER_LOOP; \
}

#endif
